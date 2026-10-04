/**
 * @file     mcu_rif.c
 * @brief    STM32N6 resource isolation policy; implements mcu_rif.h.
 * @author   Nikita Prismakov
 *
 * SPDX-FileCopyrightText: 2026 Nikita Prismakov
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************/
/*                                   INCLUDES                                  */
/*******************************************************************************/

#include "mcu_rif.h"

#include <stdint.h>

#include "stm32n6xx_hal.h"

/*******************************************************************************/
/*                              PRIVATE FUNCTIONS                              */
/*******************************************************************************/

/**
 * @brief Open one RISAF firewall to every compartment, secure and non-secure.
 * @param[in] risaf Firewall to open.
 * @param[in] limit Last address offset the firewall filters.
 * @details Two fully overlapping base regions, one per security state, each
 *          readable and writable by all CIDs.
 */
static void open_firewall(RISAF_TypeDef *risaf, uint32_t limit)
{
    RISAF_BaseRegionConfig_t region = {0};

    region.StartAddress = 0U;
    region.EndAddress = limit;
    region.Filtering = RISAF_FILTER_ENABLE;
    region.PrivWhitelist = RIF_CID_NONE;
    region.ReadWhitelist = RIF_CID_MASK;
    region.WriteWhitelist = RIF_CID_MASK;

    region.Secure = RIF_ATTRIBUTE_SEC;
    HAL_RIF_RISAF_ConfigBaseRegion(risaf, 0U, &region);
    region.Secure = RIF_ATTRIBUTE_NSEC;
    HAL_RIF_RISAF_ConfigBaseRegion(risaf, 1U, &region);
}

/*******************************************************************************/
/*                               PUBLIC FUNCTIONS                              */
/*******************************************************************************/

void mcu_rif_grant_cpu_identity(uint32_t master_index, uint32_t periph_index)
{
    RIMC_MasterConfig_t master = {0};

    /* The image runs secure and privileged as CID1. */
    __HAL_RCC_RIFSC_CLK_ENABLE();
    master.MasterCID = RIF_CID_1;
    master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
    HAL_RIF_RIMC_ConfigMasterAttributes(master_index, &master);
    HAL_RIF_RISC_SetSlaveSecureAttributes(periph_index, RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
}

void mcu_rif_open_memory_firewalls(void)
{
    open_firewall(RISAF2_S, RISAF2_LIMIT_ADDRESS_SPACE_SIZE);   /* AXISRAM1 */
    open_firewall(RISAF3_S, RISAF3_LIMIT_ADDRESS_SPACE_SIZE);   /* AXISRAM2 */
    open_firewall(RISAF4_S, RISAF4_LIMIT_ADDRESS_SPACE_SIZE);   /* NPU master port 0 */
    open_firewall(RISAF5_S, RISAF5_LIMIT_ADDRESS_SPACE_SIZE);   /* NPU master port 1 */
    open_firewall(RISAF6_S, RISAF6_LIMIT_ADDRESS_SPACE_SIZE);   /* AXISRAM3..6 */
    open_firewall(RISAF7_S, RISAF7_LIMIT_ADDRESS_SPACE_SIZE);   /* FLEXMEM */
    open_firewall(RISAF8_S, RISAF8_LIMIT_ADDRESS_SPACE_SIZE);   /* NPU cache RAM */
    open_firewall(RISAF15_S, RISAF15_LIMIT_ADDRESS_SPACE_SIZE); /* NPU cache configuration */
    open_firewall(RISAF11_S, RISAF11_LIMIT_ADDRESS_SPACE_SIZE); /* XSPI1: PSRAM */
    open_firewall(RISAF12_S, RISAF12_LIMIT_ADDRESS_SPACE_SIZE); /* XSPI2: NOR, weights */
}
