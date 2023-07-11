/* Based on riscv_plic.h
 * Copyright (c) 2022 Carlo Caione <ccaione@baylibre.com>
 *
 * Changes Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Driver for Codasip Peripheral Interrupt Controller (PIC)
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_CODASIP_FPGA_PIC_H_
#define ZEPHYR_INCLUDE_DRIVERS_CODASIP_FPGA_PIC_H_

/**
 * @brief Enable interrupt
 *
 * @param irq interrupt ID
 */
void codasip_fpga_pic_irq_enable(uint32_t irq);

/**
 * @brief Disable interrupt
 *
 * @param irq interrupt ID
 */
void codasip_fpga_pic_irq_disable(uint32_t irq);

/**
 * @brief Check if an interrupt is enabled
 *
 * @param irq interrupt ID
 * @return Returns true if interrupt is enabled, false otherwise
 */
int codasip_fpga_pic_irq_is_enabled(uint32_t irq);

#if 0
PRIORITY NOT SUPPORTED IN PIC
/**
 * @brief Set interrupt priority
 *
 * @param irq interrupt ID
 * @param prio interrupt priority
 */
void codasip_fpga_pic_set_priority(uint32_t irq, uint32_t prio);
#endif

/**
 * @brief Get active interrupt ID
 *
 * @return Returns the ID of an active interrupt
 */
int codasip_fpga_pic_get_irq(void);

#endif /* ZEPHYR_INCLUDE_DRIVERS_CODASIP_FPGA_PIC_H_ */
