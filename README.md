# Payload Data Acquisition Module (DAM) Revision B Firmware
A repository for the C and HDL source code for the DAM REVB

# Setup Instructions
1. Clone the repository
    i. `git clone git@github.com:McMasterNEUDOSE/PRE-DAM-REVB-FW.git`
    ii. `cd PRE-DAM-REVB-FW`
    iii. `git submodule update --init --recursive`
2. Create the Vivado project using HOG
    i. `cd projects/hdl`
    ii. `./Hog/Do CREATE ZedBoard`
3. Open the Vivado project created in `Projects/ZedBoad/ZedBoard.xpr`
4. Create the Zynq block diagram
    i. In the TCL shell run `source <PathToYourRepo>/PRE-DAM-REVB-FW/projects/hdl/sw/zynq7_ip_bd.tcl`
    ii. Right-click the newly created block diagram in the Design Sources explorer and click `Generate Output Products...`
    iii. Right-click the newly created block diagram in the Design Sources explorer and click `Generate HDL Wrapper...`
    iv. Right-click any empty space in the Design Sources explorer and click `Refresh Hierarchy`
5. Enjoy!

# -- PLD QSPI Driver Documentation --

## Overview

The PLD QSPI (Quad Serial Peripheral Interface) driver provides a simplified interface for communicating with QSPI devices on McMaster PRESET's Data Acquisition Module (DAM). This driver is a lightweight wrapper around Xilinx's XQspiPs driver, designed for basic QSPI operations including flash memory communication and peripheral testing.

## Features

- Simple initialization and cleanup
- Clock prescaler configuration  
- Flexible option setting
- Polled transfer operations
- Compatible with both traditional device ID and System Device Tree (SDT) initialization

## Data Types

### PLD_QSPI_t
```c
typedef XQspiPs PLD_QSPI_t;
```
The main driver instance type, which is a direct alias to Xilinx's `XQspiPs` structure.

## Function Reference

### 1. PLD_QSPI_Open()

**Purpose:** Initializes the QSPI driver instance

**Signature:**
```c
#ifndef SDT
XStatus PLD_QSPI_Open(PLD_QSPI_t *InstancePtr, uint16_t SpiDevId);
#else
XStatus PLD_QSPI_Open(PLD_QSPI_t *InstancePtr, UINTPTR SpiBaseAddr);
#endif
```

**Parameters:**
- `InstancePtr`: Pointer to the QSPI driver instance
- `SpiDevId` (non-SDT): Device ID from xparameters.h (e.g., `XPAR_XQSPIPS_0_DEVICE_ID`)
- `SpiBaseAddr` (SDT): Base address of the QSPI controller (e.g., `XPAR_XQSPIPS_0_BASEADDR`)

**Returns:**
- `XST_SUCCESS`: Initialization successful
- `XST_DEVICE_NOT_FOUND`: Configuration lookup failed
- `XST_FAILURE`: Initialization or self-test failed

**Description:**
Performs complete QSPI driver initialization including configuration lookup, driver initialization, self-test, and enabling the peripheral. The function prevents double initialization of the same instance.

**Example Usage:**
```c
PLD_QSPI_t qspi_instance;

#ifndef SDT
Status = PLD_QSPI_Open(&qspi_instance, XPAR_XQSPIPS_0_DEVICE_ID);
#else
Status = PLD_QSPI_Open(&qspi_instance, XPAR_XQSPIPS_0_BASEADDR);
#endif

if (Status != XST_SUCCESS) {
    xil_printf("QSPI initialization failed\n");
    return XST_FAILURE;
}
```

### 2. PLD_QSPI_Close()

**Purpose:** Disables and cleans up the QSPI driver

**Signature:**
```c
XStatus PLD_QSPI_Close(PLD_QSPI_t *InstancePtr);
```

**Parameters:**
- `InstancePtr`: Pointer to the QSPI driver instance

**Returns:**
- `XST_SUCCESS`: Always returns success

**Description:**
Disables the QSPI peripheral. This should be called when finished using the QSPI interface.

**Example Usage:**
```c
PLD_QSPI_Close(&qspi_instance);
```

