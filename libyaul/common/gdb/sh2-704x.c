/*
 * Copyright (c) 2001-2012
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cartridge.h>

#include <cpu/intc.h>
#include <cpu/registers.h>

#include <smpc/smc.h>

#include <assert.h>
#include <stddef.h>
#include <sys/queue.h>
#include <stdlib.h>

#include "gdb.h"
#include "ihr.h"
#include "sh2-704x.h"

#define R0      0
#define R1      1
#define R2      2
#define R3      3
#define R4      4
#define R5      5
#define R6      6
#define R7      7
#define R8      8
#define R9      9
#define R10     10
#define R11     11
#define R12     12
#define R13     13
#define R14     14
#define SP      15
#define PC      16
#define PR      17
#define GBR     18
#define VBR     19
#define MACH    20
#define MACL    21
#define SR      22

#define INSTRN_TRAPA(i)         (0xC300 | ((i) & 0xFF))

#define OPCODE_BT(op)           (((op) & 0xFF00) == 0x8900)
#define OPCODE_BTS(op)          (((op) & 0xFF00) == 0x8D00)
#define OPCODE_BF(op)           (((op) & 0xFF00) == 0x8B00)
#define OPCODE_BFS(op)          (((op) & 0xFF00) == 0x8F00)
#define OPCODE_8_DISP(op) ((((op) & 0x0080) == 0x0000)                        \
                ? ((op) & 0x00FF)                                             \
                /* Sign-extend */                                             \
                : ((op) & 0x00FF) | 0xFFFFFF00)
#define OPCODE_BRA(op)          (((op) & 0xF000) == 0xA000)
#define OPCODE_BSR(op)          (((op) & 0xF000) == 0xB000)
#define OPCODE_12_DISP(op) ((((op) & 0x0800) == 0x0000)                       \
                ? ((op) & 0x0FFF)                                             \
                /* Sign-extend */                                             \
                : ((op) & 0x0FFF) | 0xFFFFF000)
#define OPCODE_BRAF(op)         (((op) & 0xF0FF) == 0x0023)
#define OPCODE_BRAF_M(op)       (((op) & 0x0F00) >> 8)
#define OPCODE_BSRF(op)         (((op) & 0xF0FF) == 0x0003)
#define OPCODE_BSRF_M(op)       (((op) >> 8) & 0x0F)
#define OPCODE_JMP(op)          (((op) & 0xF0FF) == 0x402B)
#define OPCODE_JMP_M(op)        (((op) >> 8) & 0x0F)
#define OPCODE_JSR(op)          (((op) & 0xF0FF) == 0x400B)
#define OPCODE_JSR_M(op)        (((op) >> 8) & 0x0F)
#define OPCODE_RTS(op)          ((op) == 0x000B)
#define OPCODE_RTE(op)          ((op) == 0x002B)
#define OPCODE_TRAPA(op)        (((op) & 0xFF00) == 0xC300)
#define OPCODE_TRAPA_IMM(op)    ((op) & 0x00FF)

static uint32_t calculate_pc(struct cpu_registers *);

typedef TAILQ_HEAD(bp_list, bp) bp_list_t;

typedef struct bp bp_t;

struct bp {
        void *addr;
        uint16_t instruction;

        TAILQ_ENTRY(bp) entries;
};

static bp_list_t *bp_list_alloc(void);
static bool bp_list_empty(bp_list_t *);
static void bp_list_free(bp_list_t **) __attribute__ ((unused));
static bp_t *bp_list_breakpoint_alloc(void);
static void bp_list_breakpoint_free(bp_list_t *, bp_t *);
static int bp_list_breakpoint_add(bp_list_t *, void *);
static bp_t *bp_list_breakpoint_find(bp_list_t *, void *);

/* Overwritten instruction meant to allow stepping through */
static bool stepping = false;
static bp_t bp_step = {
        .addr = NULL,
        .instruction = 0x0000
};

static bp_list_t *bp_list = NULL;

void
gdb_init(void)
{
        void (**vbr)(void);

        usb_cartridge_init();

        /* Disable interrupts */
        cpu_intc_disable();

        bp_list = bp_list_alloc();

        /* Clear */
        bp_step.addr = 0x00000000;
        bp_step.instruction = 0x0000;
        stepping = false;

        vbr = cpu_intc_vector_base_get();
        vbr[0x04] = ihr_illegal_instruction;
        vbr[0x06] = ihr_illegal_slot;
        vbr[0x09] = ihr_cpu_address_error;
        vbr[0x0A] = ihr_dma_address_error;
        /* Register UBC IHR */
        vbr[0x0C] = ihr_ubc;
        /* Register break IHR */
        vbr[0x20] = ihr_break;

        /* Initialize UBC */
        /*
         * Channel A is to be a queue of breakpoints/watchpoints
         * Channel B is used to break into GDB
         */

        /* Enable interrupts */
        cpu_intc_enable();

        /* Cause a breakpoint to sync with GDB */
        gdb_sync();
}

