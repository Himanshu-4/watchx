cmake_minimum_required(VERSION 3.2.0)


# include the toolchain file
include(arm_toolchain.cmake)

# specify the cmake directories 
set(CMAKE_SOURCE_DIR src)
set(CMAKE_BINARY_DIR build )

set(CMAKE_PROJECT_DIR 
        D:/smart_watch/watchx
)

message("the openocd is here $ENV{OPENOCD}")

project(Watchx 
       VERSION 1.0.0 )

enable_language(C ASM)



# specify whether u want to generate DEbug or release Version
set(DEBUG_Flags 
        -O3 -g3
        )

set(RELEASE_Flags 
        # -O3
        )


set(ENABLE_DEBUG_LOGS 
        -DDEBUG_LOG=1
        )

# specify the C standard and the c extensions

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


# also export compile commadnds 
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)


############################################################################################################################################################
#############################################################################################################################################################
##############################################################################################################################################################
##########################################################   specify the sdk directoreis for BLE stack and nordic source 

# specify the sdk directory nd the project directory 
set(NRF_SDK_DIR  D:/smart_watch/watchx/nRF5_SDK_17)

set( LINKER_SRCH_PATH 
        D:/smart_watch/watchx
        )

set(app_linker_Script 
        nrf52_linkerscript.ld
    )




set(NIMBLE_PATH 
        D:/smart_watch/watchx/nRF5_SDK_17/external/mynewt-nimble 
    )

set(NRFX_DRIV_PATH
        D:/smart_watch/Watchx/nRF5_SDK_17/modules/nrfx
        )

set( PROJECT_BLE_FOLDER 
        ${CMAKE_SOURCE_DIR}/libs/components/ble
        )       

set( PROJECT_DEVICE_FOLDER 
${CMAKE_SOURCE_DIR}/libs/components/devices
)

############################################################################################################################
###############################################################################################################################
################################################################################################################################
############################################# specify the flags , compiler , linker , etc for the application   ###########################################################
set(COMMON_DEFINES 
        -DBUILD_APPLICATION=1 # this will configure the application build to use differnt sysconfig from bootloader build 
        -DBOARD_PCA10040
        -DCONFIG_GPIO_AS_PINRESET # make a GPIO to use as a pin reset 
        -DFLOAT_ABI_HARD # this will do this  __FPU_USED = 1
        # -DFLOAT_ABI_SOFT
      

        # -D__START=main 
        -D__HEAP_SIZE=8192 # specify the minimum heap and stack size 
        -D__STACK_SIZE=8192 # 8 Kb space 
        -DFREERTOS
        -DNRF52
        -DNRF52832_XXAA  
        -DNRF52_PAN_74
        -DCONFIG_NFCT_PINS_AS_GPIOS
        -DARM_MATH_CM4=1
        -DENABLE_FAULT_HANDLERS=1

        -DNRF_SD_BLE_API_VERSION=7
        -DS132
        -DSOFTDEVICE_PRESENT
        
        ${ENABLE_DEBUG_LOGS}
        # -DDEBUG  # if you want to debug the code then check it otherwise comment it 
        # -DSOFTDEVICE_PRESENT # tell the compiler that softdevice is present 
        # -DENABLE_SWO # uncomment it if you want to enable serial wire output 
)

set(COMMON_C_FLAGS        
        -mcpu=cortex-m4
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        # -mfloat-abi=soft #for  using the soft floating point library 
        ${DEBUG_Flags}
        ${RELEASE_Flags}
        -mthumb
        -mabi=aapcs
        -Wall -Werror 
        # -Wno-discarded-qualifiers
        # keep every  function in a separate section, this allows linker to discard unused ones
        -ffunction-sections
        -fdata-sections
        -fno-strict-aliasing
        -fno-builtin
        -fshort-enums
        # # must have to include this because in nimble stack there are function used by ble stack which are not declared to them
        ###333 you can discard these flags in the release build
        -Wno-format
        -Wno-unused-function
        -Wno-unused-variable
)


set(LINKER_FLAGS 
        -L${LINKER_SRCH_PATH}
        -T${app_linker_Script}
        -mcpu=cortex-m4
        -mthumb -mabi=aapcs
        -mfloat-abi=hard -mfpu=fpv4-sp-d16
        ${DEBUG_Flags}
        -Wl,-Map=${PROJECT_NAME}.map
        -Wl,--gc-sections  --specs=nano.specs
        -Wl,--start-group
        -lstdc++
        -Wl,--end-group
        -lc
        -lnosys
        -lm
        -Wl,--print-memory-usage
    )





