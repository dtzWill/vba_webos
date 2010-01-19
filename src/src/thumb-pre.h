/*
 * ===========================================================================
 *
 *       Filename:  thumb-pre.h
 *
 *    Description:  Thumb ARM implementations
 *
 *        Version:  1.0
 *        Created:  01/19/2010 02:37:32 AM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

/*
 * similar to arm-new-pre.h, this file contains
 * the c implementation, the x86 implementation, and a
 * WIP arm implementation.
 * This is for my own convenience, and to assist the porting
 * and catching of bugs, even if it's a bit cluttered.
 */

//Some c-core helper macros
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


/*-----------------------------------------------------------------------------
 *  Thumb macro definitions
 *-----------------------------------------------------------------------------*/



#define ADD_RD_RS_RN \
   {\
     u32 lhs = reg[source].I;\
     u32 rhs = value;\
     u32 res = lhs + rhs;\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     ADDCARRY(lhs, rhs, res);\
     ADDOVERFLOW(lhs, rhs, res);\
   }
//#define ADD_RD_RS_RN \
//     asm ("add %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setcb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[dest].I)\
//          : "r" (value), "b" (reg[source].I));
#define ADD_RD_RS_O3 \
   {\
     u32 lhs = reg[source].I;\
     u32 rhs = value;\
     u32 res = lhs + rhs;\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     ADDCARRY(lhs, rhs, res);\
     ADDOVERFLOW(lhs, rhs, res);\
   }
//#define ADD_RD_RS_O3 \
//     asm ("add %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setcb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[dest].I)\
//          : "r" (value), "b" (reg[source].I));
#define ADD_RN_O8(d) \
   {\
     u32 lhs = reg[(d)].I;\
     u32 rhs = (opcode & 255);\
     u32 res = lhs + rhs;\
     reg[(d)].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     ADDCARRY(lhs, rhs, res);\
     ADDOVERFLOW(lhs, rhs, res);\
   }
//#define ADD_RN_O8(d) \
//     asm ("add %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setcb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[(d)].I)\
//          : "r" (opcode & 255), "b" (reg[(d)].I));
#define CMN_RD_RS \
   {\
     u32 lhs = reg[dest].I;\
     u32 rhs = value;\
     u32 res = lhs + rhs;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     ADDCARRY(lhs, rhs, res);\
     ADDOVERFLOW(lhs, rhs, res);\
   }
//#define CMN_RD_RS \
//     asm ("add %0, %1;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setcb C_FLAG;"\
//          "setob V_FLAG;"\
//          : \
//          : "r" (value), "r" (reg[dest].I):"1");
#define ADC_RD_RS \
   {\
     u32 lhs = reg[dest].I;\
     u32 rhs = value;\
     u32 res = lhs + rhs + (u32)C_FLAG;\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     ADDCARRY(lhs, rhs, res);\
     ADDOVERFLOW(lhs, rhs, res);\
   }
//#define ADC_RD_RS \
//     asm ("bt $0, C_FLAG;"\
//          "adc %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setcb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[dest].I)\
//          : "r" (value), "b" (reg[dest].I));
#define SUB_RD_RS_RN \
   {\
     u32 lhs = reg[source].I;\
     u32 rhs = value;\
     u32 res = lhs - rhs;\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(lhs, rhs, res);\
     SUBOVERFLOW(lhs, rhs, res);\
   }
//#define SUB_RD_RS_RN \
//     asm ("sub %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setncb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[dest].I)\
//          : "r" (value), "b" (reg[source].I));
#define SUB_RD_RS_O3 \
   {\
     u32 lhs = reg[source].I;\
     u32 rhs = value;\
     u32 res = lhs - rhs;\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(lhs, rhs, res);\
     SUBOVERFLOW(lhs, rhs, res);\
   }
//#define SUB_RD_RS_O3 \
//     asm ("sub %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setncb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[dest].I)\
//          : "r" (value), "b" (reg[source].I));
#define SUB_RN_O8(d) \
   {\
     u32 lhs = reg[(d)].I;\
     u32 rhs = (opcode & 255);\
     u32 res = lhs - rhs;\
     reg[(d)].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(lhs, rhs, res);\
     SUBOVERFLOW(lhs, rhs, res);\
   }
//#define SUB_RN_O8(d) \
//     asm ("sub %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setncb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[(d)].I)\
//          : "r" (opcode & 255), "b" (reg[(d)].I));
#define CMP_RN_O8(d) \
   {\
     u32 lhs = reg[(d)].I;\
     u32 rhs = (opcode & 255);\
     u32 res = lhs - rhs;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(lhs, rhs, res);\
     SUBOVERFLOW(lhs, rhs, res);\
   }