void
gdb_putc(int c)
{

        /* Non-blocking */
        usb_cartridge_send_byte(c);
}

int
gdb_getc(void)
{
        uint8_t c;

        /* Blocks */
        c = usb_cartridge_read_byte();

        return c;
}

void
gdb_step(struct cpu_registers *reg_file, uint32_t addr)
{
        uint16_t *p;

        if (addr != 0x00000000)
                p = (uint16_t *)addr;
        else
                /* Determine where we'll be going */
                p = (uint16_t *)calculate_pc(reg_file);

        bp_step.addr = (void *)p;
        bp_step.instruction = *p;
        *p = INSTRN_TRAPA(0x20);

        /* We're stepping, be aware of breakpoints and watchpoints */
        stepping = true;
}

int
gdb_remove_break(uint32_t type, uint32_t addr, uint32_t kind)
{
        bp_t *bp;

        kind = kind;

        if (addr == 0x00000000)
                return -1;

        switch (type) {
        case 0x00:
                if ((bp = bp_list_breakpoint_find(bp_list, (void *)addr)) == NULL)
                        return -1;
                bp_list_breakpoint_free(bp_list, bp);
                return 0;
        default:
                return -1;
        }
}

int
gdb_break(uint32_t type, uint32_t addr, uint32_t kind)
{

        kind = kind;

        if (addr == 0x00000000)
                return -1;

        switch (type) {
        case 0x00:
                if ((bp_list_breakpoint_add(bp_list, (void *)addr)) < 0)
                        return -1;

                return 0;
        default:
                return -1;
        }
}

void
gdb_kill(void)
{

        smpc_smc_resenab_call();
        cpu_intc_enable();
        smpc_smc_sysres_call();
}

void
gdb_monitor_entry(struct cpu_registers *reg_file)
{
        uint16_t *p;
        uint32_t *pc;

        bp_t *bp;

        if (!stepping && bp_list_empty(bp_list))
                return;

        /* Clobber what TRAPA stored on the stack and jump back by one
         * instruction because of the TRAPA instruction */
        pc = (uint32_t *)&reg_file->pc;
        *pc = reg_file->pc - 0x00000002;

        /* Determine if we're stepping into/over a breakpoint */
        p = (uint16_t *)reg_file->pc;
        if ((bp = bp_list_breakpoint_find(bp_list, (void *)p)) != NULL) {
                p = (uint16_t *)bp->addr;
                *p = bp->instruction;
        }

        /* Upon GDB monitor entry */
        if (stepping) {
                /* Overwrite TRAPA instruction */
                p = (uint16_t *)bp_step.addr;
                *p = bp_step.instruction;

                /* Clear */
                bp_step.addr = 0x00000000;
                bp_step.instruction = 0x0000;

                stepping = false;
        }
}

bool
gdb_register_file_read(struct cpu_registers *reg_file, uint32_t n, uint32_t *r)
{

        switch (n) {
        case R0:
        case R1:
        case R2:
        case R3:
        case R4:
        case R5:
        case R6:
        case R7:
        case R8:
        case R9:
        case R10:
        case R11:
        case R12:
        case R13:
        case R14:
                *r = reg_file->r[n];
                return true;
        case SP:
                *r = reg_file->sp;
                return true;
        case MACL:
                *r = reg_file->macl;
                return true;
        case MACH:
                *r = reg_file->mach;
                return true;
        case VBR:
                *r = reg_file->vbr;
                return true;
        case GBR:
                *r = reg_file->gbr;
                return true;
        case PR:
                *r = reg_file->pr;
                return true;
        case PC:
                *r = reg_file->pc;
                return true;
        case SR:
                *r = reg_file->sr;
                return true;
        default:
                return false;
        }
}

bool
gdb_register_file_write(struct cpu_registers *reg_file, uint32_t n, uint32_t r)
{

        uint32_t *p;

        switch (n) {
        case R0:
        case R1:
        case R2:
        case R3:
        case R4:
        case R5:
        case R6:
        case R7:
        case R8:
        case R9:
        case R10:
        case R11:
        case R12:
        case R13:
        case R14:
                p = &reg_file->r[n];
                break;
        case SP:
                p = &reg_file->sp;
                break;
        case MACL:
                p = &reg_file->macl;
                break;
        case MACH:
                p = &reg_file->mach;
                break;
        case VBR:
                p = &reg_file->vbr;
                break;
        case GBR:
                p = &reg_file->gbr;
                break;
        case PR:
                p = &reg_file->pr;
                break;
        case PC:
                p = &reg_file->pc;
                break;
        case SR:
                p = &reg_file->sr;
                break;
        default:
                return false;
        }

        *p = r;
        return true;
}

