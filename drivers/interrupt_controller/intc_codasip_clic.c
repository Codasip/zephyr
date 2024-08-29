/* Copyright (c) 2024 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT codasip_clic

/**
 * @brief Codasip Core Level Interrupt Controller (CLIC) driver
 *        for RISC-V processors
 */

/* Useful info - External:
 * https://docs.zephyrproject.org/latest/kernel/services/interrupts.html
 * https://github.com/riscv/riscv-fast-interrupt/blob/5ef35cd40f809018a4a90b97fa0a7f5b4e150eba/clic.adoc
 * (https://github.com/riscv/riscv-fast-interrupt/blob/v0.9/src/clic.adoc)
 *
 * Useful info - Codasip Internal:
 * https://gitlab.codasip.com/codasip-ip-cores/pilsner/-/blob/main/pilsner/presets/eagle/ip.conf
 * https://codasip.atlassian.net/wiki/spaces/Pilsner/pages/1120993496/L100+Series+CLIC+Specification+v0.9.7
 */
#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>
#include <zephyr/irq_multilevel.h>
#include <zephyr/init.h>
#include <soc.h>

#include <zephyr/sw_isr_table.h>
#include <zephyr/drivers/interrupt_controller/riscv_clic.h>
#include <zephyr/irq.h>
#include <zephyr/arch/riscv/csr.h>

/* CLIC Mode for MTVT CSR Register */
#define MTVEC_CLIC_MODE (3U)

/* CLIC INTATTR Trigger Mask */
#define CLICINTATTR_TRIG_MASK  (3U)

/* CLIC INTATTR Modes */
#define CLICINTATTR_MODE_USER         (0)
#define CLICINTATTR_MODE_SUPERVISOR   (1)
#define CLICINTATTR_MODE_MACHINE      (3)


/* This driver is (currently) for the following Codasip CLIC Configuration:
    CLIC: true
    CLIC_NUM_INTERRUPT: 64
    CLICINTCTLBITS: 3
    CLIC_VECTORING: "SHV"
    CLIC_INT_TRIGGER_POLARITY: "DYNAMIC"
    CLIC_INT_TRIGGER_TYPE: "DYNAMIC"
    SSCLIC: false
    SMCLIC: true
    CLICBASE: 1610612736
    CLICINTATTR_NOT_EMPTY: true
    
    Other configuration:
    M mode only, so nmbits == 0
    smclicconfig is always present in Codasip's CLIC component
    CLIC Interrupts start at 0 (not 16)
*/

/* Get some of the CLIC configuration from the Device Tree */
#define CLIC_NUM_INTERRUPT  (DT_PROP(DT_NODELABEL(clic), num_interrupt))
#define CLICINTCTLBITS      (DT_PROP(DT_NODELABEL(clic), intctlbits))
#define CLICCFG_ADDR        (DT_REG_ADDR_BY_IDX(DT_NODELABEL(clic), 0))
#define CLICINT_ADDR        (DT_REG_ADDR_BY_IDX(DT_NODELABEL(clic), 1))

/* From the CLIC table:

The smclicconfig extension uses the following scheme for implementations that choose to implement fewer
than 8-bits in the clicintctl registers is as follows. The implemented bits are kept left-justified
in the most-significant bits of each 8-bit clicintctl[i] register, with the lower unimplemented bits
treated as hardwired to 1. The parameter CLICINTCTLBITS represents the number of implemented bits
in this scheme.

Examples of xcliccfg settings:

 CLICINTCTLBITS xnlbits clicintctl[i] interrupt levels
       0         2      ........      255
       1         2      l.......      127,255
       2         2      ll......      63,127,191,255
       3         3      lll.....      31,63,95,127,159,191,223,255
       4         1      lppp....      127,255

The parameter xNLBITS (MNLBITS, SNLBITS, etc) defines for each privilege mode how many upper bits in 
clicintctl[i] are assigned to encode the interrupt level at that privilege level, the remaining lower
bits of clicintctl[i] encode the priority.

Notice that the 8-bit interrupt level is used to determine preemption (for nesting interrupts). 
In contrast, the 8-bit interrupt priority does not affect preemption but is only used as a 
tie-breaker when there are multiple pending interrupts with the same interrupt level. 
*/