### 3. PLD_QSPI_SetClockPrescalar()

**Purpose:** Sets the QSPI clock prescaler

**Signature:**
```c
XStatus PLD_QSPI_SetClockPrescalar(PLD_QSPI_t *InstancePtr, uint8_t Prescaler);
```

**Parameters:**
- `InstancePtr`: Pointer to the QSPI driver instance
- `Prescaler`: Clock prescaler value (see Xilinx constants below)

**Returns:**
- `XST_SUCCESS`: Clock prescaler set successfully
- Other values indicate failure

**Common Prescaler Values:**
- `XQSPIPS_CLK_PRESCALE_2`: Divide by 2
- `XQSPIPS_CLK_PRESCALE_4`: Divide by 4  
- `XQSPIPS_CLK_PRESCALE_8`: Divide by 8
- `XQSPIPS_CLK_PRESCALE_16`: Divide by 16
- `XQSPIPS_CLK_PRESCALE_32`: Divide by 32
- `XQSPIPS_CLK_PRESCALE_64`: Divide by 64
- `XQSPIPS_CLK_PRESCALE_128`: Divide by 128
- `XQSPIPS_CLK_PRESCALE_256`: Divide by 256

**Description:**
Configures the QSPI clock frequency by setting a prescaler that divides the reference clock. Lower prescaler values result in higher QSPI clock frequencies.

**Example Usage:**
```c
// Set clock prescaler to divide by 8
Status = PLD_QSPI_SetClockPrescalar(&qspi_instance, XQSPIPS_CLK_PRESCALE_8);
if (Status != XST_SUCCESS) {
    xil_printf("Failed to set clock prescaler\n");
}
```

### 4. PLD_QSPI_SetOptionsManually()

**Purpose:** Sets QSPI configuration options

**Signature:**
```c
XStatus PLD_QSPI_SetOptionsManually(PLD_QSPI_t *InstancePtr, uint32_t options);
```

**Parameters:**
- `InstancePtr`: Pointer to the QSPI driver instance
- `options`: Bitwise OR of option flags

**Returns:**
- `XST_SUCCESS`: Options set successfully
- Other values indicate failure

**Common Option Flags:**
- `XQSPIPS_FORCE_SSELECT_OPTION`: Enable manual chip select control
- `XQSPIPS_MANUAL_START_OPTION`: Enable manual start mode
- `XQSPIPS_HOLD_B_DRIVE_OPTION`: Drive HOLD_B signal high
- `XQSPIPS_CLK_ACTIVE_LOW_OPTION`: Clock active low polarity
- `XQSPIPS_CLK_PHASE_1_OPTION`: Clock phase 1

**Description:**
Configures various QSPI operational modes and signal behaviors. Options can be combined using bitwise OR.

**Example Usage:**
```c
// Enable manual chip select and drive HOLD_B high
uint32_t options = XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION;
Status = PLD_QSPI_SetOptionsManually(&qspi_instance, options);
if (Status != XST_SUCCESS) {
    xil_printf("Failed to set QSPI options\n");
}
```

### 5. PLD_QSPI_Transfer()

**Purpose:** Performs a polled QSPI data transfer

**Signature:**
```c
XStatus PLD_QSPI_Transfer(PLD_QSPI_t *InstancePtr, uint8_t *WriteData, 
                          uint8_t *ReadData, uint32_t DataLength);
```

**Parameters:**
- `InstancePtr`: Pointer to the QSPI driver instance
- `WriteData`: Pointer to data to transmit (can be NULL for read-only)
- `ReadData`: Pointer to buffer for received data (can be NULL for write-only)
- `DataLength`: Number of bytes to transfer

**Returns:**
- `XST_SUCCESS`: Transfer completed successfully
- `XST_DEVICE_NOT_FOUND`: Driver not ready
- Other values indicate transfer failure

**Description:**
Performs a synchronous QSPI transfer operation. The function will automatically assert chip select if manual chip select mode is enabled. This is a blocking operation that waits for completion.