//#define CMP_RN_O8(d) \
//     asm ("sub %0, %1;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setncb C_FLAG;"\
//          "setob V_FLAG;"\
//          : \
//          : "r" (opcode & 255), "r" (reg[(d)].I) : "1");
#define SBC_RD_RS \
   {\
     u32 lhs = reg[dest].I;\
     u32 rhs = value;\
     u32 res = lhs - rhs - !((u32)C_FLAG);\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(lhs, rhs, res);\
     SUBOVERFLOW(lhs, rhs, res);\
   }
//#define SBC_RD_RS \
//     asm volatile ("bt $0, C_FLAG;"\
//                   "cmc;"\
//                   "sbb %1, %%ebx;"\
//                   "setsb N_FLAG;"\
//                   "setzb Z_FLAG;"\
//                   "setncb C_FLAG;"\
//                   "setob V_FLAG;"\
//                   : "=b" (reg[dest].I)\
//                   : "r" (value), "b" (reg[dest].I) : "cc", "memory");
#define LSL_RD_RM_I5 \
   {\
     C_FLAG = (reg[source].I >> (32 - shift)) & 1 ? true : false;\
     value = reg[source].I << shift;\
   }
//#define LSL_RD_RM_I5 \
//       asm ("shl %%cl, %%eax;"\
//            "setcb C_FLAG;"\
//            : "=a" (value)\
//            : "a" (reg[source].I), "c" (shift));
#define LSL_RD_RS \
   {\
     C_FLAG = (reg[dest].I >> (32 - value)) & 1 ? true : false;\
     value = reg[dest].I << value;\
   }
//#define LSL_RD_RS \
//         asm ("shl %%cl, %%eax;"\
//              "setcb C_FLAG;"\
//              : "=a" (value)\
//              : "a" (reg[dest].I), "c" (value));
#define LSR_RD_RM_I5 \
   {\
     C_FLAG = (reg[source].I >> (shift - 1)) & 1 ? true : false;\
     value = reg[source].I >> shift;\
   }
//#define LSR_RD_RM_I5 \
//       asm ("shr %%cl, %%eax;"\
//            "setcb C_FLAG;"\
//            : "=a" (value)\
//            : "a" (reg[source].I), "c" (shift));
#define LSR_RD_RS \
   {\
     C_FLAG = (reg[dest].I >> (value - 1)) & 1 ? true : false;\
     value = reg[dest].I >> value;\
   }
//#define LSR_RD_RS \
//         asm ("shr %%cl, %%eax;"\
//              "setcb C_FLAG;"\
//              : "=a" (value)\
//              : "a" (reg[dest].I), "c" (value));
#define ASR_RD_RM_I5 \
   {\
     C_FLAG = ((s32)reg[source].I >> (int)(shift - 1)) & 1 ? true : false;\
     value = (s32)reg[source].I >> (int)shift;\
   }
//#define ASR_RD_RM_I5 \
//     asm ("sar %%cl, %%eax;"\
//          "setcb C_FLAG;"\
//          : "=a" (value)\
//          : "a" (reg[source].I), "c" (shift));
#define ASR_RD_RS \
   {\
     C_FLAG = ((s32)reg[dest].I >> (int)(value - 1)) & 1 ? true : false;\
     value = (s32)reg[dest].I >> (int)value;\
   }
//#define ASR_RD_RS \
//         asm ("sar %%cl, %%eax;"\
//              "setcb C_FLAG;"\
//              : "=a" (value)\
//              : "a" (reg[dest].I), "c" (value));
#define ROR_RD_RS \
   {\
     C_FLAG = (reg[dest].I >> (value - 1)) & 1 ? true : false;\
     value = ((reg[dest].I << (32 - value)) |\
              (reg[dest].I >> value));\
   }
//#define ROR_RD_RS \
//         asm ("ror %%cl, %%eax;"\
//              "setcb C_FLAG;"\
//              : "=a" (value)\
//              : "a" (reg[dest].I), "c" (value));
#define NEG_RD_RS \
   {\
     u32 lhs = reg[source].I;\
     u32 rhs = 0;\
     u32 res = rhs - lhs;\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(rhs, lhs, res);\
     SUBOVERFLOW(rhs, lhs, res);\
   }
//#define NEG_RD_RS \
//     asm ("neg %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setncb C_FLAG;"\
//          "setob V_FLAG;"\
//          : "=b" (reg[dest].I)\
//          : "b" (reg[source].I));
#define CMP_RD_RS \
   {\
     u32 lhs = reg[dest].I;\
     u32 rhs = value;\
     u32 res = lhs - rhs;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(lhs, rhs, res);\
     SUBOVERFLOW(lhs, rhs, res);\
   }
//#define CMP_RD_RS \
//     asm ("sub %0, %1;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setncb C_FLAG;"\
//          "setob V_FLAG;"\
//          : \
//          : "r" (value), "r" (reg[dest].I):"1");