static uint8_t clic_max_level;
static uint8_t clic_max_priority;

/*
All CLIC registers are accessible from M-mode.
Interrupts with clicintattr[i].mode == S are accessible from S-mode, other interrupts read zero and
writes have no effect when accessed from S-mode.
Accesses from any other privilege modes than S and M read zero and writes have no effect.


Table 1. CLIC memory map, i denotes the interrupt index

Offset to CLICBASE  Size    Access                                              Name
0x0                 4B      RW                                                  xcliccfg

0x1000 + 4*i        1B      R if clicintattr[i].trig[0] == LEVEL, otherwise RW  clicintip[i]
0x1001 + 4*i        1B      RW                                                  clicintie[i]
0x1002 + 4*i        1B      RW                                                  clicintattr[i]
0x1003 + 4*i        1B      RW                                                  clicintctl[i]

Accessing any other address than specified in the memory map table reads zero and writes have no effect. 
CLIC address space has 32 KiB (round_to_nearest_bigger_power_of_2(0x1000 + 4 * 4096). 
Therefore, CLIC component checks only the 15 lowest address bits, higher bits are ignored.
*/

/* xcliccfg

Due to the implementation choice of aliasing MCLICBASE and SCLICBASE to the same address, Codasip CLIC
component has only one configuration register - xcliccfg, which serves as both mcliccfg and scliccfg
depending on the cores privilege mode.
When xcliccfg is accesed from M-mode, it behaves like mcliccfg and all of it’s fields are readable and writable.
When it is accessed from S-mode, it behaves like scliccfg and fields nmbits and mnlbits behave as WPRI 0.
Accesses from other privilege modes read 0 and writes are ignored.

  xcliccfg register layout
  located at CLICBASE

  Bits    Field
  31:28   reserved (WPRI 0)
  27:24   reserved (WPRI 0)
  23:20   reserved (WPRI 0)
  19:16   snlbits[3:0] if ssclic is supported, else reserved (WPRI 0)
  15:6    reserved (WPRI 0)
   5:4    nmbits[1:0] if accessed from M-mode and ssclic is supported, else WPRI 0.
   3:0    mnlbits[3:0] if accessed from M-mode, else WPRI 0

When ssclic extension is supported, xcliccfg.nmbits is implemented as a single bit, otherwise it is RO 0. This field
can be used to override privilege specified in the clicintattr[i].mode field:

Interrupt Mode Table
priv-modes nmbits clicintattr[i].mode  Interpretation
       M      0       xx               M-mode interrupt

     M/U      0       xx               M-mode interrupt
     M/U      1       0x               U-mode interrupt
     M/U      1       1x               M-mode interrupt

   M/S/U      0       xx               M-mode interrupt
   M/S/U      1       0x               S-mode interrupt
   M/S/U      1       1x               M-mode interrupt
   M/S/U      2       00               U-mode interrupt
   M/S/U      2       01               S-mode interrupt
   M/S/U      2       10               Reserved (or extended S-mode)
   M/S/U      2       11               M-mode interrupt

   M/S/U      3       xx               Reserved
*/
   
typedef union
{
    struct
    {
        uint32_t    mnlbits   :  4; /* if accessed from M-mode, else WPRI 0 */
        uint32_t    nmbits    :  2; /* if accessed from M-mode and ssclic is supported, else WPRI 0. */
        uint32_t    reserved0 : 10; /* (WPRI 0) */
        uint32_t    snlbits   :  4; /* if ssclic is supported, else reserved (WPRI 0) */
        uint32_t    reserved1 :  4; /* (WPRI 0) */
        uint32_t    reserved2 :  4; /* (WPRI 0) */
        uint32_t    reserved3 :  4; /* (WPRI 0) */
    };
    uint32_t reg;
} xcliccfg_t;



/* clicintip

clicintip register has a 8 bits and signals whether there is a pending interrupt.
This bit can be written by software only when interrupt is configured as an edge interrupt (clicintattr.trig[0] == 1).

  clicintip register layout
  located at CLICBASE + 0x1000 + 4*i

  Bits    Field
  7:1    reserved (WPRI 0)
  0      pending
*/

