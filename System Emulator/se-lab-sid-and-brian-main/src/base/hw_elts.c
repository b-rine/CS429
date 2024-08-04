/**************************************************************************
 * C S 429 system emulator
 * 
 * Student TODO: AE
 * 
 * hw_elts.c - Module for emulating hardware elements.
 * 
 * Copyright (c) 2022, 2023. 
 * Authors: S. Chatterjee, Z. Leeper. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include <assert.h>
#include "hw_elts.h"
#include "mem.h"
#include "machine.h"
#include "forward.h"
#include "err_handler.h"

extern machine_t guest;

comb_logic_t 
imem(uint64_t imem_addr,
     uint32_t *imem_rval, bool *imem_err) {
    // imem_addr must be in "instruction memory" and a multiple of 4
    *imem_err = (!addr_in_imem(imem_addr) || (imem_addr & 0x3U));
    *imem_rval = (uint32_t) mem_read_I(imem_addr);
}

/*
 * Regfile logic.
 * STUDENT TO-DO:
 * Read from source registers and write to destination registers when enabled.
 */
comb_logic_t
regfile(uint8_t src1, uint8_t src2, uint8_t dst, uint64_t val_w,
        // bool src1_31isSP, bool src2_31isSP, bool dst_31isSP, 
        bool w_enable,
        uint64_t *val_a, uint64_t *val_b) {

            // Always read
            // Read from src1
            if (src1 >= 32) {
                *val_a = 0;
            } else if (src1 == 31) {
                *val_a = guest.proc->SP;
            } else {
                *val_a = guest.proc->GPR[src1];
            }

            // Read from src2
            if (src2 >= 32) {
                *val_b = 0;
            } else if (src2 == 31) {
                *val_b = guest.proc->SP;
            } else {
                *val_b = guest.proc->GPR[src2];
            }

            // Write if w_enable
            if (w_enable) {

                // Write to SP if SP
                if (dst == 31) {
                    guest.proc->SP = val_w;

                // Otherwise write to correct GPR
                } else if (dst < 31) {
                    guest.proc->GPR[dst] = val_w;
                }
            }
}

/*
 * cond_holds logic.
 * STUDENT TO-DO:
 * Determine if the condition is true or false based on the condition code values
 */
static bool 
cond_holds(cond_t cond, uint8_t ccval) {
    switch(cond) {

        // Equal
        case C_EQ:
            if (GET_ZF(ccval) == 1) {
                return true;
            } else {
                return false;
            }
            break;

        // Not Equal
        case C_NE:
            if (GET_ZF(ccval) == 0) {
                return true;
            } else {
                return false;
            }
            break;

        // Carry Set
        case C_CS:
            if (GET_CF(ccval) == 1) {
                return true;
            } else {
                return false;
            }
            break;

        // Carry Clear
        case C_CC:
            if (GET_CF(ccval) == 0) {
                return true;
            } else {
                return false;
            }
            break;

        // Minus/Negative
        case C_MI:
            if (GET_NF(ccval) == 1) {
                return true;
            } else {
                return false;
            }
            break;

        // Plus/Non-negative
        case C_PL:
            if (GET_NF(ccval) == 0) {
                return true;
            } else {
                return false;
            }
            break;

        // Overflow
        case C_VS:
            if (GET_VF(ccval) == 1) {
                return true;
            } else {
                return false;
            }
            break;

        // No Overflow
        case C_VC:
            if (GET_VF(ccval) == 0) {
                return true;
            } else {
                return false;
            }
            break;

        // Higher
        case C_HI:
            if (GET_ZF(ccval) == 0 && GET_CF(ccval) == 1) {
                return true;
            } else {
                return false;
            }
            break;

        // Lower or Same
        case C_LS:
            if (GET_CF(ccval) == 0 || GET_ZF(ccval) == 1) {
                return true;
            } else {
                return false;
            }
            break;

        // Greater than or Equal
        case C_GE:
            if (GET_NF(ccval) == GET_VF(ccval)) {
                return true;
            } else {
                return false;
            }
            break;

        // Less Than
        case C_LT:
            if (GET_NF(ccval) != GET_VF(ccval)) {
                return true;
            } else {
                return false;
            }
            break;

        // Greater Than
        case C_GT:
            if (GET_ZF(ccval) == 0 && GET_NF(ccval) == GET_VF(ccval)) {
                return true;
            } else {
                return false;
            }
            break;

        // Less than or Equal
        case C_LE:
            if (GET_ZF(ccval) == 1 || GET_NF(ccval) != GET_VF(ccval)) {
                return true;
            } else {
                return false;
            }
            break;

        // Always
        case C_AL:
            return true;
            break;

        // Never
        case C_NV:
            return true;
            break;

        // Default
        default:
            break;
    }
}

/*
 * alu logic.
 * STUDENT TO-DO:
 * Compute the result of a bitwise or mathematial operation (all operations in alu_op_t).
 * Additionally, apply hw or compute condition code values as necessary.
 * Finally, compute condition values with cond_holds.
 */
