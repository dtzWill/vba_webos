/*
 * ===========================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  Testing arm emulation code
 *
 *        Version:  1.0
 *        Created:  01/19/2010 12:51:22 PM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#include <assert.h>
#include <stdio.h>

int Z_FLAG,
           N_FLAG,
           C_FLAG,
           V_FLAG;

int op1, op2, op3;

#define true 1
#define false 0

#define NEG(i) ((i) >> 31)
#define POS(i) ((~(i)) >> 31)
#define ADDCARRY(a, b, c) \
  C_FLAG = ((NEG(a) & NEG(b)) |\
            (NEG(a) & POS(c)) |\
            (NEG(b) & POS(c))) ? true : false;
#define ADDOVERFLOW(a, b, c) \
  V_FLAG = ((NEG(a) & NEG(b) & POS(c)) |\
            (POS(a) & POS(b) & NEG(c))) ? true : false;
#define SUBCARRY(a, b, c) \
  C_FLAG = ((NEG(a) & POS(b)) |\
            (NEG(a) & POS(c)) |\
            (POS(b) & POS(c))) ? true : false;
#define SUBOVERFLOW(a, b, c)\
  V_FLAG = ((NEG(a) & POS(b) & POS(c)) |\
            (POS(a) & NEG(b) & NEG(c))) ? true : false;
void adds_c()
{
    int res = op2 + op3;
    op1 = res;
    Z_FLAG =  (res == 0) ? true: false;
    N_FLAG = NEG(res) ? true : false;
    ADDCARRY(op2, op3, res );
    ADDOVERFLOW( op2, op3, res );
}

void adds_asm()
{
    //Idea for flags:
    //instead of two instructions for each,
    //can we load flags /once/
    //and then use a shift/mask operation to get
    //each?
asm( "adds %0, %6, %5;"
     "movmi %1, #1;"
     "movpl %1, #0;"
     "movne %2, #0;"
     "moveq %2, #1;"
     "movcs %3, #1;"
     "movcc %3, #0;"
     "movvs %4, #1;"
     "movvc %4, #0;"
       : "=r" (op1),
        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG)
       : "r" (op2), "r" (op3));
}

int main()
{
    int vals[] = { 1<<32 - 1, 1<<31, 1<<16, 10, 1, 0 };
    int count = sizeof( vals ) / sizeof( int );
    for ( int i = 0; i < count*2; i++ )
    {
        for ( int j = 0; j < count*2; j++ )
        {
            for ( int k = 0; k < count*2; k++ )
            {
                for ( int f = 0; f < 2; f++ )
                {
                    int n1,z1,c1,v1, o11, o12, o13;
                    int n2,z2,c2,v2, o21, o22, o23;
                    op1 = vals[ i % count ] * (  (i>=count) ? -1 : 1 );
                    op2 = vals[ j % count ] * (  (j>=count) ? -1 : 1 );
                    op3 = vals[ k % count ] * (  (k>=count) ? -1 : 1 );

                    printf( "Testing: %d, %d, %d\n", op1, op2, op3 );


                    N_FLAG = Z_FLAG = C_FLAG = V_FLAG = f;

                    //run c version
                    adds_c();

                    //get results
                    n1 = N_FLAG; z1 = Z_FLAG; c1 = C_FLAG; v1 = V_FLAG;
                    o11 = op1; o12 = op2; o13 = op3;

                    op1 = vals[ i % count ] * (  (i>=count) ? -1 : 1 );
                    op2 = vals[ j % count ] * (  (j>=count) ? -1 : 1 );
                    op3 = vals[ k % count ] * (  (k>=count) ? -1 : 1 );

                    N_FLAG = Z_FLAG = C_FLAG = V_FLAG = f;

                    //run asm version
                    adds_asm();

                    n2 = N_FLAG; z2 = Z_FLAG; c2 = C_FLAG; v2 = V_FLAG;
                    o21 = op1; o22 = op2; o23 = op3;

                    printf( "\tresults c: %d, %d, %d\t%d, %d, %d, %d\n",
                            o11, o12, o13,
                            n1, z1, c1, v1
                            );
                    printf( "\tresults a: %d, %d, %d\t%d, %d, %d, %d\n",
                            o21, o22, o23,
                            n2, z2, c2, v2
                            );

                    //check results
                    assert( o11 == o21 );
                    assert( o12 == o22 );
                    assert( o13 == o23 );
                    assert( n1 == n2 );
                    assert( z1 == z2 );
                    assert( c1 == c2 );
                    assert( v1 == v2 );
                }
            }
        }
    }
}
