/**
  ******************************************************************************
  * @file    network.h
  * @author  STEdgeAI
  * @date    2026-09-22 12:37:14
  * @brief   Minimal description of the generated c-implemention of the network
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * Modifications Copyright (c) 2026 Nikita Prismakov.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef LL_ATON_NETWORK_H
#define LL_ATON_NETWORK_H

/******************************************************************************/
#define LL_ATON_NETWORK_C_MODEL_NAME        "cynipsnet"
#define LL_ATON_NETWORK_ORIGIN_MODEL_NAME   "cynipsnet"

/************************** USER ALLOCATED IOs ********************************/
// No user allocated inputs
// No user allocated outputs

/************************** INPUTS ********************************************/
#define LL_ATON_NETWORK_IN_NUM        (1)    // Total number of input buffers
// Input buffer 1 -- input
#define LL_ATON_NETWORK_IN_1_ALIGNMENT   (32)
#define LL_ATON_NETWORK_IN_1_SIZE_BYTES  (38400)

/************************** OUTPUTS *******************************************/
#define LL_ATON_NETWORK_OUT_NUM        (1)    // Total number of output buffers
// Output buffer 1 -- output
#define LL_ATON_NETWORK_OUT_1_ALIGNMENT   (32)
#define LL_ATON_NETWORK_OUT_1_SIZE_BYTES  (8700)

#endif /* LL_ATON_NETWORK_H */
