/*
* The Clear BSD License
* Copyright 2013-2016 Freescale Semiconductor, Inc.
* Copyright 2016-2018 NXP
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted (subject to the limitations in the
* disclaimer below) provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
* * Neither the name of the copyright holder nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
* GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
* HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "fsl_ftfx_cache.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @name Flash cache and speculation control defines
 * @{
 */
#if defined(MCM_PLACR_CFCC_MASK)
#define FLASH_CACHE_IS_CONTROLLED_BY_MCM (1)
#else
#define FLASH_CACHE_IS_CONTROLLED_BY_MCM (0)
#endif

#define FLASH_CACHE_IS_CONTROLLED_BY_MSCM (0)

#if defined(FMC_PFB0CR_CINV_WAY_MASK) || defined(FMC_PFB01CR_CINV_WAY_MASK)
#define FLASH_CACHE_IS_CONTROLLED_BY_FMC (1)
#else
#define FLASH_CACHE_IS_CONTROLLED_BY_FMC (0)
#endif

#if defined(MCM_PLACR_DFCS_MASK)
#define FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MCM (1)
#else
#define FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MCM (0)
#endif

#if defined(MSCM_OCMDR_OCMC1_MASK) || defined(MSCM_OCMDR_OCM1_MASK) || defined(MSCM_OCMDR0_OCM1_MASK) || \
        defined(MSCM_OCMDR1_OCM1_MASK)
#define FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM (1)
#else
#define FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM (0)
#endif

#if defined(FMC_PFB0CR_S_INV_MASK) || defined(FMC_PFB0CR_S_B_INV_MASK) || defined(FMC_PFB01CR_S_INV_MASK) || \
        defined(FMC_PFB01CR_S_B_INV_MASK)
#define FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC (1)
#else
#define FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC (0)
#endif

#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM || FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC || \
        FLASH_CACHE_IS_CONTROLLED_BY_MCM || FLASH_CACHE_IS_CONTROLLED_BY_FMC || FLASH_CACHE_IS_CONTROLLED_BY_MSCM
#define FLASH_IS_CACHE_INVALIDATION_AVAILABLE (1)
#else
#define FLASH_IS_CACHE_INVALIDATION_AVAILABLE (0)
#endif
/*@}*/

/*! @brief A function pointer used to point to relocated ftfx_common_bit_operation() */
typedef void (*callftfxCommonBitOperation_t)(FTFx_REG32_ACCESS_TYPE base,
                                              uint32_t bitMask,
                                              uint32_t bitShift,
                                              uint32_t bitValue);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#if FLASH_CACHE_IS_CONTROLLED_BY_MCM
/*! @brief Performs the cache clear to the flash by MCM.*/
void mcm_flash_cache_clear(ftfx_cache_config_t *config);
#endif /* FLASH_CACHE_IS_CONTROLLED_BY_MCM */

#if FLASH_CACHE_IS_CONTROLLED_BY_MSCM
/*! @brief Performs the cache clear to the flash by MSCM.*/
void mscm_flash_cache_clear(ftfx_cache_config_t *config);
#endif /* FLASH_CACHE_IS_CONTROLLED_BY_MSCM */

#if FLASH_CACHE_IS_CONTROLLED_BY_FMC
/*! @brief Performs the cache clear to the flash by FMC.*/
void fmc_flash_cache_clear(ftfx_cache_config_t *config);
#endif /* FLASH_CACHE_IS_CONTROLLED_BY_FMC */

#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM
/*! @brief Sets the prefetch speculation buffer to the flash by MSCM.*/
void mscm_flash_prefetch_speculation_enable(ftfx_cache_config_t *config, bool enable);
#endif /* FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM */

#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC
/*! @brief Performs the prefetch speculation buffer clear to the flash by FMC.*/
void fmc_flash_prefetch_speculation_clear(ftfx_cache_config_t *config);
#endif /* FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC */

