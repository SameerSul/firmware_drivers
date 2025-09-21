/*******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/


 // qspitestsequence.c: QSPI test application with proper initialization
 

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "pld_qspi.h"
#include "xparameters.h"
#include "xqspips.h"
#include <string.h>

// Define buffer sizes and test parameters
#define BUFFER_SIZE 256
#define TEST_ADDRESS 0x100000 // Starting address for test
// #define TEST_ADDRESS 0x000000  // CHECK test at 0x000000 instead of 0x100000 to see if data is maybe at the beginning
#define TEST_PATTERN_START 0xAA // Starting value for test pattern

// QSPI Flash commands
#define QUAD_READ_CMD		0x6B
#define READ_ID_CMD			0x9F
#define WRITE_ENABLE_CMD	0x06

// Buffer offsets
#define COMMAND_OFFSET		0
#define ADDRESS_1_OFFSET	1
#define ADDRESS_2_OFFSET	2
#define ADDRESS_3_OFFSET	3
#define DATA_OFFSET			4
#define DUMMY_OFFSET		4
#define DUMMY_SIZE			1
#define OVERHEAD_SIZE		4
#define RD_ID_SIZE			4

// Flash identification (global variables)
u32 QspiFlashSize = 0;
u32 QspiFlashMake = 0;

// Flash manufacturer IDs
#define MICRON_ID           0x20
#define SPANSION_ID         0x01
#define WINBOND_ID          0xEF
#define MACRONIX_ID         0xC2
#define ISSI_ID             0x9D

// Flash size definitions
#define FLASH_SIZE_ID_8M    0x13
#define FLASH_SIZE_ID_16M   0x14
#define FLASH_SIZE_ID_32M   0x15
#define FLASH_SIZE_ID_64M   0x16
#define FLASH_SIZE_ID_128M  0x17
#define FLASH_SIZE_ID_256M  0x18
#define FLASH_SIZE_ID_512M  0x19
#define FLASH_SIZE_ID_1G    0x20
#define FLASH_SIZE_ID_2G    0x21

#define FLASH_SIZE_8M       0x100000      // 8Mbit = 1MB
#define FLASH_SIZE_16M      0x200000      // 16Mbit = 2MB
#define FLASH_SIZE_32M      0x400000      // 32Mbit = 4MB
#define FLASH_SIZE_64M      0x800000      // 64Mbit = 8MB
#define FLASH_SIZE_128M     0x1000000     // 128Mbit = 16MB
#define FLASH_SIZE_256M     0x2000000     // 256Mbit = 32MB
#define FLASH_SIZE_512M     0x4000000     // 512Mbit = 64MB
#define FLASH_SIZE_1G       0x8000000     // 1Gbit = 128MB
#define FLASH_SIZE_2G       0x10000000    // 2Gbit = 256MB

/**
 * Function to read flash ID and determine flash characteristics
 * Based on the reference implementation
 */
