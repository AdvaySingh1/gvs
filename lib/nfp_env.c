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

// TODO add gigaflow offload vlogs for ovs-appctl
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

// This gives access to the top-level SDNet configuration file
#include "sdnet/sdnet_0_defs.h"

#include "nfp_env.h"

#define DISPLAY_ERROR(ErrorCode) printf("Error Code is value %s", XilSdnetReturnTypeToString(ErrorCode))

/****************************************************************************************************************************************************/
/* NetFPGA Plus Env Functions */
/****************************************************************************************************************************************************/

XilSdnetReturnType NfpWordWrite32(XilSdnetEnvIf *EnvIfPtr, XilSdnetAddressType Address, uint32_t WriteValue)
{
    if (EnvIfPtr == NULL)
    {
        return XIL_SDNET_GENERAL_ERR_NULL_PARAM;
    }

    // Get UserCtx
    NfpContext *NfpCtxPtr;
    NfpCtxPtr = (NfpContext *)EnvIfPtr->UserCtx;

    XilSdnetAddressType PciWriteAddress = Address + NfpCtxPtr->SDNetBaseAddress;
    XilSdnetAddressType VirtWriteAddress =
        PciWriteAddress + NfpCtxPtr->MapBaseAddr - NfpCtxPtr->PCIBaseAddr;

    *(uint32_t *)VirtWriteAddress = WriteValue;

    return XIL_SDNET_SUCCESS;
}

XilSdnetReturnType NfpWordRead32(XilSdnetEnvIf *EnvIfPtr, XilSdnetAddressType Address, uint32_t *ReadValuePtr)
{
    if (EnvIfPtr == NULL)
    {
        return XIL_SDNET_GENERAL_ERR_NULL_PARAM;
    }

    if (ReadValuePtr == NULL)
    {
        return XIL_SDNET_GENERAL_ERR_NULL_PARAM;
    }

    // Get UserCtx
    NfpContext *NfpCtxPtr;
    NfpCtxPtr = (NfpContext *)EnvIfPtr->UserCtx;

    XilSdnetAddressType PciReadAddress = Address + NfpCtxPtr->SDNetBaseAddress;
    XilSdnetAddressType VirtReadAddress =
        PciReadAddress + NfpCtxPtr->MapBaseAddr - NfpCtxPtr->PCIBaseAddr;

    *ReadValuePtr = *(uint32_t *)VirtReadAddress;

    return XIL_SDNET_SUCCESS;
}

XilSdnetReturnType NfpLog(XilSdnetEnvIf *EnvIfPtr, const char *MessagePtr)
{
    if (EnvIfPtr == NULL)
    {
        return XIL_SDNET_GENERAL_ERR_NULL_PARAM;
    }

    if (MessagePtr == NULL)
    {
        return XIL_SDNET_GENERAL_ERR_NULL_PARAM;
    }

    printf("%s\n", MessagePtr);

    return XIL_SDNET_SUCCESS;
}

XilSdnetReturnType NfpCreateEnvIf(XilSdnetEnvIf *EnvIfPtr,
                                  XilSdnetAddressType SDNetBaseAddress, const char *SysfsFile)
{
    XilSdnetReturnType Result;
    NfpContext *NfpCtxPtr;

    int SysfsFd;
    off_t PCIBaseAddr;
    XilSdnetAddressType MapBaseAddr;
    // TODO (praveenk): figure out right value of MapSize automatically
    int MapSize = 131072UL;

    SysfsFd = open(SysfsFile, O_RDWR | O_SYNC);

    if (SysfsFd == -1)
    {
        return XIL_SDNET_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
    }

    PCIBaseAddr = SDNetBaseAddress & ~(sysconf(_SC_PAGE_SIZE) - 1);

    MapBaseAddr = (XilSdnetAddressType)mmap(0, MapSize, PROT_READ | PROT_WRITE,
                                            MAP_SHARED, SysfsFd, PCIBaseAddr);
    if (MapBaseAddr == -1)
    {
        return XIL_SDNET_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE;
    }

    NfpCtxPtr = malloc(sizeof(NfpContext));
    NfpCtxPtr->SDNetBaseAddress = SDNetBaseAddress;
    NfpCtxPtr->PCIBaseAddr = PCIBaseAddr;
    NfpCtxPtr->MapBaseAddr = MapBaseAddr;
    NfpCtxPtr->MapSize = MapSize;

    /* Resuse stub and update functions */
    Result = XilSdnetStubEnvIf(EnvIfPtr);
    if (Result == XIL_SDNET_SUCCESS)
    {
        /* Replace stub functions */
        EnvIfPtr->WordWrite32 = NfpWordWrite32;
        EnvIfPtr->WordRead32 = NfpWordRead32;
        EnvIfPtr->LogError = NfpLog;
        EnvIfPtr->LogInfo = NfpLog;
        /* Replace stub UserCtx */
        EnvIfPtr->UserCtx = (XilSdnetUserCtxType)NfpCtxPtr;
    }
    return XIL_SDNET_SUCCESS;
}

void NfpDestroyEnvIf(XilSdnetEnvIf *EnvIfPtr)
{
    NfpContext *NfpCtxPtr = (NfpContext *)EnvIfPtr->UserCtx;
    if (munmap((void *)(NfpCtxPtr->MapBaseAddr), NfpCtxPtr->MapSize) == -1)
    {
        DISPLAY_ERROR(XIL_SDNET_GENERAL_ERR_INVALID_ENVIRONMENT_INTERFACE);
    }

    free(NfpCtxPtr);
}
