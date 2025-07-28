//
// Copyright (c) 2020 Stanford University All rights reserved.
//
// This software was developed by
// Stanford University and the University of Cambridge Computer Laboratory
// under National Science Foundation under Grant No. CNS-0855268,
// the University of Cambridge Computer Laboratory under EPSRC INTERNET Project EP/H040536/1 and
// by the University of Cambridge Computer Laboratory under DARPA/AFRL contract FA8750-11-C-0249 ("MRC2"),
// as part of the DARPA MRC research programme.
//
// @NETFPGA_LICENSE_HEADER_START@
//
// Licensed to NetFPGA C.I.C. (NetFPGA) under one or more contributor
// license agreements.  See the NOTICE file distributed with this work for
// additional information regarding copyright ownership.  NetFPGA licenses this
// file to you under the NetFPGA Hardware-Software License, Version 1.0 (the
// "License"); you may not use this file except in compliance with the
// License.  You may obtain a copy of the License at:
//
//   http://www.netfpga-cic.org
//
// Unless required by applicable law or agreed to in writing, Work distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations under the License.
//
// @NETFPGA_LICENSE_HEADER_END@

#ifndef NFP_ENV_H_
#define NFP_ENV_H_

// This gives access to the top-level SDNet configuration file
#include "sdnet/sdnet_0_defs.h"

/****************************************************************************************************************************************************/
/* NetFPGA Plus Env Functions */
/****************************************************************************************************************************************************/

typedef struct NfpContext
{
    // SDNet base address
    XilSdnetAddressType SDNetBaseAddress;

    // Page aligned base address
    XilSdnetAddressType PCIBaseAddr;

    // Page aligned virtual base address
    XilSdnetAddressType MapBaseAddr;

    // mmap address range
    int MapSize;
} NfpContext;

XilSdnetReturnType NfpWordWrite32(XilSdnetEnvIf *EnvIfPtr,
                                  XilSdnetAddressType Address,
                                  uint32_t WriteValue);

XilSdnetReturnType NfpWordRead32(XilSdnetEnvIf *EnvIfPtr,
                                 XilSdnetAddressType Address,
                                 uint32_t *ReadValuePtr);

XilSdnetReturnType NfpLog(XilSdnetEnvIf *EnvIfPtr,
                          const char *MessagePtr);

XilSdnetReturnType NfpCreateEnvIf(XilSdnetEnvIf *EnvIfPtr,
                                  XilSdnetAddressType SdnetBaseAddress,
                                  const char *SysfsFile);

void NfpDestroyEnvIf(XilSdnetEnvIf *EnvIfPtr);

#endif // NFP_ENV_H_
