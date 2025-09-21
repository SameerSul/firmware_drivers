## Drivers I've been working on that I can share...

- Completed in Vitis 2024.1, in C++, responsible for Memory Integrity of Satellite
- Won't work unless initialized with a custom Vitis hw platform and with the right dependencies, reach out if you wanna learn more about it

## QSPI Driver Test Functions

**Peripheral Tests (No Flash Required)**
- `TestQSPIPeripheral()` - Validates basic QSPI transfer functionality by sending test data patterns
- `TestQSPICommands()` - Tests flash command transmission (READ_ID) without expecting responses  
- `TestQSPIConfiguration()` - Displays current QSPI configuration settings

**Flash Tests (Requires Hardware)**  
- `FlashReadID_TEMP()` - Identifies connected flash device (manufacturer, capacity)
- `FlashPolledRead()` - Performs quad read operations from flash memory

**Test Sequence**: Peripheral tests run first to validate driver functionality, then flash tests verify hardware communication. Helps isolate driver vs hardware issues through layered diagnostics.

## Last Updated: Sept 21 2025
