/*******************************************************************************
*   McMaster PRESET (www.mcmasterneudose.ca)
*
*   Data Acquisition Module - DAM
*   Flight Firmware
*
*   @file       pld_qspi.h
*   @desc       Header file to the QSPI low level driver
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
*   1.1.0   sam     2025-09-27  QSPI works perfectly on board, updating soon
*	</pre>
*
*******************************************************************************/

/*******************************************************************************
*   Prevent circular dependency
*   DO NOT REMOVE
*******************************************************************************/
#ifndef PLD_QSPI
#define PLD_QSPI

/*******************************************************************************
*   Includes
*******************************************************************************/
/* STD Includes */
#include <stdint.h>

/* Xilinx Includes */
#include "xstatus.h"
#include "xqspips.h"

/* FreeAct Includes */

/* NEUDOSE Includes */

/*******************************************************************************
*   Preprocessor Macros
*******************************************************************************/

/*******************************************************************************
*   Datatype Definitions
*******************************************************************************/
typedef XQspiPs PLD_QSPI_t;

/*******************************************************************************
*   Constant Definitions
*******************************************************************************/

/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

/* Basic QSPI Functions - Only functions actually used in helloworld.c */

#ifndef SDT
XStatus PLD_QSPI_Open(PLD_QSPI_t *InstancePtr, uint16_t SpiDevId);
#else
XStatus PLD_QSPI_Open(PLD_QSPI_t *InstancePtr, UINTPTR SpiBaseAddr);
#endif

XStatus PLD_QSPI_Close(PLD_QSPI_t *InstancePtr);

/* Configuration functions */
XStatus PLD_QSPI_SetClockPrescalar(PLD_QSPI_t *InstancePtr, uint8_t Prescaler);
XStatus PLD_QSPI_SetOptionsManually(PLD_QSPI_t *InstancePtr, uint32_t options);

/* Transfer function */
XStatus PLD_QSPI_Transfer(PLD_QSPI_t *InstancePtr, uint8_t *WriteData, uint8_t *ReadData, uint32_t DataLength);

/*******************************************************************************
*   Global Variables
*******************************************************************************/

/*******************************************************************************
*   Functions
*******************************************************************************/

/*******************************************************************************
*   Prevent circular dependency
*   DO NOT REMOVE
*******************************************************************************/
#endif /* PLD_QSPI */