#if FTFx_DRIVER_IS_FLASH_RESIDENT && FLASH_IS_CACHE_INVALIDATION_AVAILABLE
/*! @brief Copy flash_cache_clear_command() to RAM*/
static void ftfx_copy_common_bit_operation_to_ram(uint32_t *ftfxCommonBitOperation);
#endif /* FTFx_DRIVER_IS_FLASH_RESIDENT */

/*******************************************************************************
 * Variables
 ******************************************************************************/

#if FTFx_DRIVER_IS_FLASH_RESIDENT && FLASH_IS_CACHE_INVALIDATION_AVAILABLE
/*!
 * @brief Position independent code of ftfx_common_bit_operation()
 *
 * Note1: The prototype of C function is shown as below:
 * @code
 *   void ftfx_common_bit_operation(FTFx_REG32_ACCESS_TYPE base, uint32_t bitMask, uint32_t bitShift, uint32_t
 * bitValue)
 *   {
 *       if (bitMask)
 *       {
 *           uint32_t value = (((uint32_t)(((uint32_t)(bitValue)) << bitShift)) & bitMask);
 *           *base = (*base & (~bitMask)) | value;
 *       }
 *
 *       __ISB();
 *       __DSB();
 *   }
 * @endcode
 * Note2: The binary code is generated by IAR 7.70.1
 */
static const uint16_t s_ftfxCommonBitOperationFunctionCode[] = {
    0xb510, /* PUSH  {R4, LR} */
    0x2900, /* CMP   R1, #0 */
    0xd005, /* BEQ.N @12 */
    0x6804, /* LDR   R4, [R0] */
    0x438c, /* BICS  R4, R4, R1 */
    0x4093, /* LSLS  R3, R3, R2 */
    0x4019, /* ANDS  R1, R1, R3 */
    0x4321, /* ORRS  R1, R1, R4 */
    0x6001, /* STR   R1, [R0] */
    /*  @12: */
    0xf3bf, 0x8f6f, /* ISB */
    0xf3bf, 0x8f4f, /* DSB */
    0xbd10          /* POP   {R4, PC} */
};

#if (!FTFx_DRIVER_IS_EXPORTED)
/*! @brief A static buffer used to hold ftfx_common_bit_operation() */
static uint32_t s_ftfxCommonBitOperation[kFTFx_CACHE_RamFuncMaxSizeInWords];
#endif /* (!FTFx_DRIVER_IS_EXPORTED) */
#endif /* FLASH_IS_CACHE_INVALIDATION_AVAILABLE && FTFx_DRIVER_IS_FLASH_RESIDENT */

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t FTFx_CACHE_Init(ftfx_cache_config_t *config)
{
    if (config == NULL)
    {
        return kStatus_FTFx_InvalidArgument;
    }

/* copy required flash commands to RAM */
#if FTFx_DRIVER_IS_FLASH_RESIDENT && FLASH_IS_CACHE_INVALIDATION_AVAILABLE
    if (NULL == config->comBitOperFuncAddr)
    {
#if FTFx_DRIVER_IS_EXPORTED
        return kStatus_FTFx_ExecuteInRamFunctionNotReady;
#else
        config->comBitOperFuncAddr = s_ftfxCommonBitOperation;
#endif /* FTFx_DRIVER_IS_EXPORTED */
    }
    ftfx_copy_common_bit_operation_to_ram(config->comBitOperFuncAddr);
#endif /* FLASH_IS_CACHE_INVALIDATION_AVAILABLE && FTFx_DRIVER_IS_FLASH_RESIDENT */

    return kStatus_FTFx_Success;
}

/*!
 * @brief Flash Cache/Prefetch/Speculation Clear Process
 *
 * This function is used to perform the cache and prefetch speculation clear process to the flash.
 */
