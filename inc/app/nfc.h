#pragma once

#define PN532_CONFIG_TWI_INSTANCE 1
#define PN532_CHIP_RESET          8
#define PN532_IRQ                 9
#define PN532_CONFIG_SCL          10
#define PN532_CONFIG_SDA          11
#define PN532_RESET               20 // this is a pn532 reset indicator pin

#define TAG_DATA_BUFFER_SIZE 512

void nfc_initialize(void);
void nfc_scan(void);