typedef struct
{
    uint8_t     pending     : 1;    /* 1 = Interrupt Pending */
    uint8_t     reserved    : 7;    /* (WPRI 0) */
} clicintip_t;



/* clicintie

clicintie register has 8 bits and is used for enabling and disabling interrupts.



  clicintie register layout
  located at CLICBASE + 0x1001 + 4*i

  Bits    Field
  7:1    reserved (WPRI 0)
  0      enable
*/

typedef struct
{
    uint8_t     enabled     : 1;    /* 1 = Interrupt Enabled */
    uint8_t     reserved    : 7;    /* (WPRI 0) */
} clicintie_t;



/* clicintattr

clicintattr is an 8 bit register that has the following fields:

 clicintattr register layout
  located at CLICBASE + 0x1002 + 4*i

  Bits    Field
  7:6     mode
  5:3     reserved (WPRI 0)
  2:1     trig
  0       shv

mode is a WARL field. Without ssclic extension this field is a read only 0.
With ssclic this field is physcially implemented as a single bit and can contain either a 1 for M-mode
interrupts or 0 for S-mode interrupts.

When CLIC_VECTORING is set to NONE, shv field is RO zero.
When CLIC_VECTORING is set to SHV_ALL, shv field is RO one.
When CLIC_VECTORING is set to SHV, shv field is RW.
*/

typedef union
{
    struct
    {
        uint8_t     shv         : 1;    /* 1 = Enable Hardware vectoring */
        uint8_t     trig        : 2;    /* trig[1:0]  Polarity      Trigger-Type
                                                      ------------  ---------------
                                                0 0   Logical 1     Level triggered (Reset value)
                                                0 1   Pos edge      Edge  triggered
                                                1 0   Logical 0     Level triggered
                                                1 1   Neg edge      Edge  triggered */
        uint8_t     reserved    : 3;
        uint8_t     mode        : 2;
    };
    uint8_t reg;
} clicintattr_t;



/* clicintctl

clicintctl is an 8 bit register that specifies interrupt priority and level.
The number of bits that are physicaly implemented in clicintctl is specified by the CLICINTCTLBITS option.

The split between level and priority bits is controlled by the mnlbits and snlbits fields in xcliccfg register.
When CLICINTCTLBITS is less than 8 bits, then these bits are left-justified and appended with 1 bits for the
lower missing bits.

The least-significant bits in clicintctl[i] that are not configured to be part of the interrupt level
are interrupt priority, which are used to prioritize among interrupts pending-and-enabled at the same
privilege mode and interrupt level. The highest-priority interrupt at a given privilege mode and
interrupt level is taken first. In case there are multiple pending-and-enabled interrupts at the same
highest priority, the highest-numbered interrupt is taken first.

Notice that the 8-bit interrupt level is used to determine preemption (for nesting interrupts). 
In contrast, the 8-bit interrupt priority does not affect preemption but is only used as a tie-breaker
when there are multiple pending interrupts with the same interrupt level.

Examples of xcliccfg settings:

 CLICINTCTLBITS xnlbits clicintctl[i] interrupt levels
       0         2      ........      255
       1         2      l.......      127,255
       2         2      ll......      63,127,191,255
       3         3      lll.....      31,63,95,127,159,191,223,255
       4         1      lppp....      127,255

 "." bits are non-existent bits for level encoding, assumed to be 1
 "l" bits are available variable bits in level specification
 "p" bits are available variable bits in priority specification

*/

typedef union
{
    struct
    {
        uint8_t     __ones  : 8 - CLICINTCTLBITS;   /* Read Only bits, read as 1's */
        uint8_t     level   : CLICINTCTLBITS;
    };
    uint8_t reg;
} clicintctl_t;

typedef union
{
    struct
    {
        clicintip_t     clicintip;
        clicintie_t     clicintie;
        clicintattr_t   clicintattr;
        clicintctl_t    clicintctl;
    };
    uint32_t reg;
} clicint_t;

static volatile xcliccfg_t *xcliccfg = (volatile xcliccfg_t *) CLICCFG_ADDR;

static volatile clicint_t (*clicint)[CLIC_NUM_INTERRUPT] = (volatile clicint_t (*)[]) (CLICINT_ADDR);

/* Interrupt enable */
void riscv_clic_irq_enable(uint32_t irq)
{
    (*clicint)[irq].clicintie.enabled = 1;
}