###########################################################################################################################################################
##########################################################################################################################################################
#########################################################################################################################################################
############################################################## set the SDK libs and startup file for the NRF52
# specify the source files 
set( SDK_SRC_FILES

# ${NRF_SDK_DIR}/components/boards/boards.c 

# # build the softdevice specific files
# ${NRF_SDK_DIR}/components/softdevice/common/nrf_sdh_ble.c
# ${NRF_SDK_DIR}/components/softdevice/common/nrf_sdh_soc.c
# ${NRF_SDK_DIR}/components/softdevice/common/nrf_sdh.c
# sdk libraries 
#  for sttring error handling 
# nrf string error libs
# ${NRF_SDK_DIR}/components/libraries/strerror/nrf_strerror.c
# nordic section iterator for handling sections 
# ${NRF_SDK_DIR}/components/libraries/experimental_section_vars/nrf_section_iter.c

# # nordic log module 
# ${NRF_SDK_DIR}/components/libraries/log/src/nrf_log_frontend.c 
# ${NRF_SDK_DIR}/components/libraries/log/src/nrf_log_str_formatter.c 

  # std libs library 
# ${NRF_SDK_DIR}/components/libraries/button/app_button.c 

# util platform error handler , gcc error handler 
# ${NRF_SDK_DIR}/components/libraries/util/app_error.c 
# ${NRF_SDK_DIR}/components/libraries/util/app_error_handler_gcc.c 
# ${NRF_SDK_DIR}/components/libraries/util/app_error_weak.c 
# ${NRF_SDK_DIR}/components/libraries/util/app_util_platform.c 
# ${NRF_SDK_DIR}/components/libraries/util/nrf_assert.c 
# ${NRF_SDK_DIR}/components/libraries/timer/app_timer_freertos.c 
# ${NRF_SDK_DIR}/components/libraries/atomic/nrf_atomic.c 
# ${NRF_SDK_DIR}/components/libraries/memobj/nrf_memobj.c 
# ${NRF_SDK_DIR}/components/libraries/ringbuf/nrf_ringbuf.c 
# ${NRF_SDK_DIR}/components/libraries/strerror/nrf_strerror.c 
# ${NRF_SDK_DIR}/components/libraries/balloc/nrf_balloc.c 
# ${NRF_SDK_DIR}/components/boards/boards.c 
# contains the 
# ${NRF_SDK_DIR}/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c 
# ${NRF_SDK_DIR}/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c 


# integration library for the clock and power modules 

# the nrf clcok hal for the clock inits
# ${NRF_SDK_DIR}/integration/nrfx/legacy/nrf_drv_clock.c
# ${NRF_SDK_DIR}/modules/nrfx/drivers/src/nrfx_clock.c 

# # nrf power driver for power regulation system 
# ${NRF_SDK_DIR}/integration/nrfx/legacy/nrf_drv_power.c
# ${NRF_SDK_DIR}/modules/nrfx/drivers/src/nrfx_power.c 

# build the driver library for the uart 
# ${NRF_SDK_DIR}/integration/nrfx/legacy/nrf_drv_uart.c
# ${NRF_SDK_DIR}/modules/nrfx/drivers/src/nrfx_uart.c 


# ${NRF_SDK_DIR}/modules/nrfx/soc/nrfx_atomic.c 
# build the file for power init 
# include the nrf uart peripheral driver 

# ${NRF_SDK_DIR}/modules/nrfx/drivers/src/nrfx_gpiote.c 

# segger RTT support 
# ${NRF_SDK_DIR}/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
# ${NRF_SDK_DIR}/external/segger_rtt/SEGGER_RTT.c
# ${NRF_SDK_DIR}/external/segger_rtt/SEGGER_RTT_printf.c

# the file printf format library 
# ${NRF_SDK_DIR}/external/fprintf/nrf_fprintf.c 
# ${NRF_SDK_DIR}/external/fprintf/nrf_fprintf_format.c 
# the source file of your project
        
)

      
# specify the include directories 
set(SDK_INCLUDE_DIR 

      ${NRF_SDK_DIR}/external/freertos/include 
      ${CMAKE_PROJECT_DIR}/src/config

      ###################################
#       ${NRF_SDK_DIR}/components/libraries/strerror

      ##########################################################################3
      ############################################################################
      ########################  these are the includes when using a softdevice 
# #       include the softdevice s132  headers 
      ${NRF_SDK_DIR}/components/softdevice/s132/headers/nrf52
      ${NRF_SDK_DIR}/components/softdevice/s132/headers
        #include the common headers for the softdevice 
        # ${NRF_SDK_DIR}/components/softdevice/common 

      # include the  experimental section variables used to define vars in particular ram section 
#       ${NRF_SDK_DIR}/components/libraries/experimental_section_vars

    

# arm math library ffor the computation of the floats, and math operations
        ${NRF_SDK_DIR}/components/toolchain/cmsis/dsp/Include
      ${NRF_SDK_DIR}/components   # contains the file sdk validation .h to check the sdkconfig is correct or not 
      
      # this contains the app error handlers 
      ${NRF_SDK_DIR}/components/libraries/util 
#       ${NRF_SDK_DIR}/components/libraries/balloc 
#       ${NRF_SDK_DIR}/components/libraries/ringbuf 
#       ${NRF_SDK_DIR}/components/libraries/bsp 
#       ${NRF_SDK_DIR}/components/libraries/log cd
#       ${NRF_SDK_DIR}/components/libraries/button 
#       ${NRF_SDK_DIR}/components/libraries/timer 
#       ${NRF_SDK_DIR}/components/libraries/strerror 
#       ${NRF_SDK_DIR}/components/libraries/atomic 
#       ${NRF_SDK_DIR}/components/libraries/memobj 
#       ${NRF_SDK_DIR}/components/libraries/experimental_section_vars 

# this contains the log library structure for the nrf
#       ${NRF_SDK_DIR}/components/libraries/log/src 
#       ${NRF_SDK_DIR}/components/libraries/log
#       ${NRF_SDK_DIR}/components/libraries/delay
#       ${NRF_SDK_DIR}/components/boards 
#       ${NRF_SDK_DIR}/components/drivers_nrf/nrf_soc_nosd 

      ${NRF_SDK_DIR}/components/toolchain/cmsis/include 
      


      # path for the NRFX driver API v 2.10
      ${NRFX_DRIV_PATH}
      ${NRFX_DRIV_PATH}/hal 
      ${NRFX_DRIV_PATH}/nrfx 
      ${NRFX_DRIV_PATH}/mdk 
#       ${NRFX_DRIV_PATH}/drivers/include 

#       ${NRF_SDK_DIR}/integration/nrfx/legacy 
#       ${NRF_SDK_DIR}/integration/nrfx 
      
         # contains the main_helper library 
         ${CMAKE_SOURCE_DIR}/main

      )




