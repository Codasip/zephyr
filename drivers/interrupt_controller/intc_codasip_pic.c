/* Based on intc_plic.c
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 * Contributors: 2018 Antmicro <www.antmicro.com>
 *
 * Changes Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT codasip_pic

/**
 * @brief Codasip Peripheral Interrupt Controller (PIC) driver
 *        for RISC-V processors
 */

/* Useful info:
 * https://docs.zephyrproject.org/latest/kernel/services/interrupts.html
 *
 */
#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/init.h>
#include <soc.h>

#include <zephyr/sw_isr_table.h>
#include <zephyr/drivers/interrupt_controller/codasip_pic.h>
#include <zephyr/irq.h>
#include <zephyr/arch/riscv/csr.h>
#include <zephyr/arch/common/ffs.h>	/* For find_lsb_set() */

//#define PLIC_MAX_PRIO	DT_INST_PROP(0, riscv_max_priority)
//#define PLIC_PRIO	DT_INST_REG_ADDR_BY_NAME(0, prio)
//#define PLIC_IRQ_EN	DT_INST_REG_ADDR_BY_NAME(0, irq_en)
//#define PLIC_REG	DT_INST_REG_ADDR_BY_NAME(0, reg)

/* RISC-V Custom Control and Status Registers */
#define CCSR_MPICFLAG	0x7c1
#define CCSR_MPICFLAG1	0x7c4
#define CCSR_MPICFLAG2	0x7c7
#define CCSR_MPICFLAG3	0x7ca

#define CCSR_MPICMASK	0x7c2
#define CCSR_MPICMASK1	0x7c5
#define CCSR_MPICMASK2	0x7c8
#define CCSR_MPICMASK3	0x7cb

/* #define CCSR_MEXCAUSE	0x7d4 */

#define PIC_IRQS        (CONFIG_NUM_IRQS - CONFIG_2ND_LVL_ISR_TBL_OFFSET)
#define PIC_EN_SIZE     ((PIC_IRQS >> 5) + 1)

#if 0
struct plic_regs_t {
	uint32_t threshold_prio;
	uint32_t claim_complete;
};
#endif

static int save_irq;

/**
 * @brief Enable a Codasip PIC-specific interrupt line
 *
 * This routine enables a Codasip PIC-specific interrupt line.
 * codasip_pic_irq_enable is called by SOC_FAMILY_RISCV_PRIVILEGE
 * arch_irq_enable function to enable external interrupts for
 * IRQS level == 2, whenever CONFIG_CODASIP_HAS_PIC variable is set.
 *
 * @param irq IRQ number to enable
 */
void codasip_pic_irq_enable(uint32_t irq)
{
	uint32_t key, reg;

	key = irq_lock();

	reg  = irq >> 5;
	irq &= 31;

	switch( reg )
	{
	case 0:
		csr_read_set( CCSR_MPICMASK,  1 << irq );
		break;

	case 1:
		csr_read_set( CCSR_MPICMASK1, 1 << irq );
		break;

	case 2:
		csr_read_set( CCSR_MPICMASK2, 1 << irq );
		break;

	case 3:
		csr_read_set( CCSR_MPICMASK3, 1 << irq );
		break;
	} 

	irq_unlock(key);
}

/**
 * @brief Disable a Codasip PIC-specific interrupt line
 *
 * This routine disables a Codasip PIC-specific interrupt line.
 * codasip_pic_irq_disable is called by SOC_FAMILY_RISCV_PRIVILEGE
 * arch_irq_disable function to disable external interrupts, for
 * IRQS level == 2, whenever CONFIG_CODASIP_HAS_PIC variable is set.
 *
 * @param irq IRQ number to disable
 */
void codasip_pic_irq_disable(uint32_t irq)
{
	uint32_t key, reg;

	key = irq_lock();

	reg  = irq >> 5;
	irq &= 31;

	switch( reg )
	{
	case 0:
		csr_read_clear( CCSR_MPICMASK,  1 << irq );
		break;

	case 1:
		csr_read_clear( CCSR_MPICMASK1, 1 << irq );
		break;

	case 2:
		csr_read_clear( CCSR_MPICMASK2, 1 << irq );
		break;

	case 3:
		csr_read_clear( CCSR_MPICMASK3, 1 << irq );
		break;
	} 

	irq_unlock(key);
}

/**
 * @brief Check if a Codasip PIC-specific interrupt line is enabled
 *
 * This routine checks if a Codasip PIC-specific interrupt line is enabled.
 * @param irq IRQ number to check
 *
 * @return 1 or 0
 */
int codasip_pic_irq_is_enabled(uint32_t irq)
{
	uint32_t enabled = 0, reg;

	reg  = irq >> 5;
	irq &= 31;

	switch( reg )
	{
	case 0:
		enabled = csr_read( CCSR_MPICMASK  );
		break;

	case 1:
		enabled = csr_read( CCSR_MPICMASK1 );
		break;

	case 2:
		enabled = csr_read( CCSR_MPICMASK2 );
		break;

	case 3:
		enabled = csr_read( CCSR_MPICMASK3 );
		break;
	} 

	return ( enabled & ( 1UL << irq ) ) != 0;
}