status_t FTFx_CACHE_ClearCachePrefetchSpeculation(ftfx_cache_config_t *config, bool isPreProcess)
{
    /* We pass the ftfx register address as a parameter to ftfx_common_bit_operation() instead of using
     * pre-processed MACROs or a global variable in ftfx_common_bit_operation()
     * to make sure that ftfx_common_bit_operation() will be compiled into position-independent code (PIC). */
    if (!isPreProcess)
    {
#if FLASH_CACHE_IS_CONTROLLED_BY_MCM
        mcm_flash_cache_clear(config);
#endif
#if FLASH_CACHE_IS_CONTROLLED_BY_MSCM
        mscm_flash_cache_clear(config);
#endif
#if FLASH_CACHE_IS_CONTROLLED_BY_FMC
        fmc_flash_cache_clear(config);
#endif
#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM
        mscm_flash_prefetch_speculation_enable(config, true);
#endif
#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC
        fmc_flash_prefetch_speculation_clear(config);
#endif
    }
    if (isPreProcess)
    {
#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM
        mscm_flash_prefetch_speculation_enable(config, false);
#endif
    }

    return kStatus_FTFx_Success;
}

status_t FTFx_CACHE_PflashSetPrefetchSpeculation(ftfx_prefetch_speculation_status_t *speculationStatus)
{
#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MCM
    {
        if (speculationStatus->instructionOff)
        {
            if (!speculationStatus->dataOff)
            {
                return kStatus_FTFx_InvalidSpeculationOption;
            }
            else
            {
                MCM0_CACHE_REG |= MCM_PLACR_DFCS_MASK;
            }
        }
        else
        {
            MCM0_CACHE_REG &= ~MCM_PLACR_DFCS_MASK;
            if (!speculationStatus->dataOff)
            {
                MCM0_CACHE_REG |= MCM_PLACR_EFDS_MASK;
            }
            else
            {
                MCM0_CACHE_REG &= ~MCM_PLACR_EFDS_MASK;
            }
        }
    }
#elif FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC
    {
        if (!speculationStatus->instructionOff)
        {
            FMC_CACHE_REG |= FMC_CACHE_B0IPE_MASK;
        }
        else
        {
            FMC_CACHE_REG &= ~FMC_CACHE_B0IPE_MASK;
        }
        if (!speculationStatus->dataOff)
        {
            FMC_CACHE_REG |= FMC_CACHE_B0DPE_MASK;
        }
        else
        {
            FMC_CACHE_REG &= ~FMC_CACHE_B0DPE_MASK;
        }

        /* Invalidate Prefetch Speculation Buffer */
        FMC_SPECULATION_INVALIDATE_REG |= FMC_SPECULATION_INVALIDATE_MASK;
    }
#elif FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM
    {
        if (speculationStatus->instructionOff)
        {
            if (!speculationStatus->dataOff)
            {
                return kStatus_FTFx_InvalidSpeculationOption;
            }
            else
            {
                MSCM_OCMDR0_REG |= MSCM_OCMDR_OCMC1_DFCS_MASK;
            }
        }
        else
        {
            MSCM_OCMDR0_REG &= ~MSCM_OCMDR_OCMC1_DFCS_MASK;
            if (!speculationStatus->dataOff)
            {
                MSCM_OCMDR0_REG &= ~MSCM_OCMDR_OCMC1_DFDS_MASK;
            }
            else
            {
                MSCM_OCMDR0_REG |= MSCM_OCMDR_OCMC1_DFDS_MASK;
            }
        }
    }
#endif /* FSL_FEATURE_FTFx_MCM_FLASH_CACHE_CONTROLS */

    return kStatus_FTFx_Success;
}