u32 FlashReadID_TEMP(PLD_QSPI_t *QspiInstancePtr)
{
    u32 Status;
    u8 WriteBuffer[RD_ID_SIZE];
    u8 ReadBuffer[RD_ID_SIZE];

    // Prepare READ ID command
    WriteBuffer[COMMAND_OFFSET]   = READ_ID_CMD;
    WriteBuffer[ADDRESS_1_OFFSET] = 0x00;  // 3 dummy bytes
    WriteBuffer[ADDRESS_2_OFFSET] = 0x00;
    WriteBuffer[ADDRESS_3_OFFSET] = 0x00;

    // Execute polled transfer to read ID
    Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, ReadBuffer, RD_ID_SIZE);

    // CHECK TO SEE IF WE'RE ACTUALLY TALKING TO THE FLASH CHIP -> WE SHOULD SEE READID CMD SENT (0x9F), if we see 0xFF after, no device responding, 0x00 means bus pulled floating/low
    xil_printf("Raw ID buffer dump: ");
    for (int j = 0; j < RD_ID_SIZE; j++) {
        xil_printf("0x%02x ", ReadBuffer[j]);
    }
    xil_printf("\r\n");

    if (Status != XST_SUCCESS) {
        xil_printf("Failed to read flash ID\r\n");
        return XST_FAILURE;
    }

    xil_printf("Flash ID: 0x%02x 0x%02x 0x%02x\r\n", 
               ReadBuffer[1], ReadBuffer[2], ReadBuffer[3]);

    // Determine flash manufacturer
    switch (ReadBuffer[1]) {
        case MICRON_ID:
            QspiFlashMake = MICRON_ID;
            xil_printf("Flash Manufacturer: MICRON ");
            break;
        case SPANSION_ID:
            QspiFlashMake = SPANSION_ID;
            xil_printf("Flash Manufacturer: SPANSION ");
            break;
        case WINBOND_ID:
            QspiFlashMake = WINBOND_ID;
            xil_printf("Flash Manufacturer: WINBOND ");
            break;
        case MACRONIX_ID:
            QspiFlashMake = MACRONIX_ID;
            xil_printf("Flash Manufacturer: MACRONIX ");
            break;
        case ISSI_ID:
            QspiFlashMake = ISSI_ID;
            xil_printf("Flash Manufacturer: ISSI ");
            break;
        default:
            QspiFlashMake = ReadBuffer[1];
            xil_printf("Flash Manufacturer: UNKNOWN (0x%02x) ", ReadBuffer[1]);
            break;
    }

    // Deduce flash size
    switch (ReadBuffer[3]) {
        case FLASH_SIZE_ID_8M:
            QspiFlashSize = FLASH_SIZE_8M;
            xil_printf("8M Bits\r\n");
            break;
        case FLASH_SIZE_ID_16M:
            QspiFlashSize = FLASH_SIZE_16M;
            xil_printf("16M Bits\r\n");
            break;
        case FLASH_SIZE_ID_32M:
            QspiFlashSize = FLASH_SIZE_32M;
            xil_printf("32M Bits\r\n");
            break;
        case FLASH_SIZE_ID_64M:
            QspiFlashSize = FLASH_SIZE_64M;
            xil_printf("64M Bits\r\n");
            break;
        case FLASH_SIZE_ID_128M:
            QspiFlashSize = FLASH_SIZE_128M;
            xil_printf("128M Bits\r\n");
            break;
        case FLASH_SIZE_ID_256M:
            QspiFlashSize = FLASH_SIZE_256M;
            xil_printf("256M Bits\r\n");
            break;
        case FLASH_SIZE_ID_512M:
            QspiFlashSize = FLASH_SIZE_512M;
            xil_printf("512M Bits\r\n");
            break;
        case FLASH_SIZE_ID_1G:
            QspiFlashSize = FLASH_SIZE_1G;
            xil_printf("1G Bits\r\n");
            break;
        case FLASH_SIZE_ID_2G:
            QspiFlashSize = FLASH_SIZE_2G;
            xil_printf("2G Bits\r\n");
            break;
        default:
            QspiFlashSize = FLASH_SIZE_16M; // Default assumption
            xil_printf("Unknown size (0x%02x), assuming 16M Bits\r\n", ReadBuffer[3]);
            break;
    }

    return XST_SUCCESS;
}

/**
 * Function to perform QSPI flash read using polled transfer
 * Based on the reference implementation
 */
