/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _IRQ_MUX_H_
#define _IRQ_MUX_H_

#include <inttypes.h>

#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct irq_mux_handle;

typedef TAILQ_HEAD(irq_mux_tq, irq_mux_handle) irq_mux_tq_t;

typedef struct {
        uint8_t im_total;
        irq_mux_tq_t im_tq;
} irq_mux_t;

typedef struct irq_mux_handle irq_mux_handle_t;

struct irq_mux_handle {
        irq_mux_t *imh;
        void (*imh_hdl)(irq_mux_handle_t *);
        void *imh_user_ptr;

        TAILQ_ENTRY(irq_mux_handle) handles;
};

extern void irq_mux_handle(irq_mux_t *);
extern void irq_mux_handle_add(irq_mux_t *, void (*)(irq_mux_handle_t *), void *);
extern void irq_mux_handle_remove(irq_mux_t *, void (*)(irq_mux_handle_t *));
extern void irq_mux_init(irq_mux_t *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_IRQ_MUX_H_ */