status_t FTFx_CACHE_PflashGetPrefetchSpeculation(ftfx_prefetch_speculation_status_t *speculationStatus)
{
    memset(speculationStatus, 0, sizeof(ftfx_prefetch_speculation_status_t));

    /* Assuming that all speculation options are enabled. */
    speculationStatus->instructionOff = false;
    speculationStatus->dataOff = false;

#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MCM
    {
        uint32_t value = MCM0_CACHE_REG;
        if (value & MCM_PLACR_DFCS_MASK)
        {
            /* Speculation buffer is off. */
            speculationStatus->instructionOff = true;
            speculationStatus->dataOff = true;
        }
        else
        {
            /* Speculation buffer is on for instruction. */
            if (!(value & MCM_PLACR_EFDS_MASK))
            {
                /* Speculation buffer is off for data. */
                speculationStatus->dataOff = true;
            }
        }
    }
#elif FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC
    {
        uint32_t value = FMC_CACHE_REG;
        if (!(value & FMC_CACHE_B0DPE_MASK))
        {
            /* Do not prefetch in response to data references. */
            speculationStatus->dataOff = true;
        }
        if (!(value & FMC_CACHE_B0IPE_MASK))
        {
            /* Do not prefetch in response to instruction fetches. */
            speculationStatus->instructionOff = true;
        }
    }
#elif FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM
    {
        uint32_t value = MSCM_OCMDR0_REG;
        if (value & MSCM_OCMDR_OCMC1_DFCS_MASK)
        {
            /* Speculation buffer is off. */
            speculationStatus->instructionOff = true;
            speculationStatus->dataOff = true;
        }
        else
        {
            /* Speculation buffer is on for instruction. */
            if (value & MSCM_OCMDR_OCMC1_DFDS_MASK)
            {
                /* Speculation buffer is off for data. */
                speculationStatus->dataOff = true;
            }
        }
    }
#endif

    return kStatus_FTFx_Success;
}

#if FTFx_DRIVER_IS_FLASH_RESIDENT && FLASH_IS_CACHE_INVALIDATION_AVAILABLE
/*! @brief Copy PIC of ftfx_common_bit_operation() to RAM */
static void ftfx_copy_common_bit_operation_to_ram(uint32_t *ftfxCommonBitOperation)
{
    assert(sizeof(s_ftfxCommonBitOperationFunctionCode) <= (kFTFx_CACHE_RamFuncMaxSizeInWords * 4));

    memcpy(ftfxCommonBitOperation, s_ftfxCommonBitOperationFunctionCode,
           sizeof(s_ftfxCommonBitOperationFunctionCode));
}
#endif /* FTFx_DRIVER_IS_FLASH_RESIDENT && FLASH_IS_CACHE_INVALIDATION_AVAILABLE */

#if FLASH_CACHE_IS_CONTROLLED_BY_MCM
/*! @brief Performs the cache clear to the flash by MCM.*/
void mcm_flash_cache_clear(ftfx_cache_config_t *config)
{
    FTFx_REG32_ACCESS_TYPE regBase;

#if defined(MCM0_CACHE_REG)
    regBase = (FTFx_REG32_ACCESS_TYPE)&MCM0_CACHE_REG;
#elif defined(MCM1_CACHE_REG)
    regBase = (FTFx_REG32_ACCESS_TYPE)&MCM1_CACHE_REG;
#endif

#if FTFx_DRIVER_IS_FLASH_RESIDENT
    /* Since the value of ARM function pointer is always odd, but the real start address
     * of function memory should be even, that's why +1 operation exist. */
    callftfxCommonBitOperation_t callftfxCommonBitOperation = (callftfxCommonBitOperation_t)((uint32_t)config->comBitOperFuncAddr + 1);
    callftfxCommonBitOperation(regBase, MCM_CACHE_CLEAR_MASK, MCM_CACHE_CLEAR_SHIFT, 1U);
#else  /* !FTFx_DRIVER_IS_FLASH_RESIDENT */
    *regBase |= MCM_CACHE_CLEAR_MASK;

    /* Memory barriers for good measure.
     * All Cache, Branch predictor and TLB maintenance operations before this instruction complete */
    __ISB();
    __DSB();
#endif /* FTFx_DRIVER_IS_FLASH_RESIDENT */
}
#endif /* FLASH_CACHE_IS_CONTROLLED_BY_MCM */

