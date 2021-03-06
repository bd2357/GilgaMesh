cmake_minimum_required(VERSION 3.2)

# generate outputs macro
macro(GEN_OUTPUTS target)
    get_filename_component(name ${target} NAME_WE)
    #copy test executables
    add_custom_command(
        OUTPUT ${name}_copy_tests DEPENDS ${target}
        COMMAND mkdir -p ${BUILD_PATH}/tests
        COMMAND cp ${target} ${BUILD_PATH}/tests/${target}
    )
    add_custom_target(
        ${name}_outputs ALL DEPENDS ${name}_copy_tests
    )
endmacro(GEN_OUTPUTS)

#set compiler and linker options
SET(CMAKE_C_FLAGS_DEBUG "-DDEBUG -DSUPPRESS_INLINE_IMPLEMENTATION -D${TARGET_BOARD} -DNRF_LOG_USES_RTT -DSVCALL_AS_NORMAL_FUNCTION -DNRF51422 -DBOARD_PCA10028 -DSOFTDEVICE_PRESENT -DBLE_STACK_SUPPORT_REQD -DNRF51 -D__HEAP_SIZE=0 -DS130 -DSWI_DISABLE0 -DBSP_UART_SUPPORT -DTESTING -DNO_LOG_OUTPUT -std=gnu11 -g3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -fno-move-loop-invariants -fno-inline -Wextra -Wno-attributes -Wno-old-style-declaration -Og -O0 -fno-exceptions -fprofile-arcs -ftest-coverage")
SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")

# include cmocka test framework
include_directories(${DEPLOY_PATH}/cmocka/include)
link_directories(${DEPLOY_PATH}/cmocka/lib)

# production code headers
include_directories(${INC_PATH})
include_directories(${CONFIG_PATH})

# includes for real nordic sdk11 headers
include_directories(${COMPONENTS}/ble/common)
include_directories(${COMPONENTS}/libraries/experimental_section_vars)
include_directories(${COMPONENTS}/libraries/fds)
include_directories(${COMPONENTS}/libraries/fstorage)
include_directories(${COMPONENTS}/libraries/scheduler)
include_directories(${COMPONENTS}/libraries/timer)
include_directories(${COMPONENTS}/libraries/uart)
include_directories(${COMPONENTS}/libraries/log)
include_directories(${COMPONENTS}/libraries/log/src)
include_directories(${COMPONENTS}/libraries/twi)
include_directories(${COMPONENTS}/softdevice/common/softdevice_handler)
include_directories(${COMPONENTS}/drivers_nrf/common)
include_directories(${COMPONENTS}/drivers_nrf/hal)
include_directories(${COMPONENTS}/drivers_nrf/twi_master)
include_directories(${COMPONENTS}/device)
include_directories(${EXTERNAL}/segger_rtt)
include_directories(${SOFTDEVICE}/include)
include_directories(${SOFTDEVICE}/include/nrf51)

# include project mock headers, mosty for working around static implementations in header files
include_directories(mocks/include/sdk12/components/toolchain)
include_directories(mocks/include/sdk12/components/toolchain/cmsis/include)
include_directories(mocks/include/sdk12/components/libraries/util)
include_directories(mocks/include)


# add sdk12 mock implementations
list(APPEND TestFramework_SOURCES "mocks/fakeS130_v201/ble_mock.c")
list(APPEND TestFramework_SOURCES "mocks/fakeS130_v201/ble_gattc_mock.c")
list(APPEND TestFramework_SOURCES "mocks/fakeS130_v201/ble_gatts_mock.c")
list(APPEND TestFramework_SOURCES "mocks/fakeS130_v201/ble_l2cap_mock.c")
list(APPEND TestFramework_SOURCES "mocks/fakeS130_v201/nrf_sdm_mock.c")
list(APPEND TestFramework_SOURCES "mocks/fakeS130_v201/nrf_soc_mock.c")
list(APPEND TestFramework_SOURCES "mocks/fakeS130_v201/nrf51/nrf_mbr_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/ble/common/ble_conn_state_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/libraries/fds/fds_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/libraries/util/app_error_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/libraries/util/app_util_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/libraries/log/nrf_log_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/libraries/scheduler/app_scheduler_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/softdevice/common/softdevice_handler/softdevice_handler_appsh_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/softdevice/common/softdevice_handler/softdevice_handler_mock.c")
list(APPEND TestFramework_SOURCES "mocks/sdk12/components/toolchain/cmsis/cmsis_gcc_mock.c")

