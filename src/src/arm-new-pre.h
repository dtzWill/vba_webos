/*
 * ===========================================================================
 *
 *       Filename:  arm-new-pre.h
 *
 *    Description:  Arm-new impl for pre
 *
 *        Version:  1.0
 *        Created:  01/18/2010 12:57:15 AM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

/**
 * This file might seem a little confusing, but it was written to facilitate
 * implementing these macros in ARM assembly.
 * For each macro, I have the (presumably perfectly correct) c
 * and x86 asm macros for reference.
 * As I port the arm ones over, the c implementions will get commented
 * and the arm ones defined.  This is so I can iteratively test and move
 * the code over little by little--having two reference implementations.
 * After I'm satisfied this works I possibly will remove this comment block
 * and the reference implementations, but then again it would be useful
 * for future people to look at, myself included.
 */

/**
 * Notes:
 * I'm not entirely sure why, but many of these implementations *don't* update
 * all the flags that the ARM reference says the corresponding instructions do.
 * I'm not really sure why that is the case, so for now I'm just following suit
 */



/*-----------------------------------------------------------------------------
 *  Some macros used by the c implementations
 *-----------------------------------------------------------------------------*/
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




//=============================================================================
#define OP_SUB \
    asm ( "sub %0, %2, %1;" \
            : "=r" (reg[dest].I) \
            : "r" (value), "r" (reg[base].I) );
//#define OP_SUB \
//    {\
//      reg[dest].I = reg[base].I - value;\
//    }
//
//#define OP_SUB \
//     asm ("sub %1, %%ebx;"\
//                  : "=b" (reg[dest].I)\
//                  : "r" (value), "b" (reg[base].I));
//
//=============================================================================
#define OP_SUBS \
     asm( "subs %0, %6, %5;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #31, #1;" \
     "ubfx %2, r3, #30, #1;" \
     "ubfx %3, r3, #29, #1;" \
     "ubfx %4, r3, #28, #1;" \
       : "=r" (reg[dest].I), \
        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
       : "r" (value), "r" (reg[base].I) \
       : "r3" );
//#define OP_SUBS \
//   {\
//     u32 lhs = reg[base].I;\
//     u32 rhs = value;\
//     u32 res = lhs - rhs;\
//     reg[dest].I = res;\
//     Z_FLAG = (res == 0) ? true : false;\
//     N_FLAG = NEG(res) ? true : false;\
//     SUBCARRY(lhs, rhs, res);\
//     SUBOVERFLOW(lhs, rhs, res);\
//   }
//#define OP_SUBS \
//     asm ("sub %1, %%ebx;"\
//          "setsb N_FLAG;"\
//          "setzb Z_FLAG;"\
//          "setncb C_FLAG;"\
//          "setob V_FLAG;"\
//                  : "=b" (reg[dest].I)\
//                  : "r" (value), "b" (reg[base].I));
//
//=============================================================================
#define OP_RSB \
    asm( "rsb %0, %2, %1;" \
            : "=r" (reg[dest].I) \
            : "r" (value), "r" (reg[base].I) );
//#define OP_RSB \
//    {\
//      reg[dest].I = value - reg[base].I;\
//    }
//#define OP_RSB \
//            asm  ("sub %1, %%ebx;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (reg[base].I), "b" (value));
//
//=============================================================================
#define OP_RSBS \
     asm( "rsbs %0, %6, %5;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #31, #1;" \
     "ubfx %2, r3, #30, #1;" \
     "ubfx %3, r3, #29, #1;" \
     "ubfx %4, r3, #28, #1;" \
       : "=r" (reg[dest].I), \
        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
       : "r" (value), "r" (reg[base].I) \
       : "r3" );
//#define OP_RSBS \
//   {\
//     u32 lhs = reg[base].I;\
//     u32 rhs = value;\
//     u32 res = rhs - lhs;\
//     reg[dest].I = res;\
//     Z_FLAG = (res == 0) ? true : false;\
//     N_FLAG = NEG(res) ? true : false;\
//     SUBCARRY(rhs, lhs, res);\
//     SUBOVERFLOW(rhs, lhs, res);\
//   }
//#define OP_RSBS \
//            asm  ("sub %1, %%ebx;"\
//                  "setsb N_FLAG;"\
//                  "setzb Z_FLAG;"\
//                  "setncb C_FLAG;"\
//                  "setob V_FLAG;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (reg[base].I), "b" (value));
//
//=============================================================================
#define OP_ADD \
    asm( "add %0, %2, %1;" \
            : "=r" (reg[dest].I) \
            : "r" (value), "r" (reg[base].I) );
