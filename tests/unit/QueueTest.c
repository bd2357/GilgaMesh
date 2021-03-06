#include "queue.h"

#include "cmocka_includes.h"

static uint8_t testData[3] = { 1, 2, 3 };
static uint8_t data[MAX_QUEUE_DATA_LENGTH] = { 0 };
static uint16_t dataLength;

static void Queue_is_empty_with_no_elements() {
  transmissionPacketQueue queue = { 0 };

  assert_true(queue_is_empty(&queue));
}

static void Queue_is_not_empty_with_elements() {
  transmissionPacketQueue queue = { 0 };
  push_onto_queue(&queue, testData, sizeof(testData));

  assert_false(queue_is_empty(&queue));
}

static void Queue_is_empty_after_popping_all_elements() {
  transmissionPacketQueue queue = { 0 };
  push_onto_queue(&queue, testData, sizeof(testData));
  pop_from_queue(&queue);

  assert_true(queue_is_empty(&queue));
}

static void Queue_push_succeeds_when_queue_has_empty_spots() {
  transmissionPacketQueue queue = { 0 };

  assert_true(push_onto_queue(&queue, testData, sizeof(testData)));
}

static void Queue_push_fails_when_queue_is_full() {
  transmissionPacketQueue queue = { 0 };
  for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
    uint8_t data[1] = { (uint8_t)(i + 1) };
    assert_true(push_onto_queue(&queue, data, 1));
  }

  assert_false(push_onto_queue(&queue, testData, sizeof(testData)));
}

static void Queue_push_fails_when_data_is_too_big() {
  transmissionPacketQueue queue = { 0 };
  uint8_t data[MAX_QUEUE_DATA_LENGTH + 1] = { 1 };

  assert_false(push_onto_queue(&queue, data, sizeof(data)));
}

static void Queue_push_adds_element_to_queue() {
  transmissionPacketQueue queue = { 0 };
  push_onto_queue(&queue, testData, sizeof(testData));

  peek_from_queue(&queue, data, &dataLength);

  assert_memory_equal(testData, data, dataLength);
  assert_int_equal(sizeof(testData), dataLength);
}

static void Queue_pop_frees_space_in_queue() {
  transmissionPacketQueue queue = { 0 };
  for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
    uint8_t data[1] = { (uint8_t)(i + 1) };
    assert_true(push_onto_queue(&queue, data, 1));
  }

  pop_from_queue(&queue);

  assert_true(push_onto_queue(&queue, testData, sizeof(testData)));
}

static void Queue_peek_and_pop_return_elements_in_fifo_order() {
  uint8_t data1[1] = { 1 };
  uint8_t data2[2] = { 1, 2 };
  uint8_t data3[3] = { 1, 2, 3 };

  transmissionPacketQueue queue = { 0 };
  push_onto_queue(&queue, data1, sizeof(data1));
  push_onto_queue(&queue, data2, sizeof(data2));
  push_onto_queue(&queue, data3, sizeof(data3));

  assert_true(peek_from_queue(&queue, data, &dataLength));
  assert_memory_equal(&data1, data, dataLength);
  assert_int_equal(sizeof(data1), dataLength);
  assert_true(pop_from_queue(&queue));

  assert_true(peek_from_queue(&queue, data, &dataLength));
  assert_memory_equal(&data2, data, dataLength);
  assert_int_equal(sizeof(data2), dataLength);
  assert_true(pop_from_queue(&queue));

  assert_true(peek_from_queue(&queue, data, &dataLength));
  assert_memory_equal(&data3, data, dataLength);
  assert_int_equal(sizeof(data3), dataLength);
  assert_true(pop_from_queue(&queue));
}

static void Queue_pop_returns_false_when_queue_is_empty() {
  transmissionPacketQueue queue = { 0 };
  assert_false(pop_from_queue(&queue));
};

static void Queue_pop_returns_true_when_queue_has_elements() {
  transmissionPacketQueue queue = { 0 };

  push_onto_queue(&queue, testData, sizeof(testData));

  assert_true(pop_from_queue(&queue));
}

static void Queue_wraps_around_when_adding_elements() {
  transmissionPacketQueue queue = { 0 };
  queue.front = MAX_QUEUE_SIZE - 1;

  push_onto_queue(&queue, testData, sizeof(testData));
  assert_int_equal(1, queue.count);
  assert_int_equal(MAX_QUEUE_SIZE - 1, queue.front);

  push_onto_queue(&queue, testData, sizeof(testData));
  assert_int_equal(2, queue.count);
  assert_int_equal(MAX_QUEUE_SIZE - 1, queue.front);

  pop_from_queue(&queue);
  assert_int_equal(1, queue.count);
  assert_int_equal(0, queue.front);

  pop_from_queue(&queue);
  assert_int_equal(0, queue.count);
  assert_int_equal(1, queue.front);
}

int RunQueueTest(void) {
  const struct CMUnitTest tests[] = {
          cmocka_unit_test(Queue_is_empty_with_no_elements),
          cmocka_unit_test(Queue_is_not_empty_with_elements),
          cmocka_unit_test(Queue_is_empty_after_popping_all_elements),
          cmocka_unit_test(Queue_push_succeeds_when_queue_has_empty_spots),
          cmocka_unit_test(Queue_push_fails_when_queue_is_full),
          cmocka_unit_test(Queue_push_fails_when_data_is_too_big),
          cmocka_unit_test(Queue_push_adds_element_to_queue),
          cmocka_unit_test(Queue_pop_frees_space_in_queue),
          cmocka_unit_test(Queue_peek_and_pop_return_elements_in_fifo_order),
          cmocka_unit_test(Queue_pop_returns_false_when_queue_is_empty),
          cmocka_unit_test(Queue_pop_returns_true_when_queue_has_elements),
          cmocka_unit_test(Queue_wraps_around_when_adding_elements)
  };

  return cmocka_run_group_tests_name("QueueTest", tests, NULL, NULL);
}

int main() {
  RunQueueTest();
}
