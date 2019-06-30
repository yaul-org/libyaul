/* -*- mode: c -*- */

/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * William A. Gatliff <bgat@billgatliff.com>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>
#include <sys/queue.h>

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
#include <usb-cart.h>
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#include <arp.h>
#else
#error "Invalid value for `HAVE_DEV_CARTRIDGE'"
#endif /* HAVE_DEV_CARTRIDGE */

#include <bios.h>

#include <cpu/intc.h>
#include <cpu/registers.h>
#include <cpu/instructions.h>

#include <smpc/smc.h>

extern void internal_gdb_ihr_break(void);
extern void internal_gdb_exception_cpu_address_error(void);
extern void internal_gdb_exception_dma_address_error(void);
extern void internal_gdb_exception_illegal_instruction(void);
extern void internal_gdb_exception_illegal_slot(void);
extern void internal_gdb_ihr_ubc(void);

#define REG_R0          0
#define REG_R1          1
#define REG_R2          2
#define REG_R3          3
#define REG_R4          4
#define REG_R5          5
#define REG_R6          6
#define REG_R7          7
#define REG_R8          8
#define REG_R9          9
#define REG_R10         10
#define REG_R11         11
#define REG_R12         12
#define REG_R13         13
#define REG_R14         14
#define REG_SP          15
#define REG_PC          16
#define REG_PR          17
#define REG_GBR         18
#define REG_VBR         19
#define REG_MACH        20
#define REG_MACL        21
#define REG_SR          22

#define INSTRN_TRAPA(i)         (0xC300 | ((i) & 0xFF))

#define OPCODE_BT(op)           (((op) & 0xFF00) == 0x8900)
#define OPCODE_BTS(op)          (((op) & 0xFF00) == 0x8D00)
#define OPCODE_BF(op)           (((op) & 0xFF00) == 0x8B00)
#define OPCODE_BFS(op)          (((op) & 0xFF00) == 0x8F00)
#define OPCODE_8_DISP(op)       ((((op) & 0x0080) == 0x0000)                   \
                ? ((op) & 0x00FF)                                              \
                /* Sign-extend */                                              \
                : ((op) & 0x00FF) | 0xFFFFFF00)
#define OPCODE_BRA(op)          (((op) & 0xF000) == 0xA000)
#define OPCODE_BSR(op)          (((op) & 0xF000) == 0xB000)
#define OPCODE_12_DISP(op)      ((((op) & 0x0800) == 0x0000)                   \
                ? ((op) & 0x0FFF)                                              \
                /* Sign-extend */                                              \
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

static uint32_t _calculate_pc(struct cpu_registers *);

typedef TAILQ_HEAD(bp_list, bp) bp_list_t;

typedef struct bp bp_t;

struct bp {
        void *addr;
        uint16_t instruction;

        TAILQ_ENTRY(bp) entries;
};

static bp_list_t *_bp_list_alloc(void);
static bool _bp_list_empty(bp_list_t *);
static void _bp_list_free(bp_list_t **) __unused;
static bp_t *_bp_list_breakpoint_alloc(void);
static void _bp_list_breakpoint_free(bp_list_t *, bp_t *);
static int _bp_list_breakpoint_add(bp_list_t *, void *);
static bp_t *_bp_list_breakpoint_find(bp_list_t *, void *);

static void _device_init(void);
static uint8_t _device_read(void);
static void _device_write(uint8_t);

/* Overwritten instruction meant to allow stepping through */
static bool _stepping = false;
static bp_t _bp_step = {
        .addr = NULL,
        .instruction = 0x0000
};

static bp_list_t *_bp_list = NULL;

static void
_gdb_sh2_704x_init(void)
{
        /* Disable interrupts */
        uint32_t mask;
        mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        _device_init();

        _bp_list = _bp_list_alloc();

        /* Clear */
        _bp_step.addr = 0x00000000;
        _bp_step.instruction = 0x0000;
        _stepping = false;

        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_ILLEGAL_INSTRUCTION,
            internal_gdb_exception_illegal_instruction);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_CPU_ADDRESS_ERROR,
            internal_gdb_exception_illegal_slot);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_CPU_ADDRESS_ERROR,
            internal_gdb_exception_cpu_address_error);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMA_ADDRESS_ERROR,
            internal_gdb_exception_dma_address_error);

        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_UBC, internal_gdb_ihr_ubc);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_BREAK, internal_gdb_ihr_break);

        /* Initialize UBC */
        /*
         * Channel A is to be a queue of breakpoints/watchpoints
         * Channel B is used to break into GDB
         */

        /* Enable interrupts */
        cpu_intc_mask_set(mask);

        /* Cause a breakpoint to sync with GDB */
        cpu_instr_trapa(0x20);
}