#if FLASH_CACHE_IS_CONTROLLED_BY_MSCM
/*! @brief Performs the cache clear to the flash by MSCM.*/
void mscm_flash_cache_clear(ftfx_cache_config_t *config)
{
    uint8_t setValue = 0x1U;

/* The OCMDR[0] is always used to cache main Pflash*/
/* For device with FlexNVM support, the OCMDR[1] is used to cache Dflash.
 * For device with secondary flash support, the OCMDR[1] is used to cache secondary Pflash. */
#if FTFx_DRIVER_IS_FLASH_RESIDENT
    /* Since the value of ARM function pointer is always odd, but the real start address
     * of function memory should be even, that's why +1 operation exist. */
    callftfxCommonBitOperation_t callftfxCommonBitOperation = (callftfxCommonBitOperation_t)((uint32_t)config->comBitOperFuncAddr + 1);
    switch (config->flashMemoryIndex)
    {
        case kFLASH_MemoryIndexSecondaryFlash:
            callftfxCommonBitOperation((FTFx_REG32_ACCESS_TYPE)&MSCM_OCMDR1_REG, MSCM_CACHE_CLEAR_MASK,
                                        MSCM_CACHE_CLEAR_SHIFT, setValue);
            break;
        case kFLASH_MemoryIndexPrimaryFlash:
        default:
            callftfxCommonBitOperation((FTFx_REG32_ACCESS_TYPE)&MSCM_OCMDR0_REG, MSCM_CACHE_CLEAR_MASK,
                                        MSCM_CACHE_CLEAR_SHIFT, setValue);
            break;
    }
#else /* !FTFx_DRIVER_IS_FLASH_RESIDENT */
    switch (config->flashMemoryIndex)
    {
        case kFLASH_MemoryIndexSecondaryFlash:
            MSCM_OCMDR1_REG = (MSCM_OCMDR1_REG & (~MSCM_CACHE_CLEAR_MASK)) | MSCM_CACHE_CLEAR(setValue);
            /* Each cahce clear instaruction should be followed by below code*/
            __ISB();
            __DSB();
            break;
        case kFLASH_MemoryIndexPrimaryFlash:
        default:
            MSCM_OCMDR0_REG = (MSCM_OCMDR0_REG & (~MSCM_CACHE_CLEAR_MASK)) | MSCM_CACHE_CLEAR(setValue);
            /* Memory barriers for good measure.
             * All Cache, Branch predictor and TLB maintenance operations before this instruction complete */
            __ISB();
            __DSB();
            break;
    }
#endif /* FTFx_DRIVER_IS_FLASH_RESIDENT */
}
#endif /* FLASH_CACHE_IS_CONTROLLED_BY_MSCM */

#if FLASH_CACHE_IS_CONTROLLED_BY_FMC
/*! @brief Performs the cache clear to the flash by FMC.*/
void fmc_flash_cache_clear(ftfx_cache_config_t *config)
{
#if FTFx_DRIVER_IS_FLASH_RESIDENT
    /* Since the value of ARM function pointer is always odd, but the real start address
     * of function memory should be even, that's why +1 operation exist. */
    callftfxCommonBitOperation_t callftfxCommonBitOperation = (callftfxCommonBitOperation_t)((uint32_t)config->comBitOperFuncAddr + 1);
    callftfxCommonBitOperation((FTFx_REG32_ACCESS_TYPE)&FMC_CACHE_REG, FMC_CACHE_CLEAR_MASK, FMC_CACHE_CLEAR_SHIFT, 0xFU);
#else /* !FTFx_DRIVER_IS_FLASH_RESIDENT */
    FMC_CACHE_REG = (FMC_CACHE_REG & (~FMC_CACHE_CLEAR_MASK)) | FMC_CACHE_CLEAR(~0);
    /* Memory barriers for good measure.
     * All Cache, Branch predictor and TLB maintenance operations before this instruction complete */
    __ISB();
    __DSB();
#endif /* FTFx_DRIVER_IS_FLASH_RESIDENT */
}
#endif /* FLASH_CACHE_IS_CONTROLLED_BY_FMC */

