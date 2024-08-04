/**************************************************************************
 * C S 429 system emulator
 * 
 * forward.c - The value forwarding logic in the pipelined processor.
 **************************************************************************/ 

#include <stdbool.h>
#include "forward.h"

/* STUDENT TO-DO:
 * Implement forwarding register values from
 * execute, memory, and writeback back to decode.
 */
comb_logic_t forward_reg(uint8_t D_src1, uint8_t D_src2, uint8_t X_dst, uint8_t M_dst, uint8_t W_dst,
                 uint64_t X_val_ex, uint64_t M_val_ex, uint64_t M_val_mem, uint64_t W_val_ex,
                 uint64_t W_val_mem, bool M_wval_sel, bool W_wval_sel, bool X_w_enable,
                 bool M_w_enable, bool W_w_enable,
                 uint64_t *val_a, uint64_t *val_b) {

    // if instruction in Writeback is writing to register
    if (W_w_enable && (D_src1 == W_dst || D_src2 == W_dst)) {

        // if src1 is match and w_wval_select
        if (D_src1 == W_dst && W_wval_sel) {
            *val_a = W_val_mem;
        }

        // if src2 is match and w_wval_select
        if (D_src2 == W_dst && W_wval_sel) {
            *val_b = W_val_mem;
        }

        // if src1 is match and !w_wval_select
        if (D_src1 == W_dst && !W_wval_sel) {
            *val_a = W_val_ex;
        }

        // if src2 is match and !w_wval_select
        if (D_src2 == W_dst && !W_wval_sel) {
            *val_b = W_val_ex;
        }
    }

    // if instruction in Memory is writing to register
    if (M_w_enable && (D_src1 == M_dst || D_src2 == M_dst)) {

        // if src1 is match and m_wval_select
        if (D_src1 == M_dst && M_wval_sel) {
            *val_a = M_val_mem;
        }

        // if src2 is match and m_wval_select
        if (D_src2 == M_dst && M_wval_sel) {
            *val_b = M_val_mem;
        }

        // if src1 is match and !m_wval_select
        if (D_src1 == M_dst && !M_wval_sel) {
            *val_a = M_val_ex;
        }

        // if src2 is match and !m_wval_select
        if (D_src2 == M_dst && !M_wval_sel) {
            *val_b = M_val_ex;
        }
    }

    // if instruction in Execute is writing to register 
    if (X_w_enable && (D_src1 == X_dst || D_src2 == X_dst)) {

        // if src1 is match
        if (D_src1 == X_dst) {
            *val_a = X_val_ex;
        }

        // if src2 is match
        if (D_src2 == X_dst) {
            *val_b = X_val_ex;
        }
    }
}