static void
_gdb_putc(int c)
{
        _device_write(c);
}

static int
_gdb_getc(void)
{
        /* Blocks */
        return _device_read();
}

static void
_gdb_step(struct cpu_registers *reg_file, uint32_t address)
{
        uint16_t *p;

        if (address != 0x00000000) {
                p = (uint16_t *)address;
        } else {
                /* Determine where we'll be going */
                p = (uint16_t *)_calculate_pc(reg_file);
        }

        _bp_step.addr = (void *)p;
        _bp_step.instruction = *p;
        *p = INSTRN_TRAPA(0x20);

        /* We're stepping, be aware of breakpoints and watchpoints */
        _stepping = true;
}

static int
_gdb_remove_break(uint32_t type, uint32_t addr, uint32_t kind __unused)
{
        if (addr == 0x00000000) {
                return -1;
        }

        bp_t *bp;

        switch (type) {
        case 0x00:
                if ((bp = _bp_list_breakpoint_find(_bp_list, (void *)addr)) == NULL) {
                        return -1;
                }

                _bp_list_breakpoint_free(_bp_list, bp);

                return 0;
        default:
                return -1;
        }
}

static int
_gdb_break(uint32_t type, uint32_t addr, uint32_t kind __unused)
{
        if (addr == 0x00000000) {
                return -1;
        }

        switch (type) {
        case 0x00:
                if ((_bp_list_breakpoint_add(_bp_list, (void *)addr)) < 0) {
                        return -1;
                }

                return 0;
        default:
                return -1;
        }
}

static void __noreturn
_gdb_kill(void)
{
        /* XXX: Let's use abort() here */

        /* XXX: System reset doesn't seem to work here. Could it be because
         *      we're inside an interrupt handler? */

        smpc_smc_resenab_call();
        cpu_intc_mask_set(0);
        smpc_smc_sysres_call();
}

static void
_gdb_monitor_entry(struct cpu_registers *reg_file)
{
        uint16_t *p;
        uint32_t *pc;

        bp_t *bp;

        if (!_stepping && _bp_list_empty(_bp_list)) {
                return;
        }

        /* Clobber what TRAPA stored on the stack and jump back by one
         * instruction because of the TRAPA instruction */
        pc = (uint32_t *)&reg_file->pc;
        *pc = reg_file->pc - 0x00000002;

        /* Determine if we're stepping into/over a breakpoint */
        p = (uint16_t *)reg_file->pc;
        if ((bp = _bp_list_breakpoint_find(_bp_list, (void *)p)) != NULL) {
                p = (uint16_t *)bp->addr;
                *p = bp->instruction;
        }

        /* Upon GDB monitor entry */
        if (_stepping) {
                /* Overwrite TRAPA instruction */
                p = (uint16_t *)_bp_step.addr;
                *p = _bp_step.instruction;

                /* Clear */
                _bp_step.addr = 0x00000000;
                _bp_step.instruction = 0x0000;

                _stepping = false;
        }
}

static bool
_gdb_register_file_read(struct cpu_registers *reg_file, uint32_t n, uint32_t *r)
{
        switch (n) {
        case REG_R0:
        case REG_R1:
        case REG_R2:
        case REG_R3:
        case REG_R4:
        case REG_R5:
        case REG_R6:
        case REG_R7:
        case REG_R8:
        case REG_R9:
        case REG_R10:
        case REG_R11:
        case REG_R12:
        case REG_R13:
        case REG_R14:
                *r = reg_file->r[n];
                return true;
        case REG_SP:
                *r = reg_file->sp;
                return true;
        case REG_MACL:
                *r = reg_file->macl;
                return true;
        case REG_MACH:
                *r = reg_file->mach;
                return true;
        case REG_VBR:
                *r = reg_file->vbr;
                return true;
        case REG_GBR:
                *r = reg_file->gbr;
                return true;
        case REG_PR:
                *r = reg_file->pr;
                return true;
        case REG_PC:
                *r = reg_file->pc;
                return true;
        case REG_SR:
                *r = reg_file->sr;
                return true;
        default:
                return false;
        }
}