###########################################################################################################################################################
##########################################################################################################################################################
#########################################################################################################################################################
############################################################## set the FREERTOS sources and include directories 


# freertos source files 
set(FREERTOS_SRC
        # freertos specific libraries 
        ${NRF_SDK_DIR}/external/freertos/source/croutine.c 
        ${NRF_SDK_DIR}/external/freertos/source/event_groups.c 
        ${NRF_SDK_DIR}/external/freertos/source/list.c 
        ${NRF_SDK_DIR}/external/freertos/source/queue.c 
        ${NRF_SDK_DIR}/external/freertos/source/stream_buffer.c 
        ${NRF_SDK_DIR}/external/freertos/source/tasks.c 
        ${NRF_SDK_DIR}/external/freertos/source/timers.c 
        ${NRF_SDK_DIR}/external/freertos/source/portable/MemMang/heap_4.c 

        # freerttos porting code for nrf52 target 
        ${NRF_SDK_DIR}/external/freertos/portable/GCC/nrf52/port.c
        # ${NRF_SDK_DIR}/external/freertos/portable/ARM_CM4F/port.c 
        ${NRF_SDK_DIR}/external/freertos/portable/CMSIS/nrf52/port_cmsis.c 
        ${NRF_SDK_DIR}/external/freertos/portable/CMSIS/nrf52/port_cmsis_systick.c 

        # there is a freertos addition in which two functions are defined 
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/freertos_addition/static_func_support.c
        )