**Example Usage:**
```c
uint8_t write_data[4] = {0x9F, 0x00, 0x00, 0x00}; // READ ID command
uint8_t read_data[4];

// Perform transfer
Status = PLD_QSPI_Transfer(&qspi_instance, write_data, read_data, 4);
if (Status == XST_SUCCESS) {
    xil_printf("Transfer successful\n");
    // Process read_data
} else {
    xil_printf("Transfer failed\n");
}
```

## Usage Examples

### Basic Initialization and Test

```c
#include "pld_qspi.h"
#include "xparameters.h"
#include "xil_printf.h"

int main() {
    PLD_QSPI_t qspi_instance;
    XStatus status;
    
    // Initialize QSPI driver
    #ifndef SDT
    status = PLD_QSPI_Open(&qspi_instance, XPAR_XQSPIPS_0_DEVICE_ID);
    #else
    status = PLD_QSPI_Open(&qspi_instance, XPAR_XQSPIPS_0_BASEADDR);
    #endif
    
    if (status != XST_SUCCESS) {
        xil_printf("QSPI initialization failed\n");
        return -1;
    }
    
    // Configure clock prescaler
    status = PLD_QSPI_SetClockPrescalar(&qspi_instance, XQSPIPS_CLK_PRESCALE_8);
    if (status != XST_SUCCESS) {
        xil_printf("Clock prescaler configuration failed\n");
        goto cleanup;
    }
    
    // Set options for manual chip select
    uint32_t options = XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION;
    status = PLD_QSPI_SetOptionsManually(&qspi_instance, options);
    if (status != XST_SUCCESS) {
        xil_printf("Options configuration failed\n");
        goto cleanup;
    }
    
    // Set slave select
    XQspiPs_SetSlaveSelect(&qspi_instance);
    
    xil_printf("QSPI initialization complete\n");
    
cleanup:
    PLD_QSPI_Close(&qspi_instance);
    return (status == XST_SUCCESS) ? 0 : -1;
}
```

### Flash ID Reading Example

```c
int read_flash_id(PLD_QSPI_t *qspi) {
    uint8_t write_buffer[4] = {0x9F, 0x00, 0x00, 0x00}; // READ ID command
    uint8_t read_buffer[4];
    XStatus status;
    
    status = PLD_QSPI_Transfer(qspi, write_buffer, read_buffer, 4);
    if (status != XST_SUCCESS) {
        xil_printf("Flash ID read failed\n");
        return -1;
    }
    
    xil_printf("Flash ID: 0x%02X 0x%02X 0x%02X\n", 
               read_buffer[1], read_buffer[2], read_buffer[3]);
    return 0;
}
```

## Error Handling

All functions return an `XStatus` value:
- `XST_SUCCESS` (0): Operation completed successfully
- Non-zero values: Various error conditions

Always check return values and handle errors appropriately:

```c
XStatus status = PLD_QSPI_Open(&instance, device_id);
if (status != XST_SUCCESS) {
    // Handle initialization failure
    xil_printf("QSPI init failed with status: %d\n", status);
    return status;
}
```

## Dependencies

- Xilinx XQspiPs driver
- xstatus.h for status codes
- xparameters.h for device parameters
- Standard integer types (stdint.h)

## Notes

- This driver uses polled mode transfers only (no interrupt support)
- Manual chip select management is recommended for reliable operation
- The driver automatically prevents double initialization
- Always call `PLD_QSPI_Close()` when finished to properly clean up resources
- Clock prescaler should be set based on your system clock frequency and required QSPI speed
- The SDT/non-SDT compilation pattern allows the same code to work with different Xilinx toolchain versions

## Troubleshooting

**Initialization Fails:**
- Verify correct device ID or base address
- Check that QSPI peripheral is included in hardware design
- Ensure proper clock configuration

**Transfer Fails:**
- Verify driver is properly initialized
- Check that chip select is properly configured
- Ensure correct data buffer sizes
- Verify flash device connections and power supply

**No Response from Flash:**
- Check hardware connections (MISO, MOSI, SCLK, CS)
- Verify flash device power supply
- Try different clock prescaler values
- Check flash device datasheet for correct commands