/* Analyze the next instruction, to see where the program will go to
 * when it runs
 *
 * Returns the destination address */
static uint32_t
calculate_pc(struct cpu_registers *reg_file)
{
        uint16_t opcode;
        uint32_t pc;

        int32_t disp;
        uint32_t m;

        /* Opcode at PC */
        opcode = *(uint16_t *)reg_file->pc;

        pc = reg_file->pc + 0x00000002;
        disp = 0x00000000;

        if (OPCODE_BT(opcode) || OPCODE_BTS(opcode)) {
                if (reg_file->sr & SR_T_BIT_MASK) {
                        disp = OPCODE_8_DISP(opcode);
                        pc = reg_file->pc + (disp << 1) + 0x00000004;
                }
        } else if (OPCODE_BF(opcode) || OPCODE_BFS(opcode)) {
                if ((reg_file->sr & SR_T_BIT_MASK) == 0x00000000) {
                        disp = OPCODE_8_DISP(opcode);
                        pc = reg_file->pc + (disp << 1) + 0x00000004;
                }
        } else if ((OPCODE_BRA(opcode)) || (OPCODE_BSR(opcode))) {
                disp = OPCODE_12_DISP(opcode);
                pc = reg_file->pc + (disp << 1) + 0x00000004;
        } else if (OPCODE_BRAF(opcode)) {
                m = OPCODE_BRAF_M(opcode);
                pc = reg_file->pc + reg_file->r[m] + 0x00000004;
        } else if (OPCODE_BSRF(opcode)) {
                m = OPCODE_BSRF_M(opcode);
                pc = reg_file->pc + reg_file->r[m] + 0x00000004;
        } else if (OPCODE_JMP(opcode)) {
                m = OPCODE_JMP_M(opcode);
                pc = reg_file->r[m];
        } else if (OPCODE_JSR(opcode)) {
                m = OPCODE_JSR_M(opcode);
                pc = reg_file->r[m];
        } else if (OPCODE_RTS(opcode)) {
                pc = reg_file->pr;
        } else if (OPCODE_RTE(opcode))
                pc = *(uint32_t *)reg_file->sp;
        else if (OPCODE_TRAPA(opcode))
                pc = *(uint32_t *)(reg_file->vbr + (OPCODE_TRAPA_IMM(opcode) << 1));

        return pc;
}

static bp_list_t *
bp_list_alloc(void)
{
        bp_list_t *bpl;

        if ((bpl = (bp_list_t *)malloc(sizeof(bp_list_t))) == NULL)
                return NULL;

        /* Initialize queue */
        TAILQ_INIT(bpl);

        return bpl;
}

static bool
bp_list_empty(bp_list_t *bpl)
{

        if (TAILQ_EMPTY(bpl))
                return true;

        return false;
}

static void
bp_list_free(bp_list_t **bplp)
{

        assert(*bplp != NULL);

        free(*bplp);
        *bplp = NULL;
}

static bp_t *
bp_list_breakpoint_alloc(void)
{
        bp_t *bp;

        if ((bp = (bp_t *)malloc(sizeof(bp_t))) == NULL)
                return NULL;

        bp->addr = NULL;
        bp->instruction = 0x0000;

        return bp;
}

static void
bp_list_breakpoint_free(bp_list_t *bpl, bp_t *bp)
{
        uint16_t *p;

        assert(bp != NULL);

        if (TAILQ_EMPTY(bpl))
                return;

        p = (uint16_t *)bp->addr;
        *p = bp->instruction;

        TAILQ_REMOVE(bpl, bp, entries);
        free(bp);
}

static int
bp_list_breakpoint_add(bp_list_t *bpl, void *addr)
{
        bp_t *bp;

        assert(bpl != NULL);

        /* Check if we have a breakpoint of the same address already present */
        if ((bp_list_breakpoint_find(bp_list, (void *)addr)) != NULL)
                return 0;

        if ((bp = bp_list_breakpoint_alloc()) == NULL)
                return -1;

        bp->addr = addr;
        bp->instruction = *(uint16_t *)addr;
        *(uint16_t *)addr = INSTRN_TRAPA(0x20);

        /* Insert */
        TAILQ_INSERT_TAIL(bpl, bp, entries);

        return 0;
}

static bp_t *
bp_list_breakpoint_find(bp_list_t *bpl, void *addr)
{
        bp_t *bp_np;

        assert(bpl != NULL);

        if (TAILQ_EMPTY(bpl))
                return NULL;

        TAILQ_FOREACH (bp_np, bpl, entries) {
                if (bp_np->addr == addr)
                        return bp_np;
        }

        return NULL;
}
