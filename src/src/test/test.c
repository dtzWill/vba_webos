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
#include <time.h>
#include <stdlib.h>

extern int Z_FLAG,
           N_FLAG,
           C_FLAG,
           V_FLAG;

extern int op1, op2, op3;

void adds_c();
void adds_asm();


int main( int argc, char ** argv )
{
    //Correctness
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

                    //printf( "Testing: %d, %d, %d\n", op1, op2, op3 );


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

                    //printf( "\tresults c: %d, %d, %d\t%d, %d, %d, %d\n",
                    //        o11, o12, o13,
                    //        n1, z1, c1, v1
                    //        );
                    //printf( "\tresults a: %d, %d, %d\t%d, %d, %d, %d\n",
                    //        o21, o22, o23,
                    //        n2, z2, c2, v2
                    //        );

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

    //Speed
    clock_t s1, s2, e1, e2;
    int runs = atoi( argv[1] );
    s1 = clock();
    for ( int i = 0; i < runs; i++ )
    {
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        adds_c();
        op2 = op1 + op3;
    }
    e1 = clock();
    //This is mainly to prevent the compiler from removing the loop above
    int t1 = (e1 - s1);
    printf( "%d, %d, %d, %d\n", op1, op2, op3, t1 );
    s2 = clock();
    for ( int i = 0; i < runs; i++ )
    {
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        adds_asm();
        op2 = op1 + op3;
    }
    e2 = clock();
    int t2 = (e2 - s2);
    printf( "%d, %d, %d, %d\n", op1, op2, op3, t2 );

    printf( "Timings: %d, %d, Speedup (bigger is better): %f\n",
            t1, t2, (float)t1/t2 );
}
