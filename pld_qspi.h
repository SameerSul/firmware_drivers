/*******************************************************************************
*   McMaster PRESET (www.mcmasterneudose.ca)
*
*   Data Acquisition Module - DAM
*   Flight Firmware
*
*   @file       pld_qspi.h
*   @desc       Header file to the encoder low level driver
*   @author     Graham N. Power
*               Sameer Suleman
*   @date       Sept 21 2025
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
#define PLD_QSPI_CS_0 0x01
#define PLD_QSPI_CS_1 0x02
#define PLD_QSPI_CS_2 0x04
#define PLD_QSPI_CS_3 0x08

/*******************************************************************************
*   Function Prototypes
*******************************************************************************/

XStatus PLD_QSPI_SetClockPrescalar(PLD_QSPI_t *InstancePtr, uint8_t Prescaler);
XStatus PLD_QSPI_SetClockActiveLowClock(PLD_QSPI_t *InstancePtr);
XStatus PLD_QSPI_SetClockPhaseOne(PLD_QSPI_t *InstancePtr);
XStatus PLD_QSPI_SetCsManualToggle(PLD_QSPI_t *InstancePtr);
XStatus PLD_QSPI_RemoveClockActiveLowClock(PLD_QSPI_t *InstancePtr);
XStatus PLD_QSPI_RemoveClockPhaseOne(PLD_QSPI_t *InstancePtr);
XStatus PLD_QSPI_RemoveCsManualToggle(PLD_QSPI_t *InstancePtr);
XStatus PLD_QSPI_SetOptionsManually(PLD_QSPI_t *InstancePtr, uint32_t options);
XStatus PLD_QSPI_Close(PLD_QSPI_t *InstancePtr);
XStatus PLD_QSPI_Transfer(PLD_QSPI_t *InstancePtr, uint8_t *WriteData, uint8_t *ReadData, uint32_t DataLength);

/* Basic QSPI Functions */

#ifndef SDT
    XStatus PLD_QSPI_Open(PLD_QSPI_t * InstancePtr, uint16_t SpiDevId);
#else
//
    XStatus PLD_QSPI_Open(PLD_QSPI_t * InstancePtr, UINTPTR SpiBaseAddr);
#endif

XStatus PLD_QSPI_Close(PLD_QSPI_t *InstancePtr);
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
#endif /* PLD_QSPI_L_H_ */