#if 0
PRIORITY NOT SUPPORTED IN PIC
/**
 * @brief Set priority of a riscv PLIC-specific interrupt line
 *
 * This routine set the priority of a RISCV PLIC-specific interrupt line.
 * riscv_plic_irq_set_prio is called by riscv arch_irq_priority_set to set
 * the priority of an interrupt whenever CONFIG_RISCV_HAS_PLIC variable is set.
 *
 * @param irq IRQ number for which to set priority
 * @param priority Priority of IRQ to set to
 */
void riscv_plic_set_priority(uint32_t irq, uint32_t priority)
{
	volatile uint32_t *prio = (volatile uint32_t *)PLIC_PRIO;

	if (priority > PLIC_MAX_PRIO)
		priority = PLIC_MAX_PRIO;

	prio += irq;
	*prio = priority;
}
#endif

/**
 * @brief Get Codasip PIC-specific interrupt line causing an interrupt
 *
 * This routine returns the Codasip PIC-specific interrupt line causing an
 * interrupt.
 *
 * @return PIC-specific interrupt line causing an interrupt.
 */
int codasip_pic_get_irq(void)
{
	return save_irq;
}

static void pic_irq_handler(const void *arg)
{
	uint32_t irq = 0, pending;
	struct _isr_table_entry *ite;

	/* Get the IRQ number generating the interrupt */
	pending = csr_read( CCSR_MPICFLAG ) & csr_read( CCSR_MPICMASK );

	if ( pending == 0 )
	{
		irq += 32;
		pending = csr_read( CCSR_MPICFLAG1 ) & csr_read( CCSR_MPICMASK1 );
	}

	if ( pending == 0 )
	{
		irq += 32;
		pending = csr_read( CCSR_MPICFLAG2 ) & csr_read( CCSR_MPICMASK2 );
	}

	if ( pending == 0 )
	{
		irq += 32;
		pending = csr_read( CCSR_MPICFLAG3 ) & csr_read( CCSR_MPICMASK3 );
	}

	if ( pending == 0 )
	{
		irq += 32;
	}
	else
	{
		irq += find_lsb_set( pending ) - 1; /* Note: This function returns the the first set lsb bit pos + 1 (!). alternative: __builtin_ffs( pending ); */
	}

	/*
	 * Save IRQ in save_irq. To be used, if need be, by
	 * subsequent handlers registered in the _sw_isr_table table,
	 * as IRQ number held by the claim_complete register is
	 * cleared upon read.
	 */
	save_irq = irq;

	/*
	 * If the IRQ is out of range, call z_irq_spurious.
	 * A call to z_irq_spurious will not return.
	 */
	if (irq == 0U || irq >= PIC_IRQS)
		z_irq_spurious(NULL);

	irq += CONFIG_2ND_LVL_ISR_TBL_OFFSET;

	/* Call the corresponding IRQ handler in _sw_isr_table */
	ite = (struct _isr_table_entry *)&_sw_isr_table[irq];
	ite->isr(ite->arg);

	/*
	 * Clear the IRQ PIC Flag because the IRQ has been handled.
	 */
	uint32_t reg;

	irq  = save_irq;
	reg  = irq >> 5;
	irq &= 31;

	switch( reg )
	{
	case 0:
		csr_read_clear( CCSR_MPICFLAG,  1 << irq );
		break;

	case 1:
		csr_read_clear( CCSR_MPICFLAG1, 1 << irq );
		break;

	case 2:
		csr_read_clear( CCSR_MPICFLAG2, 1 << irq );
		break;

	case 3:
		csr_read_clear( CCSR_MPICFLAG3, 1 << irq );
		break;
	} 
}

/**
 * @brief Initialize the Codasip Peripheral Interrupt Controller
 *
 * @retval 0 on success.
 */
static int pic_init(void)
{
	/* Ensure that all interrupts are disabled initially */
	csr_write( CCSR_MPICMASK,  0 );
	csr_write( CCSR_MPICMASK1, 0 );
	csr_write( CCSR_MPICMASK2, 0 );
	csr_write( CCSR_MPICMASK3, 0 );

	/* Ensure that all interrupts are cleared initially */
	csr_write( CCSR_MPICFLAG,  0 );
	csr_write( CCSR_MPICFLAG1, 0 );
	csr_write( CCSR_MPICFLAG2, 0 );
	csr_write( CCSR_MPICFLAG3, 0 );

	/* Setup IRQ handler for PIC driver */
	IRQ_CONNECT(RISCV_MACHINE_EXT_IRQ,
		    0,
		    pic_irq_handler,
		    NULL,
		    0);

	/* Enable IRQ for PIC driver */
	irq_enable(RISCV_MACHINE_EXT_IRQ);

	return 0;
}

SYS_INIT(pic_init, PRE_KERNEL_1, CONFIG_INTC_INIT_PRIORITY);