//#define OP_ADD \
//    {\
//      reg[dest].I = reg[base].I + value;\
//    }
//#define OP_ADD \
//            asm  ("add %1, %%ebx;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (value), "b" (reg[base].I));
//
//=============================================================================
#define OP_ADDS \
     asm( "adds %0, %6, %5;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #31, #1;" \
     "ubfx %2, r3, #30, #1;" \
     "ubfx %3, r3, #29, #1;" \
     "ubfx %4, r3, #28, #1;" \
       : "=r" (reg[dest].I), \
        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
       : "r" (value), "r" (reg[base].I) \
       : "r3" );
//#define OP_ADDS \
//   {\
//     u32 lhs = reg[base].I;\
//     u32 rhs = value;\
//     u32 res = lhs + rhs;\
//     reg[dest].I = res;\
//     Z_FLAG = (res == 0) ? true : false;\
//     N_FLAG = NEG(res) ? true : false;\
//     ADDCARRY(lhs, rhs, res);\
//     ADDOVERFLOW(lhs, rhs, res);\
//   }
//#define OP_ADDS \
//            asm  ("add %1, %%ebx;"\
//                  "setsb N_FLAG;"\
//                  "setzb Z_FLAG;"\
//                  "setcb C_FLAG;"\
//                  "setob V_FLAG;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (value), "b" (reg[base].I));
//
//=============================================================================
//XXX: Need to 'load' carry flag in to use native adc
#define OP_ADC \
    {\
      reg[dest].I = reg[base].I + value + (u32)C_FLAG;\
    }
//#define OP_ADC \
//            asm  ("bt $0, C_FLAG;"\
//                  "adc %1, %%ebx;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (value), "b" (reg[base].I));
//
//=============================================================================
//XXX: Need to 'load' carry flag in to use native adcs
//#define OP_ADCS \
//     asm( "adcs %0, %6, %5;" \
//     "mrs r3, cpsr;" \
//     "ubfx %1, r3, #31, #1;" \
//     "ubfx %2, r3, #30, #1;" \
//     "ubfx %3, r3, #29, #1;" \
//     "ubfx %4, r3, #28, #1;" \
//       : "=r" (reg[dest].I), \
//        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
//       : "r" (value), "r" (reg[base].I) \
//       : "r3" );
#define OP_ADCS \
   {\
     u32 lhs = reg[base].I;\
     u32 rhs = value;\
     u32 res = lhs + rhs + (u32)C_FLAG;\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     ADDCARRY(lhs, rhs, res);\
     ADDOVERFLOW(lhs, rhs, res);\
   }
//#define OP_ADCS \
//            asm  ("bt $0, C_FLAG;"\
//                  "adc %1, %%ebx;"\
//                  "setsb N_FLAG;"\
//                  "setzb Z_FLAG;"\
//                  "setcb C_FLAG;"\
//                  "setob V_FLAG;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (value), "b" (reg[base].I));
//
//=============================================================================
#define OP_SBC \
    {\
      reg[dest].I = reg[base].I - value - !((u32)C_FLAG);\
    }
//#define OP_SBC \
//            asm  ("bt $0, C_FLAG;"\
//                  "cmc;"\
//                  "sbb %1, %%ebx;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (value), "b" (reg[base].I));
//
//=============================================================================
//XXX: Need to 'load' C_FLAG in first before we can use native SBCS
//#define OP_SBCS \
//     asm( "sbcs %0, %6, %5;" \
//     "mrs r3, cpsr;" \
//     "ubfx %1, r3, #31, #1;" \
//     "ubfx %2, r3, #30, #1;" \
//     "ubfx %3, r3, #29, #1;" \
//     "ubfx %4, r3, #28, #1;" \
//       : "=r" (reg[dest].I), \
//        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
//       : "r" (value), "r" (reg[base].I) \
//       : "r3" );
#define OP_SBCS \
   {\
     u32 lhs = reg[base].I;\
     u32 rhs = value;\
     u32 res = lhs - rhs - !((u32)C_FLAG);\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(lhs, rhs, res);\
     SUBOVERFLOW(lhs, rhs, res);\
   }
