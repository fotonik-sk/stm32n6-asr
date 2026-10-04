/**
 * @file     mcu_rif.h
 * @brief    STM32N6 resource isolation (RIF): bus-master identities, peripheral
 *           security and memory firewalls.
 * @details  The single place that states the isolation policy of this image: it
 *           runs secure and privileged as CID1, and every bus master it drives
 *           (NPU, LTDC) gets that same identity so it reaches the same memories.
 *           RIF registers are volatile and reset with the chip. GPIO and GPDMA
 *           channel attributes live in those peripherals' own registers and stay
 *           with their drivers.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef HARDWARE_MCU_RIF_MCU_RIF_H
#define HARDWARE_MCU_RIF_MCU_RIF_H

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Give one bus master the CPU's secure, privileged CID1 identity and make
 *        its register port secure, so it can use every memory the CPU uses.
 * @param[in] master_index RIMC index of the bus master (@c RIF_MASTER_INDEX_*).
 * @param[in] periph_index RISC index of its register port (@c RIF_RISC_PERIPH_INDEX_*).
 * @pre The NPU, if it is the master, is clocked and out of reset.
 */
void mcu_rif_grant_cpu_identity(uint32_t master_index, uint32_t periph_index);

/**
 * @brief Open the firewalls in front of every memory an inference touches.
 * @details Without this the NPU starts its first epoch and never raises the
 *          completion interrupt. The set is: AXISRAM1/2, both NPU master
 *          ports, AXISRAM3..6, FLEXMEM, the NPU cache RAM and its configuration
 *          port, PSRAM on XSPI1 and the NOR on XSPI2.
 * @pre Every memory behind these firewalls is clocked: a firewall is programmed
 *      only once the memory behind it runs.
 */
void mcu_rif_open_memory_firewalls(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_MCU_RIF_MCU_RIF_H */