# # set the freertos include directories 

set(FREERTOS_INCLUDE 

        ${NRF_SDK_DIR}/external/freertos/source/include 

        #porting libs code  for the nrf52 target 
        ${NRF_SDK_DIR}/external/freertos/portable/GCC/nrf52
        ${NRF_SDK_DIR}/external/freertos/portable/CMSIS/nrf52 
        # ${NRF_SDK_DIR}/external/freertos/portable/CMSIS/nrf52
        )

add_library(freertos_lib STATIC 
            ${FREERTOS_SRC}
            )



target_compile_definitions(freertos_lib PRIVATE
        ${COMMON_DEFINES}   
 
)

target_compile_options(freertos_lib PRIVATE
           ${COMMON_C_FLAGS}
           $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized>
           $<$<COMPILE_LANGUAGE:CXX>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti>
           $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp>
             )


target_include_directories(freertos_lib PUBLIC SYSTEM 
        ${FREERTOS_INCLUDE}
        ${SDK_INCLUDE_DIR}
            )


     
    
# ############################################################################################################################################


##################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
##########################################################   Build the SDK stdlib files   ##################################################################################
list(APPEND 
    SDK_SRC_FILES
    # gpio related library , also gpio task and events 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/gpio/gpio.c
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/gpio/gpiote.c 
    
    # the I2c library for the nrf
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/i2c/i2c.c
    # create the thread safe API
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/i2c/i2c_thread_safe.c
    
    # spi library for the nrf52 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/spi/spi.c
    # spi thread safe library for using with retos enviournment 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/spi/spi_thread_safe.c
    
    # system related library 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/system/system.c 
    # implement the low level system calls 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/system/sys_calls.c
    # implement the mem management of heap
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/system/sysmem.c

    # the timer related libraries 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/timer/timer.c 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/timer/nrf_time.c 
    
    # uart related libraries 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/uart/uart_drv.c
    # uart logging library for log the serial data using ring buffer, thread safe API
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/uart/uart_logs.c
    
    #ppi related libraries  
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/ppi/ppi.c

    # pwm related lib 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/pwm/nrf_pwm.c 

    # adc related library 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/adc/adc.c
    
    # watchdog timer API 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/watchdog/watchdog.c
    
    # the block protection API
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/bprot/bprot.c 
    
    # flash accesss API to read write flash
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/flash/flash_write.c 

        # non volatile storage library for handling flash write operations
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/nvs/nvs.c
    
        # these are the startup files to start the arm cortex m4 core 
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/startup/gcc_startup_nrf52.S 
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/startup/system_nrf52.c 

    )

list(APPEND 
    SDK_INCLUDE_DIR 
           # gpio related library , also gpio task and events 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/gpio
    # the I2c library for the nrf
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/i2c
    # spi library for the nrf52 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/spi
    # system related library 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/system 
    # the timer related libraries 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/timer  
    # uart related libraries 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/uart
        #ppi related libraries  
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/ppi
     # pwm related lib 
     ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/pwm

    # adc related library 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/adc
    # watchdog timer API 
    ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/watchdog
        # the block protection API
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/bprot 
        # flash accesss API to read write flash
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/flash 
        # non volatile storage library 
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/nvs

        # contains the startup scripts 
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/startup
    )



        # build the nrf sdk library 

        # make a library of external 
        add_library(nrf_sdk_libs  STATIC 
        ${SDK_SRC_FILES}
        )
        
        
        
        target_compile_definitions(nrf_sdk_libs PRIVATE
            ${COMMON_DEFINES}   
        #     ${NIMBLE_STACK_DEFINES}
        
        )
        
        target_compile_options(nrf_sdk_libs PRIVATE
               ${COMMON_C_FLAGS}
               $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized >
               $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti >
               $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp>
                 )
        
        
        target_include_directories(nrf_sdk_libs PUBLIC SYSTEM 
            ${SDK_INCLUDE_DIR}
            ${FREERTOS_INCLUDE}
          )
        
        
#######################################################################################3###########################
##################################################################################################
##################################################################################################

# this contains the all ble  related files 