#add system shim mock implementations
list(APPEND TestFramework_SOURCES "mocks/system/delay_mock.c")
list(APPEND TestFramework_SOURCES "mocks/system/gpio_mock.c")
list(APPEND TestFramework_SOURCES "mocks/system/platform_mock.c")
list(APPEND TestFramework_SOURCES "mocks/system/pwm_mock.c")
list(APPEND TestFramework_SOURCES "mocks/system/timer_mock.c")

#add other useful files
list(APPEND TestFramework_SOURCES "${SRC_PATH}/system/log.c")


###############################################################################################
# add unit tests and mocks they need

add_executable(BroadcastMessageTest ${TestFramework_SOURCES}
        mocks/connection_mock.c
        mocks/gatt_mock.c
        ${SRC_PATH}/message_types/broadcast_message.c
        unit/message_types/BroadcastMessageTest.c)
target_link_libraries(BroadcastMessageTest cmocka)
GEN_OUTPUTS(BroadcastMessageTest)

add_executable(ConnectionTest ${TestFramework_SOURCES}
        mocks/device_mock.c
        mocks/gap_mock.c
        mocks/gatt_mock.c
        ${SRC_PATH}/conversion.c
        ${SRC_PATH}/connection.c
        unit/ConnectionTest.c)
target_link_libraries(ConnectionTest cmocka)
GEN_OUTPUTS(ConnectionTest)

add_executable(CommandTest ${TestFramework_SOURCES}
        mocks/message_types/broadcast_message_mock.c
        mocks/message_types/heartbeat_message_mock.c
        mocks/connection_mock.c
        mocks/gap_mock.c
        mocks/gatt_mock.c
        mocks/terminal_mock.c
        ${SRC_PATH}/command.c
        unit/CommandTest.c)
target_link_libraries(CommandTest cmocka)
GEN_OUTPUTS(CommandTest)

add_executable(GattTest ${TestFramework_SOURCES}
        mocks/message_types/broadcast_message_mock.c
        mocks/message_types/heartbeat_message_mock.c
        mocks/message_types/handshake_message_mock.c
        mocks/connection_mock.c
        mocks/error_mock.c
        mocks/gap_mock.c
        mocks/queue_mock.c
        ${SRC_PATH}/gatt.c
        unit/GattTest.c)
target_link_libraries(GattTest cmocka)
GEN_OUTPUTS(GattTest)

add_executable(HandshakeMessageTest ${TestFramework_SOURCES}
        mocks/connection_mock.c
        mocks/device_mock.c
        mocks/gatt_mock.c
        ${SRC_PATH}/message_types/handshake_message.c
        unit/message_types/HandshakeMessageTest.c)
target_link_libraries(HandshakeMessageTest cmocka)
GEN_OUTPUTS(HandshakeMessageTest)

add_executable(HeartbeatMessageTest ${TestFramework_SOURCES}
        mocks/connection_mock.c
        mocks/device_mock.c
        mocks/error_mock.c
        mocks/gatt_mock.c
        mocks/terminal_mock.c
        ${SRC_PATH}/conversion.c
        ${SRC_PATH}/message_types/heartbeat_message.c
        unit/message_types/HeartbeatMessageTest.c)
target_link_libraries(HeartbeatMessageTest cmocka)
GEN_OUTPUTS(HeartbeatMessageTest)

add_executable(QueueTest ${TestFramework_SOURCES}
        ${SRC_PATH}/queue.c
        unit/QueueTest.c)
target_link_libraries(QueueTest cmocka)
GEN_OUTPUTS(QueueTest)