u32 FlashPolledRead(PLD_QSPI_t *QspiInstancePtr, u32 Address, u32 ByteCount, u8 *ReadBuffer)
{
    u32 Status;
    u8 WriteBuffer[OVERHEAD_SIZE];

    // Prepare the read command with address
    WriteBuffer[COMMAND_OFFSET]   = QUAD_READ_CMD;  // Use quad read for better performance
    WriteBuffer[ADDRESS_1_OFFSET] = (u8)((Address & 0xFF0000) >> 16);
    WriteBuffer[ADDRESS_2_OFFSET] = (u8)((Address & 0xFF00) >> 8);
    WriteBuffer[ADDRESS_3_OFFSET] = (u8)(Address & 0xFF);

    // Add dummy byte count for quad read
    ByteCount += DUMMY_SIZE;

    // Execute polled transfer
    Status = XQspiPs_PolledTransfer(QspiInstancePtr, WriteBuffer, ReadBuffer,
                                     ByteCount + OVERHEAD_SIZE);
    
    // CHECK WHERE EXACTLY THE REAL DATA STARTS IN OUR BUFFER + MAKE SURE FLASH CONTAINS DATA, for normal read we should see 0x6B 0x00 * 3 0xFF [data].., for empty flash we shoould see 0x6B 0x00*3, 0xFF ...
    xil_printf("Raw read buffer dump (first 10 bytes): ");
    for (int j = 0; j < 10; j++) {
        xil_printf("0x%02x ", ReadBuffer[j]);
    }
    xil_printf("\r\n");

    if (Status != XST_SUCCESS) {
        xil_printf("Flash read failed at address 0x%08x\r\n", Address);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

/**
 * Test QSPI peripheral functionality without requiring flash
 * This tests if the QSPI driver and peripheral are working correctly
 */
u32 TestQSPIPeripheral(PLD_QSPI_t *QspiInstancePtr)
{
    u32 Status;
    u8 WriteBuffer[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22};
    u8 ReadBuffer[8] = {0};

    xil_printf("Testing QSPI peripheral (no flash needed)...\r\n");

    // Test: Can we send data through QSPI?
    Status = PLD_QSPI_Transfer(QspiInstancePtr, WriteBuffer, ReadBuffer, 4);
    
    xil_printf("QSPI Transfer Test:\r\n");
    xil_printf("Sent:     ");
    for(int i = 0; i < 4; i++) {
        xil_printf("0x%02x ", WriteBuffer[i]);
    }
    xil_printf("\r\nReceived: ");
    for(int i = 0; i < 4; i++) {
        xil_printf("0x%02x ", ReadBuffer[i]);
    }
    xil_printf("\r\n");

    if (Status == XST_SUCCESS) {
        xil_printf("QSPI peripheral transfer: SUCCESS\r\n");
        
        // Analyze what we received
        if (ReadBuffer[0] == 0xAA) {
            xil_printf("-> First byte matches sent data (loopback or echo)\r\n");
        } else if (ReadBuffer[0] == 0xFF) {
            xil_printf("-> Received 0xFF (normal when no device connected - bus pulled high)\r\n");
        } else if (ReadBuffer[0] == 0x00) {
            xil_printf("-> Received 0x00 (normal when no device connected - bus pulled low)\r\n");
        } else {
            xil_printf("-> Unexpected data pattern: 0x%02x\r\n", ReadBuffer[0]);
        }
        
        return XST_SUCCESS;
    } else {
        xil_printf("QSPI peripheral transfer: FAILED (Status: %d)\r\n", Status);
        return XST_FAILURE;
    }
}

/**
 * Test QSPI with flash-like commands (but don't expect valid responses)
 * This tests command formatting and transmission
 */
u32 TestQSPICommands(PLD_QSPI_t *QspiInstancePtr)
{
    u32 Status;
    u8 WriteBuffer[8];
    u8 ReadBuffer[8];

    xil_printf("\r\nTesting QSPI command transmission...\r\n");

    // Test READ ID command (even without flash)
    WriteBuffer[0] = READ_ID_CMD;  // 0x9F
    WriteBuffer[1] = 0x00;
    WriteBuffer[2] = 0x00;
    WriteBuffer[3] = 0x00;

    Status = PLD_QSPI_Transfer(QspiInstancePtr, WriteBuffer, ReadBuffer, 4);
    
    xil_printf("READ_ID Command Test:\r\n");
    xil_printf("Sent READ_ID (0x9F): ");
    for(int i = 0; i < 4; i++) {
        xil_printf("0x%02x ", WriteBuffer[i]);
    }
    xil_printf("\r\nReceived:             ");
    for(int i = 0; i < 4; i++) {
        xil_printf("0x%02x ", ReadBuffer[i]);
    }
    xil_printf("\r\n");

    if (Status == XST_SUCCESS) {
        xil_printf("Command transmission: SUCCESS\r\n");
        
        // Analyze response
        if (ReadBuffer[0] == 0x9F) {
            xil_printf("-> Command echoed back (expected with no flash)\r\n");
        } else if (ReadBuffer[0] == 0xFF || ReadBuffer[0] == 0x00) {
            xil_printf("-> No echo (QSPI configured in different mode)\r\n");
        }
        
        return XST_SUCCESS;
    } else {
        xil_printf("Command transmission: FAILED (Status: %d)\r\n", Status);
        return XST_FAILURE;
    }
}

/**
 * Test QSPI options and configuration
 */
u32 TestQSPIConfiguration(PLD_QSPI_t *QspiInstancePtr)
{
    u32 options;
    
    xil_printf("\r\nTesting QSPI configuration...\r\n");
    
    // Read current options
    options = XQspiPs_GetOptions(QspiInstancePtr);
    xil_printf("Current QSPI Options: 0x%08x\r\n", options);
    
    // Check specific options
    if (options & XQSPIPS_FORCE_SSELECT_OPTION) {
        xil_printf("-> Manual chip select: ENABLED\r\n");
    } else {
        xil_printf("-> Manual chip select: DISABLED\r\n");
    }
    
    if (options & XQSPIPS_HOLD_B_DRIVE_OPTION) {
        xil_printf("-> HOLD_B drive: ENABLED\r\n");
    } else {
        xil_printf("-> HOLD_B drive: DISABLED\r\n");
    }
    
    if (options & XQSPIPS_MANUAL_START_OPTION) {
        xil_printf("-> Manual start: ENABLED\r\n");
    } else {
        xil_printf("-> Manual start: DISABLED\r\n");
    }
    
    return XST_SUCCESS;
}

int main()
{
    int Status;
    PLD_QSPI_t QspiInstance;        // QSPI driver instance
    u8 ReadBuffer[BUFFER_SIZE + OVERHEAD_SIZE + DUMMY_SIZE];  // Read buffer (with extra space for overhead)
    u32 i;

    init_platform();

    xil_printf("QSPI Test Application\r\n");
    xil_printf("====================\r\n");

    // Step 1: Initialize the QSPI driver
    xil_printf("Initializing QSPI driver...\r\n");
#ifndef SDT
    Status = PLD_QSPI_Open(&QspiInstance, XPAR_XQSPIPS_0_DEVICE_ID);
#else
    Status = PLD_QSPI_Open(&QspiInstance, XPAR_XQSPIPS_0_BASEADDR);
#endif

    if (Status != XST_SUCCESS) {
        xil_printf("QSPI driver initialization failed\r\n");
        cleanup_platform();
        return XST_FAILURE;
    }
    xil_printf("QSPI driver initialized successfully\r\n");

    // Step 2: Configure QSPI settings
    xil_printf("Configuring QSPI settings...\r\n");
    
    // Set clock prescaler
    Status = PLD_QSPI_SetClockPrescalar(&QspiInstance, XQSPIPS_CLK_PRESCALE_8);
    if (Status != XST_SUCCESS) {
        xil_printf("Failed to set clock prescaler\r\n");
        PLD_QSPI_Close(&QspiInstance);
        cleanup_platform();
        return XST_FAILURE;
    }

    // Set manual chip select and drive HOLD_B high (following reference pattern)
    Status = PLD_QSPI_SetOptionsManually(&QspiInstance, 
                                        XQSPIPS_FORCE_SSELECT_OPTION | 
                                        XQSPIPS_HOLD_B_DRIVE_OPTION);

    if (Status != XST_SUCCESS) {
        xil_printf("Failed to set QSPI options\r\n");
        PLD_QSPI_Close(&QspiInstance);
        cleanup_platform();
        return XST_FAILURE;
    }

    // Set slave select (chip select)
    Status = XQspiPs_SetSlaveSelect(&QspiInstance);
    if (Status != XST_SUCCESS) {
        xil_printf("Failed to set slave select\r\n");
        PLD_QSPI_Close(&QspiInstance);
        cleanup_platform();
        return XST_FAILURE;
    }

    xil_printf("QSPI configuration complete\r\n");

    // === PERIPHERAL TESTS (No Flash Required) ===
    xil_printf("\r\n=== PERIPHERAL TESTS (No Flash Required) ===\r\n");
    
    Status = TestQSPIPeripheral(&QspiInstance);
    if (Status != XST_SUCCESS) {
        xil_printf("WARNING: QSPI peripheral test failed - driver may have issues\r\n");
    }

    Status = TestQSPICommands(&QspiInstance);
    if (Status != XST_SUCCESS) {
        xil_printf("WARNING: QSPI command test failed - transfer function may have issues\r\n");
    }

    TestQSPIConfiguration(&QspiInstance);
    
    xil_printf("\r\n=== FLASH TESTS (Requires Flash Device) ===\r\n");

    // Step 3: Read flash ID and characteristics
    xil_printf("Reading flash identification...\r\n");
    Status = FlashReadID_TEMP(&QspiInstance);
    if (Status != XST_SUCCESS) {
        xil_printf("Failed to read flash ID\r\n");
        PLD_QSPI_Close(&QspiInstance);
        cleanup_platform();
        return XST_FAILURE;
    }

    // Clear read buffer
    memset(ReadBuffer, 0, sizeof(ReadBuffer));

    // Step 4: Read data from flash (assuming data exists at test address)
    xil_printf("Reading data from flash at address 0x%08x...\r\n", TEST_ADDRESS);
    Status = FlashPolledRead(&QspiInstance, TEST_ADDRESS, BUFFER_SIZE, ReadBuffer);
    
    if (Status != XST_SUCCESS) {
        xil_printf("Flash read operation failed\r\n");
        PLD_QSPI_Close(&QspiInstance);
        cleanup_platform();
        return XST_FAILURE;
    }
    xil_printf("Flash read completed successfully\r\n");

    // Step 5: Display read data (first few bytes for verification)
    xil_printf("First 16 bytes read from flash:\r\n");
    for (i = 0; i < 16; i++) {
        xil_printf("0x%02x ", ReadBuffer[DATA_OFFSET + DUMMY_SIZE + i]);
        if ((i + 1) % 8 == 0) {
            xil_printf("\r\n");
        }
    }
    xil_printf("\r\n");
    
    // Step 6: Optional - Test pattern verification (will likely fail if pattern not written)
    xil_printf("Testing pattern verification (may fail if pattern not in flash)...\r\n");
    u32 mismatch_count = 0;
    
    for (i = 0; i < 16; i++) {  // Check only first 16 bytes
        u8 expected = (u8)(TEST_PATTERN_START + i);
        u8 actual = ReadBuffer[DATA_OFFSET + DUMMY_SIZE + i];
        if (actual != expected) {
            if (mismatch_count < 3) {  // Show only first 3 mismatches
                xil_printf("Mismatch at byte %lu: Expected 0x%02x, Got 0x%02x\r\n", 
                          i, expected, actual);
            }
            mismatch_count++;
        }
    }

    if (mismatch_count == 0) {
        xil_printf("Pattern verification successful!\r\n");
    } else {
        xil_printf("Pattern mismatches found (%lu total). This is normal if test pattern wasn't written.\r\n", mismatch_count);
    }
    
    xil_printf("\r\nTest Summary:\r\n");
    xil_printf("- QSPI initialization: SUCCESS\r\n");
    xil_printf("- Flash ID read: SUCCESS\r\n");
    xil_printf("- Flash data read: SUCCESS\r\n");
    xil_printf("- Flash size: %lu bytes\r\n", QspiFlashSize);
    xil_printf("- Flash manufacturer ID: 0x%02x\r\n", QspiFlashMake);

    // Step 7: Clean up - TESTING NOW
    PLD_QSPI_Close(&QspiInstance);
    cleanup_platform();

    xil_printf("QSPI test application completed successfully\r\n");
    return XST_SUCCESS;
}