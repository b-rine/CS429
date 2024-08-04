/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Memory.c - Memory stage of instruction processing pipeline.
 **************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "err_handler.h"
#include "instr.h"
#include "instr_pipeline.h"
#include "machine.h"
#include "hw_elts.h"

extern machine_t guest;
extern mem_status_t dmem_status;

extern comb_logic_t copy_w_ctl_sigs(w_ctl_sigs_t *, w_ctl_sigs_t *);

/*
 * Memory stage logic.
 * STUDENT TO-DO:
 * Implement the memory stage.
 * 
 * Use in as the input pipeline register,
 * and update the out pipeline register as output.
 * 
 * You will need the following helper functions:
 * copy_w_ctl_signals and dmem.
 */

comb_logic_t memory_instr(m_instr_impl_t *in, w_instr_impl_t *out) {

    // If excepting instruction, disallow succeeding instructions from updating state and insert bubble into M
    if (in->status != STAT_AOK && in->status != STAT_BUB) {
        X_in->X_sigs.set_CC = false;
        X_out->X_sigs.set_CC = false;
        M_in->status = STAT_BUB;
    }

    // Pass existing fields
    out->op = in->op;
    out->print_op = in->print_op;
    copy_w_ctl_sigs(&(out->W_sigs), &(in->W_sigs));
    out->val_b = in->val_b;
    out->dst = in->dst;
    out->val_ex = in->val_ex;
    out->status = in->status;

    // Memory read and/or write
    bool dmem_err;
    if (in->M_sigs.dmem_read || in->M_sigs.dmem_write) {
        dmem(in->val_ex, in->val_b, in->M_sigs.dmem_read, in->M_sigs.dmem_write, &(out->val_mem), &dmem_err);

        if (dmem_err) {
            if (out->op == OP_LDUR || out->op == OP_STUR) {
                out->status = STAT_ADR;
                out->W_sigs.w_enable = false;
            }
        }
    }
}