file(GLOB ble_ams_files 
${PROJECT_BLE_FOLDER}/ble_ams/*.c 
)

file(GLOB ble_ancs_files 
${PROJECT_BLE_FOLDER}/ble_ancs/*.c 
)

file( GLOB ble_gap_files 
${PROJECT_BLE_FOLDER}/ble_gap/*.c 
)

file ( GLOB ble_gatt_client_files
${PROJECT_BLE_FOLDER}/ble_gatt_client/*.c 
)

file( GLOB ble_gatt_server_files
${PROJECT_BLE_FOLDER}/ble_gatt_server/*.c
)

file(GLOB  ble_stack_init_files  
${PROJECT_BLE_FOLDER}/ble_stack_init/*.c
)

file(GLOB ble_task_files 
${PROJECT_BLE_FOLDER}/ble_task/*.c 
)

file( GLOB ble_current_time_files
${PROJECT_BLE_FOLDER}/ble_time/*.c 
)




    # deifne the external libraries for the sensors ans peropheral that connect with nrf52
set(BLE_LIBS_SRC
        # this is the ble stack init files 
        ${ble_ams_files}

        ${ble_ancs_files}
        
        ${ble_gap_files}
        
        ${ble_gatt_client_files}
        
        ${ble_gatt_server_files}
        
        ${ble_stack_init_files}
        
        ${ble_task_files}
                
        ${ble_current_time_files}

        )

set(BLE_LIB_INCLUDE_DIR
        # the  libraries header file paths 
  
        # here ancs services headress  be found        
        ${PROJECT_BLE_FOLDER}/ble_ancs
        # here the ams service can be found 
        ${PROJECT_BLE_FOLDER}/ble_ams
         # gap related headers here 
         ${PROJECT_BLE_FOLDER}/ble_gap
         # gatt client related headers  here 
         ${PROJECT_BLE_FOLDER}/ble_gatt_client
         
         # gatt server related headers  here 
         ${PROJECT_BLE_FOLDER}/ble_gatt_server

        # ble common library for the ble_config header file 
        ${PROJECT_BLE_FOLDER}/ble_stack_init

        # the ble task folder 
        ${PROJECT_BLE_FOLDER}/ble_task
        
       # ble time library 
        ${PROJECT_BLE_FOLDER}/ble_time 

       

        )



        # make a library of external 
add_library(ble_library STATIC 
${BLE_LIBS_SRC}
)



target_compile_definitions(ble_library PRIVATE
    ${COMMON_DEFINES}   
#     ${NIMBLE_STACK_DEFINES}

)

target_compile_options(ble_library PRIVATE
       ${COMMON_C_FLAGS}
       $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized >
       $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti >
       $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp>
         )


target_include_directories(ble_library PUBLIC SYSTEM 
    ${BLE_LIB_INCLUDE_DIR}
    ${SDK_INCLUDE_DIR}
    ${FREERTOS_INCLUDE}

  
        )



##############################################################################
###########################################################################################3
################################################################################################
######################################################################################################
####################################################################################################

# this contains external  devices  files 

        # this is accelrometer related  files 
        file ( GLOB device_accelrometer_files 
        ${PROJECT_DEVICE_FOLDER}/accelrometer/*.c 
        )
        # battery related 
        file ( GLOB device_battery_files
        ${PROJECT_DEVICE_FOLDER}/battery/*.c 
        )
        # button related
        file ( GLOB device_button_files
        ${PROJECT_DEVICE_FOLDER}/button/*.c 
        )
        # buzzer related 
        file ( GLOB device_buzzer_files
        ${PROJECT_DEVICE_FOLDER}/buzzer/*.c 
        )
        #display related 
        file ( GLOB device_display_files
        ${PROJECT_DEVICE_FOLDER}/display/*.c 
        )
        # led relted 
        file ( GLOB device_led_files
        ${PROJECT_DEVICE_FOLDER}/led/*.c 
        )

set( EXTERNAL_DEVICES_SRC 
        ${device_accelrometer_files}
        ${device_battery_files}
        ${device_button_files}
        ${device_buzzer_files}
        ${device_buzzer_files}
        ${device_display_files}
        ${device_led_files}

        )


    # deifne the external libraries for the sensors ans peropheral that connect with nrf52
set(EXTERNAL_DEVICES_INCLUDE_DIR
        # this is accelrometer related  files 
        ${PROJECT_DEVICE_FOLDER}/accelrometer 

        # battery related 
        ${PROJECT_DEVICE_FOLDER}/battery

        # button related
        ${PROJECT_DEVICE_FOLDER}/button

        # buzzer related 
        ${PROJECT_DEVICE_FOLDER}/buzzer 

        #display related 
        ${PROJECT_DEVICE_FOLDER}/display 

        # led relted 
        ${PROJECT_DEVICE_FOLDER}/led 
        )


        # make a library of external that are connected to the nrf52 
add_library(ext_devices  STATIC 
${EXTERNAL_DEVICES_SRC}
)



target_compile_definitions(ext_devices PRIVATE
    ${COMMON_DEFINES}   
#     ${NIMBLE_STACK_DEFINES}

)

target_compile_options(ext_devices PRIVATE
       ${COMMON_C_FLAGS}
       $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized >
       $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti >
       $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp>
         )


target_include_directories(ext_devices PUBLIC SYSTEM 
    ${EXTERNAL_DEVICES_INCLUDE_DIR}
    ${SDK_INCLUDE_DIR}
    ${FREERTOS_INCLUDE}
  
        )


###########################################################################################################################################################
##########################################################################################################################################################
#########################################################################################################################################################
############################################################## specify the main source files of your project here 

set(PROJECT_FILES
${CMAKE_SOURCE_DIR}/main/main.c
${CMAKE_SOURCE_DIR}/main/main_helper.c
)

set(PROJECT_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/main
        )
#########################################################################
################################# build the project 
        
# specify the executable 
set(EXECUTABLE ${PROJECT_NAME} )

add_executable(${EXECUTABLE} 
        ${PROJECT_FILES}
        )


target_include_directories(${EXECUTABLE} PRIVATE
             ${SDK_INCLUDE_DIR}
             ${BLE_LIB_INCLUDE_DIR}
             ${EXTERNAL_LIB_INCLUDE_DIR}
                ${PROJECT_INCLUDE_DIR}
        )

        
target_compile_definitions(${EXECUTABLE} PRIVATE
    # specify the common definees     
    ${COMMON_DEFINES}

)


target_compile_options(${EXECUTABLE} PRIVATE
    
        # C flags common to all targets
        ${COMMON_C_FLAGS}

        )



target_link_libraries(${EXECUTABLE}
                       freertos_lib
                       ble_library
                       ext_devices
                       nrf_sdk_libs 
                #        nimble_ble_stack
                #        gpio_lib # no library gpio_lib so cmake discard it 
                        )

                        # specify the linker options 
target_link_options(${EXECUTABLE} PRIVATE

        ${LINKER_FLAGS}

        )

     # set the target properties to make output file as .elf executable and linkable format 
set_target_properties(${EXECUTABLE}
        PROPERTIES SUFFIX  .elf
 )      



##################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
##########################################################   create extra files from the build hex and bin format  ##################################################################################

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD

        COMMAND echo  "Succesfully build YOUR Project  :)"
        COMMAND echo  "BUilding Extra Files Hex, BIN "
        
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE}.elf   ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE}.elf ${PROJECT_NAME}.bin
        )


                #Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND echo " THe Size of your project is shown below :) "
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}.elf
        )





# add_custom_target(merge_hex_files )
############################################################################################################################################################################
##############################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
################################ make the bootloader for the nrf52 ##################################################################################


###################################################################################
#########################   these are the cmds used to recover and erase the chip

add_custom_target(erase_chip
                COMMAND echo "erasing the nrf52832 chip"
                COMMAND nrfjprog -f nrf52 --eraseall)

add_custom_target(recover
                COMMAND echo "recovering the nrf52832 chip"
                COMMAND nrfjprog -f nrf52 --recover)


add_custom_target(flash_softdevice
                COMMAND echo Flashing: s132_nrf52_7.2.0_softdevice.hex
                COMMAND nrfjprog -f nrf52 --program ${NRF_SDK_DIR}/components/softdevice/s132/hex/s132_nrf52_7.2.0_softdevice.hex --sectorerase --verify
                COMMAND nrfjprog -f nrf52 --reset
                )

add_custom_target(flash 
                COMMAND echo "flashing ${PROJECT_NAME}.elf into the nrf52832"
                COMMAND nrfjprog -f nrf52 --program ${PROJECT_NAME}.hex --sectorerase --verify
                COMMAND nrfjprog -f nrf52 --reset
                )
