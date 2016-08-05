#pragma once

#include <string.h>

#include <nrf_soc.h>
#include <softdevice_handler.h>
#include <app_util.h>
#include <app_error.h>
#include <ble.h>
#include <ble_gap.h>
#include <ble_hci.h>
//#include <pstorage_platform.h>

#include <gatt.h>
#include <logger.h>
#include <connection.h>
#include <message.h>
#include <error.h>
#include <version.h>

#define MANUFACTURER_ID                    0xC0DE
#define ATTR_MESH_CONNECTION_INTERVAL      MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define ATTR_MESH_CONNECTION_TIMEOUT       MSEC_TO_UNITS(500, UNIT_10_MS)
#define ATTR_MESH_CONNECTION_SCAN_TIMEOUT  3
#define ATTR_MESH_SCANNING_INTERVAL        MSEC_TO_UNITS(100, UNIT_0_625_MS)
#define ATTR_MESH_SCANNING_WINDOW          MSEC_TO_UNITS(40, UNIT_0_625_MS)
#define ATTR_TABLE_MAX_SIZE                0x200

#ifdef BOARD_SEEED_1_1
#define CLOCK_TYPE      NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM
#endif

typedef struct
{
  uint8_t length;
  uint8_t typeDefinition;
  uint16_t manufacturerId;
  uint8_t majorVersion;
  uint8_t minorVersion;
  uint32_t unused;   // used to be familyId
}__attribute__ ((packed)) advertisingManufacturerData;

typedef struct
{
  uint8_t length;
  uint8_t typeDefinition;
  uint8_t deviceName[NODE_NAME_SIZE];
}__attribute__ ((packed)) advertisingDeviceData;

typedef struct
{
  uint8_t length;
  uint8_t typeDefinition;
  uint8_t serviceUUID[16];
}__attribute__ ((packed)) advertisingServiceData;

typedef struct
{
  advertisingManufacturerData manufacturerData;
  advertisingDeviceData deviceData;
}__attribute__ ((packed)) advertisingData;

typedef struct
{
  advertisingServiceData serviceData;
}__attribute__ ((packed)) scanResponseData;


extern const ble_gap_conn_params_t meshConnectionParams;
extern const ble_gap_adv_params_t meshAdvertisingParams;
extern const ble_gap_scan_params_t meshScanningParams;
extern const ble_gap_scan_params_t meshConnectionScanningParams;

static uint8_t currentEventBuffer[sizeof(ble_evt_t) + BLE_L2CAP_MTU_DEF];
static ble_evt_t* currentEvent = (ble_evt_t *) currentEventBuffer;
static const uint16_t sizeOfEvent = sizeof(currentEventBuffer);

void gap_initialize(void);
void set_advertising_data(void);
void start_advertising(void);
void start_scanning(void);
void check_advertising_status(char **parsedCommandArray);
void check_scanning_status(char **parsedCommandArray);
void disconnect_from_peer(uint16_t connectionHandle);
void handle_gap_event(ble_evt_t * bleEvent);
MessagePropagationType receive_broadcast_message(uint16_t connectionHandle, uint8_t *dataPacket);