//#define OP_SBCS \
//            asm  ("bt $0, C_FLAG;"\
//                  "cmc;"\
//                  "sbb %1, %%ebx;"\
//                  "setsb N_FLAG;"\
//                  "setzb Z_FLAG;"\
//                  "setncb C_FLAG;"\
//                  "setob V_FLAG;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (value), "b" (reg[base].I));
//=============================================================================
#define OP_RSC \
    {\
      reg[dest].I = value - reg[base].I - !((u32)C_FLAG);\
    }
//#define OP_RSC \
//            asm  ("bt $0, C_FLAG;"\
//                  "cmc;"\
//                  "sbb %1, %%ebx;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (reg[base].I), "b" (value));
//
//=============================================================================
//XXX: Load cflag in before native rscs
//#define OP_RSCS \
//     asm( "rscs %0, %6, %5;" \
//     "mrs r3, cpsr;" \
//     "ubfx %1, r3, #31, #1;" \
//     "ubfx %2, r3, #30, #1;" \
//     "ubfx %3, r3, #29, #1;" \
//     "ubfx %4, r3, #28, #1;" \
//       : "=r" (reg[dest].I), \
//        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
//       : "r" (value), "r" (reg[base].I) \
//       : "r3" );
#define OP_RSCS \
   {\
     u32 lhs = reg[base].I;\
     u32 rhs = value;\
     u32 res = rhs - lhs - !((u32)C_FLAG);\
     reg[dest].I = res;\
     Z_FLAG = (res == 0) ? true : false;\
     N_FLAG = NEG(res) ? true : false;\
     SUBCARRY(rhs, lhs, res);\
     SUBOVERFLOW(rhs, lhs, res);\
   }
//#define OP_RSCS \
//            asm  ("bt $0, C_FLAG;"\
//                  "cmc;"\
//                  "sbb %1, %%ebx;"\
//                  "setsb N_FLAG;"\
//                  "setzb Z_FLAG;"\
//                  "setncb C_FLAG;"\
//                  "setob V_FLAG;"\
//                 : "=b" (reg[dest].I)\
//                 : "r" (reg[base].I), "b" (value));
//=============================================================================
#define OP_CMP \
    asm ( "cmp %5, %4;" \
     "mrs r3, cpsr;" \
     "ubfx %0, r3, #31, #1;" \
     "ubfx %1, r3, #30, #1;" \
     "ubfx %2, r3, #29, #1;" \
     "ubfx %3, r3, #28, #1;" \
       : \
        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
       : "r" (value), "r" (reg[base].I) \
       : "r3" );

//#define OP_CMP \
//   {\
//     u32 lhs = reg[base].I;\
//     u32 rhs = value;\
//     u32 res = lhs - rhs;\
//     Z_FLAG = (res == 0) ? true : false;\
//     N_FLAG = NEG(res) ? true : false;\
//     SUBCARRY(lhs, rhs, res);\
//     SUBOVERFLOW(lhs, rhs, res);\
//   }
//#define OP_CMP \
//            asm  ("sub %0, %1;"\
//                  "setsb N_FLAG;"\
//                  "setzb Z_FLAG;"\
//                  "setncb C_FLAG;"\
//                  "setob V_FLAG;"\
//                 :\
//                 : "r" (value), "r" (reg[base].I));
//
//=============================================================================
#define OP_CMN \
    asm ( "cmn %5, %4;" \
     "mrs r3, cpsr;" \
     "ubfx %0, r3, #31, #1;" \
     "ubfx %1, r3, #30, #1;" \
     "ubfx %2, r3, #29, #1;" \
     "ubfx %3, r3, #28, #1;" \
       : \
        "=r" (N_FLAG), "=r" (Z_FLAG), "=r" (C_FLAG), "=r" (V_FLAG) \
       : "r" (value), "r" (reg[base].I) \
       : "r3" );
//#define OP_CMN \
//   {\
//     u32 lhs = reg[base].I;\
//     u32 rhs = value;\
//     u32 res = lhs + rhs;\
//     Z_FLAG = (res == 0) ? true : false;\
//     N_FLAG = NEG(res) ? true : false;\
//     ADDCARRY(lhs, rhs, res);\
//     ADDOVERFLOW(lhs, rhs, res);\
//   }
//#define OP_CMN \
//            asm  ("add %0, %1;"\
//                  "setsb N_FLAG;"\
//                  "setzb Z_FLAG;"\
//                  "setcb C_FLAG;"\
//                  "setob V_FLAG;"\
//                 : \
//                 : "r" (value), "r" (reg[base].I));
//=============================================================================
#define LOGICAL_LSL_REG \
    asm( "lsls %0, %2, %3;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #29, #1;" \
     : "=r" (value), "=r" (C_OUT) \
     : "r" (reg[opcode & 0x0f].I), "r" (shift) \
     : "r3" );
