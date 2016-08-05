#include "cmocka_includes.h"
#include "gatt.h"

static int gatt_test_setup(void **state) {
  initialize_write_events();
  return 0;
}

static MessagePropagationType foo_write_event_handler(uint16_t connectionHandle, uint8_t *dataPacket) {
  check_expected(connectionHandle);
  check_expected_ptr(dataPacket);
  return DoNotPropagate;
}

static void test_broadcast_event_handled() {
  ble_evt_t bleEvent;
  uint8_t broadcastEventMessageData[] = { 2 };
  uint16_t handle = 5;
  bleEvent.evt.gap_evt.conn_handle = handle;
  memcpy(bleEvent.evt.gatts_evt.params.write.data, broadcastEventMessageData, sizeof(uint8_t));
    
  expect_value(receive_broadcast_message, connectionHandle, handle);
  expect_memory(receive_broadcast_message, dataPacket, broadcastEventMessageData, sizeof(uint8_t));
  handle_write_event(&bleEvent, 0);
}

static void test_connection_info_event_handled() {
  ble_evt_t bleEvent;
  uint8_t connectionInfoEventMessageData[] = { 3 };
  uint16_t handle = 5;
  bleEvent.evt.gap_evt.conn_handle = handle;
  memcpy(bleEvent.evt.gatts_evt.params.write.data, connectionInfoEventMessageData, sizeof(uint8_t));

  expect_value(update_connection_info, connectionHandle, handle);
  expect_memory(update_connection_info, dataPacket, connectionInfoEventMessageData, sizeof(uint8_t));
  handle_write_event(&bleEvent, 0);
}

static void test_heartbeat_event_handled() {
  ble_evt_t bleEvent;
  uint8_t heartbeatEventMessageData[] = { 4 };
  uint16_t handle = 5;
  bleEvent.evt.gap_evt.conn_handle = handle;
  memcpy(bleEvent.evt.gatts_evt.params.write.data, heartbeatEventMessageData, sizeof(uint8_t));

  expect_value(receive_heartbeat, connectionHandle, handle);
  expect_memory(receive_heartbeat, dataPacket, heartbeatEventMessageData, sizeof(uint8_t));
  handle_write_event(&bleEvent, 0);
}

static void test_new_event_handled() {
  ble_evt_t bleEvent;
  uint8_t newEventMessageData[] = { 78 };
  uint16_t handle = 5;
  bleEvent.evt.gap_evt.conn_handle = handle;
  memcpy(bleEvent.evt.gatts_evt.params.write.data, newEventMessageData, sizeof(uint8_t));

  add_write_event(78, foo_write_event_handler);
  
  expect_value(foo_write_event_handler, connectionHandle, handle);
  expect_memory(foo_write_event_handler, dataPacket, newEventMessageData, sizeof(uint8_t));
  handle_write_event(&bleEvent, 0);
}

int RunGattTest(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_new_event_handled),
            cmocka_unit_test(test_heartbeat_event_handled),
            cmocka_unit_test(test_broadcast_event_handled),
            cmocka_unit_test(test_connection_info_event_handled),
    };

    return cmocka_run_group_tests_name("GattTest", tests, gatt_test_setup, NULL);
}

int main() {
    return RunGattTest();
}