comb_logic_t 
alu(uint64_t alu_vala, uint64_t alu_valb, uint8_t alu_valhw, alu_op_t ALUop, bool set_CC, cond_t cond, 
    uint64_t *val_e, bool *cond_val, uint8_t *nzcv) {
    uint64_t res = 0xFEEDFACEDEADBEEF;  // To make it easier to detect errors.

    // Switch case on operation
    switch(ALUop) {

        // Plus
        case PLUS_OP:
            *val_e = alu_vala + (alu_valb << alu_valhw);
            break;

        // Minus
        case MINUS_OP:
            *val_e = alu_vala - (alu_valb << alu_valhw);
            break;

        // Invert
        case INV_OP:
            *val_e = alu_vala | (~alu_valb);
            break;

        // Or
        case OR_OP:
            *val_e = alu_vala | alu_valb;
            break;

        // Exor
        case EOR_OP:
            *val_e = alu_vala ^ alu_valb;
            break;

        // And
        case AND_OP:
            *val_e = alu_vala & alu_valb;
            break;

        // Move
        case MOV_OP:
            *val_e = alu_vala | (alu_valb << alu_valhw);
            break;

        // LSL
        case LSL_OP:
            *val_e = alu_vala << (alu_valb & 0x3FUL);
            break;

        // LSR
        case LSR_OP:
            *val_e = (uint64_t) ((unsigned long) alu_vala >> (alu_valb & 0x3FUL));
            break;

        // ASR
        case ASR_OP:
            *val_e = (uint64_t) ((long) alu_vala >> (alu_valb & 0x3FUL));
            break;

        // Pass A
        case PASS_A_OP:
            *val_e = alu_vala;
            break;

        case CSEL_OP:
            if (cond_holds(cond, *nzcv)) {
                *val_e = alu_vala;
            } else {
                *val_e = alu_valb;
            }
            break;

        case CSINV_OP:
            if (cond_holds(cond, *nzcv)) {
                *val_e = alu_vala;
            } else {
                *val_e = ~alu_valb;
            }
            break;

        case CSINC_OP:
            if (cond_holds(cond, *nzcv)) {
                *val_e = alu_vala;
            } else {
                *val_e = alu_valb + 1;
            }
            break;

        case CSNEG_OP:
            if (cond_holds(cond, *nzcv)) {
                *val_e = alu_vala;
            } else {
                *val_e = -alu_valb;
            }
            break;

        default:
            *val_e = res;
            break;
        
        // case CBZ_OP:
        //     if (alu_vala == 0) {
        //         *cond_val = true;
        //     } else {
        //         *cond_val = false;
        //     }
        //     break;

        // case CBNZ_OP:
        //     if (alu_vala != 0) {
        //         *cond_val = false;
        //     } else {
        //         *cond_val = true;
        //     }
        //     break;
    }

    // Set condition flags
    if (set_CC) {
        bool n = 0;
        bool z = 0;
        bool c = 0;
        bool v = 0;

        // Check if negative
        if (*val_e >> 63 == 1) {
            n = 1;
        }

        // Check if zero
        if (*val_e == 0) {
            z = 1;
        }

        // Check if carry
        if (ALUop == PLUS_OP && *val_e < alu_vala) {
            c = 1;
        } else if ((ALUop == MINUS_OP && alu_vala >= alu_valb)) {
            c = 1;
        }

        // Check if overflow
        if (ALUop == PLUS_OP && alu_vala >> 63 == alu_valb >> 63 && *val_e >> 63 != alu_vala >> 63) {
            v = 1;
        } else if (ALUop == MINUS_OP && alu_vala < 0 && alu_valb < 0 && *val_e > 0) {
            v = 1;
        }

        // Set condition codes
        *nzcv = PACK_CC(n, z, c, v);
    }

    // Check condition
    *cond_val = cond_holds(cond, *nzcv);

    if (ALUop == CBZ_OP) {
        if (alu_vala == 0) {
            *cond_val = true;
        } else {
            *cond_val = false;
        }
    } else if (ALUop == CBNZ_OP) {
        if (alu_vala == 0) {
            *cond_val = false;
        } else {
            *cond_val = true;
        }
    }
}

comb_logic_t 
dmem(uint64_t dmem_addr, uint64_t dmem_wval, bool dmem_read, bool dmem_write, 
     uint64_t *dmem_rval, bool *dmem_err) {
    // dmem_addr must be in "data memory" and a multiple of 8
    *dmem_err = (!addr_in_dmem(dmem_addr) || (dmem_addr & 0x7U));
    if (is_special_addr(dmem_addr)) *dmem_err = false;
    if (dmem_read) *dmem_rval = (uint64_t) mem_read_L(dmem_addr);
    if (dmem_write) mem_write_L(dmem_addr, dmem_wval);
}