/* Interrupt disable */
void riscv_clic_irq_disable(uint32_t irq)
{
    (*clicint)[irq].clicintie.enabled = 0;
}

/* Get interrupt enable status */
int riscv_clic_irq_is_enabled(uint32_t irq)
{
    return (*clicint)[irq].clicintie.enabled;
}

/* Set interrupt priority/level */
void riscv_clic_irq_priority_set(uint32_t irq, uint32_t pri, uint32_t flags)
{
    clicintattr_t   l_clicintattr = { .reg = 0 };

    /* Set the CLIC Interrupt Level to the Zephyr Priority (we are not using CLIC Priority) */
    if (pri > clic_max_level)
    {
        pri = clic_max_level;
    }
    (*clicint)[irq].clicintctl.level = pri;


#if defined(CONFIG_RISCV_VECTORED_MODE) && !defined(CONFIG_LEGACY_CLIC)
	/*
	 * Set Selective Hardware Vectoring.
	 * Legacy SiFive does not implement smclicshv extension and vectoring is
	 * enabled in the mode bits of mtvec.
	 */
    l_clicintattr.shv = 1;

#else
    l_clicintattr.shv = 0;
#endif

    /* Set the trigger mode as requested by flags (0 being the default) */
    l_clicintattr.trig = (uint8_t) (flags & CLICINTATTR_TRIG_MASK);

    /* Set the mode to Machine */
    /* l_clicintattr.mode = CLICINTATTR_MODE_MACHINE; Not required as we are in Machine only config, 
     *                                                see "Interrupt Mode Table" (above) */
    (*clicint)[irq].clicintattr = l_clicintattr;
}

/* Set interrupt pending bit */
void riscv_clic_irq_set_pending(uint32_t irq)
{
    (*clicint)[irq].clicintip.pending = 1;
}


static int codasip_clic_init(const struct device *dev)
{
    int i;

    /* Reset the CLIC */
    for (i = 0; i < CLIC_NUM_INTERRUPT; i++)
    {
        (*clicint)[i].reg = 0UL;
    }

    xcliccfg->reg = 0UL;

    /* Set all privilege modes to operate in CLIC mode:
     * 
     * CLIC mode xtvec register layout
     *
     *  Bits          Field
     *  XLEN-1:6      base (WARL)
     *  5:2           submode (WARL)
     *  1:0           mode (WARL)
     * 
     * (xtvec[5:0])
     * submode mode  Action on Interrupt
     *    aaaa 00    pc := OBASE                       (CLINT non-vectored basic mode)
     *    aaaa 01    pc := OBASE + 4 * exccode         (CLINT vectored basic mode)
     *
     *    0000 11                                      (CLIC mode)
     *               (non-vectored)
     *               pc := NBASE
     *
     *    0000 10                                      Reserved
     *    xxxx 1?    (xxxx!=0000)                      Reserved
     *
     * OBASE = xtvec[XLEN-1:2]<<2   # CLINT mode vector base is at least 4-byte aligned.
     * NBASE = xtvec[XLEN-1:6]<<6   # CLIC mode vector base is at least 64-byte aligned.
     * TBASE = xtvt[XLEN-1:6]<<6    # Software trap vector table base is at least 64-byte aligned.
     */
	csr_write(mtvec, ((csr_read(mtvec) & 0xFFFFFFC0) | MTVEC_CLIC_MODE));

    /* We are going to use only CLIC Interrupt Levels (no Priority).
     * 
     * Zephyr uses the term Priority which we will use for the CLIC Interrupt Level.
     * NOTE: Zephyr Interrupt Levels refer to the use of Multi-Level Interrupt Controllers
     *       when lower level interrupt controllers feed in to a higher level interrupt
     *       controller. This is different than the CLIC definition of Level and Priority */
    xcliccfg->mnlbits = CLICINTCTLBITS;
    
    clic_max_level    = (1 << CLICINTCTLBITS) - 1;
    clic_max_priority = 0;

	return 0;
}

DEVICE_DT_INST_DEFINE(0, codasip_clic_init, NULL, NULL, NULL,
                      PRE_KERNEL_1, CONFIG_INTC_INIT_PRIORITY, NULL);