static bool
_gdb_register_file_write(struct cpu_registers *reg_file, uint32_t n, uint32_t r)
{
        uint32_t *p;

        switch (n) {
        case REG_R0:
        case REG_R1:
        case REG_R2:
        case REG_R3:
        case REG_R4:
        case REG_R5:
        case REG_R6:
        case REG_R7:
        case REG_R8:
        case REG_R9:
        case REG_R10:
        case REG_R11:
        case REG_R12:
        case REG_R13:
        case REG_R14:
                p = &reg_file->r[n];
                break;
        case REG_SP:
                p = &reg_file->sp;
                break;
        case REG_MACL:
                p = &reg_file->macl;
                break;
        case REG_MACH:
                p = &reg_file->mach;
                break;
        case REG_VBR:
                p = &reg_file->vbr;
                break;
        case REG_GBR:
                p = &reg_file->gbr;
                break;
        case REG_PR:
                p = &reg_file->pr;
                break;
        case REG_PC:
                p = &reg_file->pc;
                break;
        case REG_SR:
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
_calculate_pc(struct cpu_registers *reg_file)
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
                if (reg_file->sr & CPU_SR_T_BIT_MASK) {
                        disp = OPCODE_8_DISP(opcode);
                        pc = reg_file->pc + (disp << 1) + 0x00000004;
                }
        } else if (OPCODE_BF(opcode) || OPCODE_BFS(opcode)) {
                if ((reg_file->sr & CPU_SR_T_BIT_MASK) == 0x00000000) {
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
        } else if (OPCODE_RTE(opcode)) {
                pc = *(uint32_t *)reg_file->sp;
        } else if (OPCODE_TRAPA(opcode)) {
                pc = *(uint32_t *)(reg_file->vbr +
                    (OPCODE_TRAPA_IMM(opcode) << 1));
        }

        return pc;
}

static bp_list_t *
_bp_list_alloc(void)
{
        bp_list_t *bpl;

        if ((bpl = (bp_list_t *)malloc(sizeof(bp_list_t))) == NULL) {
                return NULL;
        }

        /* Initialize queue */
        TAILQ_INIT(bpl);

        return bpl;
}

static bool
_bp_list_empty(bp_list_t *bpl)
{
        if (TAILQ_EMPTY(bpl)) {
                return true;
        }

        return false;
}

static void
_bp_list_free(bp_list_t **bplp)
{
        assert(*bplp != NULL);

        free(*bplp);
        *bplp = NULL;
}

static bp_t *
_bp_list_breakpoint_alloc(void)
{
        bp_t *bp;

        if ((bp = (bp_t *)malloc(sizeof(bp_t))) == NULL) {
                return NULL;
        }

        bp->addr = NULL;
        bp->instruction = 0x0000;

        return bp;
}

static void
_bp_list_breakpoint_free(bp_list_t *bpl, bp_t *bp)
{
        uint16_t *p;

        assert(bp != NULL);

        if (TAILQ_EMPTY(bpl)) {
                return;
        }

        p = (uint16_t *)bp->addr;
        *p = bp->instruction;

        TAILQ_REMOVE(bpl, bp, entries);
        free(bp);
}

static int
_bp_list_breakpoint_add(bp_list_t *bpl, void *addr)
{
        bp_t *bp;

        assert(bpl != NULL);

        /* Check if we have a breakpoint of the same address already present */
        if ((_bp_list_breakpoint_find(_bp_list, (void *)addr)) != NULL) {
                return 0;
        }

        if ((bp = _bp_list_breakpoint_alloc()) == NULL) {
                return -1;
        }

        bp->addr = addr;
        bp->instruction = *(uint16_t *)addr;
        *(uint16_t *)addr = INSTRN_TRAPA(0x20);

        /* Insert */
        TAILQ_INSERT_TAIL(bpl, bp, entries);

        return 0;
}

static bp_t *
_bp_list_breakpoint_find(bp_list_t *bpl, void *addr)
{
        bp_t *bp_np;

        assert(bpl != NULL);

        if (TAILQ_EMPTY(bpl)) {
                return NULL;
        }

        TAILQ_FOREACH (bp_np, bpl, entries) {
                if (bp_np->addr == addr) {
                        return bp_np;
                }
        }

        return NULL;
}

static void
_device_init(void)
{
#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        usb_cart_init();
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#endif /* HAVE_DEV_CARTRIDGE */
}

static uint8_t
_device_read(void)
{
#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        usb_cart_txe_full();
        return usb_cart_byte_read();
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
        return arp_byte_read();
#endif /* HAVE_DEV_CARTRIDGE */
}

static void
_device_write(uint8_t value)
{
#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        usb_cart_rxf_wait();
        usb_cart_byte_send(value);
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
        arp_byte_xchg(value);
#endif /* HAVE_DEV_CARTRIDGE */
}
