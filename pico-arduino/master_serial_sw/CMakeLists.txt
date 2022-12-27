cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(master_serial)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(master_serial
        master_serial.c
        )
    pico_generate_pio_header(master_serial ${CMAKE_CURRENT_LIST_DIR}/uart_tx.pio)
    pico_generate_pio_header(master_serial ${CMAKE_CURRENT_LIST_DIR}/uart_rx.pio)
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(master_serial pico_stdlib hardware_pio)

	# enable usb output, disable uart output
    pico_enable_stdio_usb(master_serial 1)
    pico_enable_stdio_uart(master_serial 0)


    
	# create map/bin/hex file etc.
	pico_add_extra_outputs(master_serial)

	# add url via pico_set_program_url
#	example_auto_set_url(master_serial)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building master_serial because TinyUSB submodule is not initialized in the SDK")
endif()
	
