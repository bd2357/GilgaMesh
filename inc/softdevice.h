#pragma once

#include <stdbool.h>

#define BOOTLOADER_REGION_START 0x1b000         // this needs to match the FLASH ORIGIN address in the linker script

bool softdevice_initialize(void);