#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM
/*! @brief Performs the prefetch speculation buffer clear to the flash by MSCM.*/
void mscm_flash_prefetch_speculation_enable(ftfx_cache_config_t *config, bool enable)
{
    uint8_t setValue;
    if (enable)
    {
        setValue = 0x0U;
    }
    else
    {
        setValue = 0x3U;
    }

/* The OCMDR[0] is always used to prefetch main Pflash*/
/* For device with FlexNVM support, the OCMDR[1] is used to prefetch Dflash.
 * For device with secondary flash support, the OCMDR[1] is used to prefetch secondary Pflash. */
#if FTFx_DRIVER_IS_FLASH_RESIDENT
    /* Since the value of ARM function pointer is always odd, but the real start address
     * of function memory should be even, that's why +1 operation exist. */
    callftfxCommonBitOperation_t callftfxCommonBitOperation = (callftfxCommonBitOperation_t)((uint32_t)config->comBitOperFuncAddr + 1);
    switch (config->flashMemoryIndex)
    {
        case 1:
            callftfxCommonBitOperation((FTFx_REG32_ACCESS_TYPE)&MSCM_OCMDR1_REG, MSCM_SPECULATION_SET_MASK,
                                        MSCM_SPECULATION_SET_SHIFT, setValue);
            break;
        case 0:
        default:
            callftfxCommonBitOperation((FTFx_REG32_ACCESS_TYPE)&MSCM_OCMDR0_REG, MSCM_SPECULATION_SET_MASK,
                                        MSCM_SPECULATION_SET_SHIFT, setValue);
            break;
     }
#else /* !FTFx_DRIVER_IS_FLASH_RESIDENT */
    switch (config->flashMemoryIndex)
    {
        case kFLASH_MemoryIndexSecondaryFlash:
            MSCM_OCMDR1_REG = (MSCM_OCMDR1_REG & (~MSCM_SPECULATION_SET_MASK)) | MSCM_SPECULATION_SET(setValue);
            /* Each cahce clear instaruction should be followed by below code*/
            __ISB();
            __DSB();
            break;
        case kFLASH_MemoryIndexPrimaryFlash:
        default:
            MSCM_OCMDR0_REG = (MSCM_OCMDR0_REG & (~MSCM_SPECULATION_SET_MASK)) | MSCM_SPECULATION_SET(setValue);
            /* Memory barriers for good measure.
             * All Cache, Branch predictor and TLB maintenance operations before this instruction complete */
            __ISB();
            __DSB();
            break;
    }
#endif /* FTFx_DRIVER_IS_FLASH_RESIDENT */
}
#endif /* FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_MSCM */

#if FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC
/*! @brief Performs the prefetch speculation buffer clear to the flash by FMC.*/
void fmc_flash_prefetch_speculation_clear(ftfx_cache_config_t *config)
{
#if FTFx_DRIVER_IS_FLASH_RESIDENT
    /* Since the value of ARM function pointer is always odd, but the real start address
     * of function memory should be even, that's why +1 operation exist. */
    callftfxCommonBitOperation_t callftfxCommonBitOperation = (callftfxCommonBitOperation_t)((uint32_t)config->comBitOperFuncAddr + 1);
    callftfxCommonBitOperation((FTFx_REG32_ACCESS_TYPE)&FMC_SPECULATION_INVALIDATE_REG, FMC_SPECULATION_INVALIDATE_MASK, FMC_SPECULATION_INVALIDATE_SHIFT, 1U);
#else /* !FTFx_DRIVER_IS_FLASH_RESIDENT */
     FMC_SPECULATION_INVALIDATE_REG |= FMC_SPECULATION_INVALIDATE_MASK;
     /* Memory barriers for good measure.
      * All Cache, Branch predictor and TLB maintenance operations before this instruction complete */
     __ISB();
     __DSB();
#endif /* FTFx_DRIVER_IS_FLASH_RESIDENT */
}
#endif /* FLASH_PREFETCH_SPECULATION_IS_CONTROLLED_BY_FMC */


