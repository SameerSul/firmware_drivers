/*******************************************************************************
*   McMaster PRESET (www.mcmasterneudose.ca)
*
*   Data Acquisition Module - DAM
*   Flight Firmware
*
*   @file       pld_qspi.c
*   @desc       QSPI low level driver implementation (cleaned up)
*   @author     Graham N. Power
*               Sameer Suleman
*   @date       March 3, 2025
*
*	<pre>
*
*   Revision History:
*
*	Ver   	Who    	Date   		Changes
*	----- 	---- 	-------- 	-------------------------------------------------------
*	1.0.0	sam		2025-02-24	Initial commit to dev branch
*   1.0.1   sam     2025-03-17  Finalized changes based on Graham's feedback
*   1.1.0   sam     2025-09-27  Cleaned up to only include functions used by helloworld.c
*	</pre>
*******************************************************************************/

/*******************************************************************************
*   Includes
*******************************************************************************/
#include "pld_qspi.h"

/* STD Includes */
#include <string.h>

/* Xilinx Includes */
#include "xparameters.h"
#include "xqspips.h"
#include "xil_printf.h"
#include <xil_types.h>
#include <xstatus.h>

/*******************************************************************************
*   Functions
*******************************************************************************/

/**
 * Initialize the QSPI driver
 * Note: This function uses the SDT/non-SDT initialization pattern for compatibility
 */
#ifndef SDT
XStatus PLD_QSPI_Open(PLD_QSPI_t *InstancePtr, uint16_t SpiDevId)
#else
XStatus PLD_QSPI_Open(PLD_QSPI_t *InstancePtr, UINTPTR SpiBaseAddr)
#endif
{
    XStatus Status;
    XQspiPs_Config *ConfigPtr;

    // Prevent initializing the same SPI device twice
    if (InstancePtr->IsReady == XIL_COMPONENT_IS_READY) {
        return XST_SUCCESS;
    }

#ifndef SDT
    ConfigPtr = XQspiPs_LookupConfig(SpiDevId);
#else
    ConfigPtr = XQspiPs_LookupConfig(SpiBaseAddr);
#endif
    
    if (ConfigPtr == NULL) {
        return XST_DEVICE_NOT_FOUND;
    }

    Status = XQspiPs_CfgInitialize(InstancePtr, ConfigPtr, ConfigPtr->BaseAddress);
    if (Status != XST_SUCCESS) {
        return Status;
    }

    Status = XQspiPs_SelfTest(InstancePtr);
    if (Status != XST_SUCCESS) {
        return Status;
    }

    XQspiPs_Enable(InstancePtr);
    
    return XST_SUCCESS;
}

/*
 * Note: For future reference, the SDT/non-SDT initialization pattern used above
 * follows this format for functions that need different parameter types:
 * 
 * #ifndef SDT
 * ReturnType FunctionName(ParamType1 param1, u16 DeviceId)
 * #else  
 * ReturnType FunctionName(ParamType1 param1, UINTPTR BaseAddress)
 * #endif
 * 
 * This allows the same function to work with both traditional device ID
 * initialization and newer system device tree (SDT) base address initialization.
 */

/**
 * Set clock prescaler for QSPI
 */
XStatus PLD_QSPI_SetClockPrescalar(PLD_QSPI_t *InstancePtr, uint8_t Prescaler)
{
    return XQspiPs_SetClkPrescaler(InstancePtr, Prescaler);
}

/**
 * Set QSPI options manually
 */
XStatus PLD_QSPI_SetOptionsManually(PLD_QSPI_t *InstancePtr, uint32_t options)
{
    return XQspiPs_SetOptions(InstancePtr, options);
}

/**
 * Close/disable the QSPI driver
 */
XStatus PLD_QSPI_Close(PLD_QSPI_t *InstancePtr)
{
    XQspiPs_Disable(InstancePtr);
    return XST_SUCCESS;
}

/**
 * Perform QSPI transfer (polled mode)
 */
XStatus PLD_QSPI_Transfer(PLD_QSPI_t *InstancePtr, uint8_t *WriteData, uint8_t *ReadData, uint32_t DataLength)
{
    XStatus Status;

    // Check if driver is ready
    if (InstancePtr->IsReady != XIL_COMPONENT_IS_READY) {
        return XST_DEVICE_NOT_FOUND;
    }

    // Check if manual chip select is enabled
    uint32_t ShouldCS = XQspiPs_GetOptions(InstancePtr) & XQSPIPS_FORCE_SSELECT_OPTION;

    if (ShouldCS) {
        Status = XQspiPs_SetSlaveSelect(InstancePtr);
        if (Status != XST_SUCCESS) {
            return Status;
        }
    }

    // Use polled mode operation
    Status = XQspiPs_PolledTransfer(InstancePtr, WriteData, ReadData, DataLength);
    if (Status != XST_SUCCESS) {
        return Status;
    }

    return XST_SUCCESS;
}