//#define LOGICAL_LSL_REG \
//   {\
//     u32 v = reg[opcode & 0x0f].I;\
//     C_OUT = (v >> (32 - shift)) & 1 ? true : false;\
//     value = v << shift;\
//   }
//#define LOGICAL_LSL_REG \
//       asm("shl %%cl, %%eax;"\
//           "setcb %%cl;"\
//           : "=a" (value), "=c" (C_OUT)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));
//
//=============================================================================
#define LOGICAL_LSR_REG \
    asm( "lsrs %0, %2, %3;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #29, #1;" \
     : "=r" (value), "=r" (C_OUT) \
     : "r" (reg[opcode & 0x0f].I), "r" (shift) \
     : "r3" );
//#define LOGICAL_LSR_REG \
//   {\
//     u32 v = reg[opcode & 0x0f].I;\
//     C_OUT = (v >> (shift - 1)) & 1 ? true : false;\
//     value = v >> shift;\
//   }
//#define LOGICAL_LSR_REG \
//       asm("shr %%cl, %%eax;"\
//           "setcb %%cl;"\
//           : "=a" (value), "=c" (C_OUT)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));
//
//=============================================================================
#define LOGICAL_ASR_REG \
    asm( "asrs %0, %2, %3;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #29, #1;" \
     : "=r" (value), "=r" (C_OUT) \
     : "r" (reg[opcode & 0x0f].I), "r" (shift) \
     : "r3" );
//#define LOGICAL_ASR_REG \
//   {\
//     u32 v = reg[opcode & 0x0f].I;\
//     C_OUT = ((s32)v >> (int)(shift - 1)) & 1 ? true : false;\
//     value = (s32)v >> (int)shift;\
//   }
//#define LOGICAL_ASR_REG \
//       asm("sar %%cl, %%eax;"\
//           "setcb %%cl;"\
//           : "=a" (value), "=c" (C_OUT)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));
//
//=============================================================================
#define LOGICAL_ROR_REG \
    asm( "rors %0, %2, %3;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #29, #1;" \
     : "=r" (value), "=r" (C_OUT) \
     : "r" (reg[opcode & 0x0f].I), "r" (shift) \
     : "r3" );
//#define LOGICAL_ROR_REG \
//   {\
//     u32 v = reg[opcode & 0x0f].I;\
//     C_OUT = (v >> (shift - 1)) & 1 ? true : false;\
//     value = ((v << (32 - shift)) |\
//              (v >> shift));\
//   }
//#define LOGICAL_ROR_REG \
//       asm("ror %%cl, %%eax;"\
//           "setcb %%cl;"\
//           : "=a" (value), "=c" (C_OUT)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));       
//
//=============================================================================
//#define LOGICAL_RRX_REG \
//    asm( "rrx %0, %2;" \
//     "mrs r3, cpsr;" \
//     "ubfx %1, r3, #29, #1;" \
//     : "=r" (value), "=r" (C_OUT) \
//     : "r" (reg[opcode & 0x0f].I) \
//     : "r3" );
#define LOGICAL_RRX_REG \
   {\
     u32 v = reg[opcode & 0x0f].I;\
     shift = (int)C_FLAG;\
     C_OUT = (v  & 1) ? true : false;\
     value = ((v >> 1) |\
              (shift << 31));\
   }
//#define LOGICAL_RRX_REG \
//       asm("bt $0, C_FLAG;"\
//           "rcr $1, %%eax;"\
//           "setcb %%cl;"\
//           : "=a" (value), "=c" (C_OUT)\
//           : "a" (reg[opcode & 0x0f].I));       
//
//=============================================================================
#define LOGICAL_ROR_IMM \
    asm( "rors %0, %2, %3;" \
     "mrs r3, cpsr;" \
     "ubfx %1, r3, #29, #1;" \
     : "=r" (value), "=r" (C_OUT) \
     : "r" (opcode & 0xff), "r" (shift) \
     : "r3" );
//#define LOGICAL_ROR_IMM \
//   {\
//     u32 v = opcode & 0xff;\
//     C_OUT = (v >> (shift - 1)) & 1 ? true : false;\
//     value = ((v << (32 - shift)) |\
//              (v >> shift));\
//   }
//#define LOGICAL_ROR_IMM \
//       asm("ror %%cl, %%eax;"\
//           "setcb %%cl;"\
//           : "=a" (value), "=c" (C_OUT)\
//           : "a" (opcode & 0xff), "c" (shift));
//=============================================================================
#define ARITHMETIC_LSL_REG \
   {\
     u32 v = reg[opcode & 0x0f].I;\
     value = v << shift;\
   }
//#define ARITHMETIC_LSL_REG \
//       asm("\
//             shl %%cl, %%eax;"\
//           : "=a" (value)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));
//
//=============================================================================
#define ARITHMETIC_LSR_REG \
   {\
     u32 v = reg[opcode & 0x0f].I;\
     value = v >> shift;\
   }
//#define ARITHMETIC_LSR_REG \
//       asm("\
//             shr %%cl, %%eax;"\
//           : "=a" (value)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));
//
//=============================================================================
#define ARITHMETIC_ASR_REG \
   {\
     u32 v = reg[opcode & 0x0f].I;\
     value = (s32)v >> (int)shift;\
   }
//#define ARITHMETIC_ASR_REG \
//       asm("\
//             sar %%cl, %%eax;"\
//           : "=a" (value)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));
//
//=============================================================================
#define ARITHMETIC_ROR_REG \
    asm( "ror %0, %1, %2;" \
     : "=r" (value) \
     : "r" (reg[opcode & 0x0f].I), "r" (shift) );
//#define ARITHMETIC_ROR_REG \
//   {\
//     u32 v = reg[opcode & 0x0f].I;\
//     value = ((v << (32 - shift)) |\
//              (v >> shift));\
//   }
//#define ARITHMETIC_ROR_REG \
//       asm("\
//             ror %%cl, %%eax;"\
//           : "=a" (value)\
//           : "a" (reg[opcode & 0x0f].I), "c" (shift));       
//
//=============================================================================
#define ARITHMETIC_RRX_REG \
   {\
     u32 v = reg[opcode & 0x0f].I;\
     shift = (int)C_FLAG;\
     value = ((v >> 1) |\
              (shift << 31));\
   }
//#define ARITHMETIC_RRX_REG \
//       asm("\
//             bt $0, C_FLAG;\
//             rcr $1, %%eax;"\
//           : "=a" (value)\
//           : "a" (reg[opcode & 0x0f].I));       
//
//=============================================================================
#define ARITHMETIC_ROR_IMM \
    asm( "ror %0, %1, %2;" \
     : "=r" (value) \
     : "r" (opcode & 0xff), "r" (shift) );
//#define ARITHMETIC_ROR_IMM \
//   {\
//     u32 v = opcode & 0xff;\
//     value = ((v << (32 - shift)) |\
//              (v >> shift));\
//   }
//#define ARITHMETIC_ROR_IMM \
//       asm("\
//             ror %%cl, %%eax;"\
//           : "=a" (value)\
//           : "a" (opcode & 0xff), "c" (shift));
//=============================================================================
#define ROR_IMM_MSR \
    asm( "ror %0, %1, %2;" \
     : "=r" (value) \
     : "r" (opcode & 0xff), "r" (shift) );
//#define ROR_IMM_MSR \
//   {\
//     u32 v = opcode & 0xff;\
//     value = ((v << (32 - shift)) |\
//              (v >> shift));\
//   }
//#define ROR_IMM_MSR \
//      asm ("ror %%cl, %%eax;"\
//           : "=a" (value)\
//           : "a" (opcode & 0xFF), "c" (shift));
//=============================================================================
#define ROR_VALUE \
    asm( "ror %0, %1, %2;" \
     : "=r" (value) \
     : "r" (value), "r" (shift) );
//#define ROR_VALUE \
//   {\
//     value = ((value << (32 - shift)) |\
//              (value >> shift));\
//   }
//#define ROR_VALUE \
//      asm("ror %%cl, %0"\
//          : "=r" (value)\
//          : "r" (value), "c" (shift));
//=============================================================================
#define RCR_VALUE \
   {\
     shift = (int)C_FLAG;\
     value = ((value >> 1) |\
              (shift << 31));\
   }
//#define RCR_VALUE \
//      asm("bt $0, C_FLAG;"\
//          "rcr $1, %0"\
//          : "=r" (value)\
//          : "r" (value));
//=============================================================================
