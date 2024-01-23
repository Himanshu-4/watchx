# minimum cmake required for processing this file 
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

enable_language(C ASM CXX)



set(CMAKE_BUILD_TYPE "Debug" )
# specify whether u want to generate DEbug or release Version
set(DEBUG_Flags 
        -O0
        )

set(RELEASE_Flags 
        -O3 -g0 
        )
# specify the C standard and the c extensions

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)



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

set(LINKER_SCRIPT_WITH_SD 
        nrf52_linkerscript.ld
    )

set(LINKER_SCRIPT_WITHOUT_SD 
        nrf52_nosd_linkerscript.ld
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

set(PROJECT_KERNEL_FOLDER 
${CMAKE_SOURCE_DIR}/libs/components/kernel
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
        # this will enable differnet exception handlers like memmanagefault,busg=fault ,etc 
        -DENABLE_FAULT_HANDLERS=1

        -DNRF_SD_BLE_API_VERSION=7
        -DS132
        -DSOFTDEVICE_PRESENT
        
          # -DDEBUG  # if you want to debug the code then check it otherwise comment it 
        # -DSOFTDEVICE_PRESENT # tell the compiler that softdevice is present 
        # -DENABLE_SWO # uncomment it if you want to enable serial wire output 
)

set(COMMON_C_FLAGS        
        -mcpu=cortex-m4
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        # -mfloat-abi=soft #for  using the soft floating point library 
        $<$<CONFIG:RELEASE>: ${RELEASE_Flags}>
        $<$<CONFIG:DEBUG>: ${DEBUG_Flags} >
        
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
        -Wno-array-bounds
        -Wno-dangling-pointer
)


set(LINKER_FLAGS 
        -L${LINKER_SRCH_PATH}
        # -T${app_linker_Script}
        -mcpu=cortex-m4
        -mthumb -mabi=aapcs
        -mfloat-abi=hard -mfpu=fpv4-sp-d16
        -u _printf_float # this will include the printf symbol in the elf
        # ${DEBUG_Flags}
        $<$<CONFIG:RELEASE>: ${RELEASE_Flags} >
        $<$<CONFIG:DEBUG>: ${DEBUG_Flags} >
        -Wl,-Map=${PROJECT_NAME}.map
        -Wl,--gc-sections --specs=nano.specs
        -Wl,--start-group
        -lstdc++
        -Wl,--end-group
        -lnosys # no semihosting is required 
        -lm # link with the math library 
        -Wl,--print-memory-usage
    )




###########################################################################################################################################################
##########################################################################################################################################################
#########################################################################################################################################################
############################################################## set the SDK libs and startup file for the NRF52
# specify the source files 
set( SDK_SRC_FILES
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/startup/gcc_startup_nrf52.S
)

      
# specify the include directories 
set(SDK_INCLUDE_DIR 

      ${CMAKE_SOURCE_DIR}/config

      ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/system
      ##########################################################################3
      ############################################################################
      ########################  these are the includes when using a softdevice 
# #       include the softdevice s132  headers 
      ${NRF_SDK_DIR}/components/softdevice/s132/headers/nrf52
      ${NRF_SDK_DIR}/components/softdevice/s132/headers    

# arm math library ffor the computation of the floats, and math operations
        ${NRF_SDK_DIR}/components/toolchain/cmsis/dsp/Include
      ${NRF_SDK_DIR}/components   # contains the file sdk validation .h to check the sdkconfig is correct or not 
      
      # this contains the app error handlers 
      ${NRF_SDK_DIR}/components/libraries/util 

      ${NRF_SDK_DIR}/components/toolchain/cmsis/include 
      


      # path for the NRFX driver API v 2.10
      ${NRFX_DRIV_PATH}
      ${NRFX_DRIV_PATH}/hal 
      ${NRFX_DRIV_PATH}/nrfx 
      ${NRFX_DRIV_PATH}/mdk 
#       ${NRFX_DRIV_PATH}/drivers/include 

#       ${NRF_SDK_DIR}/integration/nrfx/legacy 
#       ${NRF_SDK_DIR}/integration/nrfx 
      
      # contains the watchx kernel directory 
      ${PROJECT_KERNEL_FOLDER} 

         
      )


###########################################################################################################################################################
##########################################################################################################################################################
#########################################################################################################################################################
################# build the freertos library here ############################################################ 

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
        # ${NRF_SDK_DIR}/external/freertos/portable/ARM_CM4F/port.c 
        ${NRF_SDK_DIR}/external/freertos/portable/GCC/nrf52/port.c
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
           $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized  >
           $<$<COMPILE_LANGUAGE:CXX>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti>
           $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp >
             )


target_include_directories(freertos_lib PUBLIC SYSTEM 
        ${FREERTOS_INCLUDE}
        ${SDK_INCLUDE_DIR}
            )

message(" =========================== the Freertos Includes directories are ============================")
foreach(item ${SDK_INCLUDE_DIR})
        message(${item})
endforeach(item ${SDK_INCLUDE_DIR})
foreach(item ${FREERTOS_INCLUDE})
        message(${item})
endforeach(item ${FREERTOS_INCLUDE})


##################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
##########################################################   Build the micro ecc crypto   ##################################################################################

file(GLOB microecc_crypto_files  
${CMAKE_PROJECT_DIR}/watchx_lib_ext/micro-ecc/*.c
)


# source file 
set(MICROECC_CRYPTO_SOURCE 
        ${microecc_crypto_files}        
)

set(MICROECC_CRYPTO_INC 
${CMAKE_PROJECT_DIR}/watchx_lib_ext/micro-ecc
)


add_library(micro_ecc_Crypto_lib STATIC 
            ${MICROECC_CRYPTO_SOURCE}
            )



set(MICRO_ECC_CRYPTO_COMPILE_DEFS 
        -DFLOAT_ABI_HARD
        -DuECC_ENABLE_VLI_API=0
        -DuECC_OPTIMIZATION_LEVEL=3
        -DuECC_SQUARE_FUNC=0
        -DuECC_SUPPORT_COMPRESSED_POINT=0
        -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
)


target_compile_definitions(micro_ecc_Crypto_lib PUBLIC
        ${COMMON_DEFINES}   
        $<$<COMPILE_LANGUAGE:C>: ${MICRO_ECC_CRYPTO_COMPILE_DEFS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${MICRO_ECC_CRYPTO_COMPILE_DEFS} >
)

target_compile_options(micro_ecc_Crypto_lib PUBLIC
           ${COMMON_C_FLAGS}
           $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fomit-frame-pointer  >
           $<$<COMPILE_LANGUAGE:CXX>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti -fomit-frame-pointer>
           $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp >
             )


target_include_directories(micro_ecc_Crypto_lib PUBLIC SYSTEM 
           ${MICROECC_CRYPTO_INC}
           )


# get all the includes of the MICRO ECC INC 
message("================= the Micro ecc library has following includes dir=============================")
foreach(item ${MICROECC_CRYPTO_INC})
        message(${item})
endforeach(item ${MICROECC_CRYPTO_INC})

##################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
##########################################################   Build the tiny crypto   ##################################################################################

# file(GLOB tiny_crypto_files  
# ${CMAKE_PROJECT_DIR}/watchx_lib_ext/tinycrypt-master/tinycrypt-master/lib/source/*.c
# )


# # source file 
# set(TINY_CRYPTO_SOURCE 
#         ${tiny_crypto_files}        
# )

# set(TINY_CRYPTO_INC 
# ${CMAKE_PROJECT_DIR}/watchx_lib_ext/tinycrypt-master/tinycrypt-master/lib/include
# )


# add_library(tiny_Crypto STATIC 
#             ${TINY_CRYPTO_SOURCE}
#             )



# set(TINY_CRYPTO_COMPILE_DEFS 
# )


# target_compile_definitions(tiny_Crypto PRIVATE
#         ${COMMON_DEFINES}   
#         $<$<COMPILE_LANGUAGE:C>: ${TINY_CRYPTO_COMPILE_DEFS}>
#         $<$<COMPILE_LANGUAGE:ASM>: ${TINY_CRYPTO_COMPILE_DEFS} >
# )

# target_compile_options(tiny_Crypto PRIVATE
#            ${COMMON_C_FLAGS}
#            $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fomit-frame-pointer >
#            $<$<COMPILE_LANGUAGE:CXX>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti -fomit-frame-pointer>
#            $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp>
#              )


# target_include_directories(tiny_Crypto PUBLIC SYSTEM 
#            ${TINY_CRYPTO_INC}
#            )

##################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
##########################################################   Build the SDK HAL library   ##################################################################################

file(GLOB_RECURSE nrf_hal_libs_srcs 
        ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/*.c  
)

set(NRF_HAL_DIR_INC )


foreach(hal_libs ${nrf_hal_libs_srcs})

get_filename_component(hal_libs ${hal_libs} DIRECTORY)

list(APPEND NRF_HAL_DIR_INC ${hal_libs})
endforeach(hal_libs ${nrf_hal_libs_srcs})


list(REMOVE_DUPLICATES NRF_HAL_DIR_INC)

message("============================= NRF HAL Direcroties ========================================")

foreach(item ${NRF_HAL_DIR_INC})
message(${item})
endforeach(item ${NRF_HAL_DIR_INC})


set(NRF_HAL_SRC_FILES
        ${nrf_hal_libs_srcs}
    )

        # build the nrf sdk library 

        # make a library of external 
        add_library(nrf_hal_lib  OBJECT 
        ${SDK_SRC_FILES}
        ${NRF_HAL_SRC_FILES}
        )
        
        
        
        target_compile_definitions(nrf_hal_lib PUBLIC
            ${COMMON_DEFINES}   
        #     ${NIMBLE_STACK_DEFINES}
        
        )
        
        target_compile_options(nrf_hal_lib PUBLIC
               ${COMMON_C_FLAGS}
               $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized  >
               $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti >
               $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp >
        #        $<$< >: > 
                 )
        
        
        target_include_directories(nrf_hal_lib PUBLIC SYSTEM 
            ${SDK_INCLUDE_DIR}
            ${FREERTOS_INCLUDE}
            ${NRF_HAL_DIR_INC}

          )


# set(EXCLUDE_FILES_FOR_OPTIMISATION 
# ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/spi/spi_thread_safe.c
# ${CMAKE_SOURCE_DIR}/libs/nrf_HAL/spi/spi.c
# )
# # Set specific compiler flags for "special_file.cpp"
# set_source_files_properties(${EXCLUDE_FILES_FOR_OPTIMISATION}  PROPERTIES COMPILE_FLAGS "-O3")


###########################################################################################
############################################################################################
############################################################################################
############### Watchx Kernel library #############################################

file ( GLOB watchx_kernel_files 
        ${PROJECT_KERNEL_FOLDER}/*/*.c 
        ${PROJECT_KERNEL_FOLDER}/*/*/*.c
        )

set(WATCH_KERNEL_INCLUDE_DIR)

foreach(watchxkernel_srcs ${watchx_kernel_files})

get_filename_component(dirname ${watchxkernel_srcs} DIRECTORY)

list(APPEND WATCH_KERNEL_INCLUDE_DIR ${dirname})
endforeach(watchxkernel_srcs ${watchx_kernel_files})

# remove the duplicates directory from the includes 
list(REMOVE_DUPLICATES WATCH_KERNEL_INCLUDE_DIR)

message("================= watchx kernel dir are ==================================")

foreach(item ${WATCH_KERNEL_INCLUDE_DIR})
message(${item})
endforeach(item ${WATCH_KERNEL_INCLUDE_DIR})

set( WATCHX_KERNEL_SRC 
      ${watchx_kernel_files}
        )

        # make a library of external that are connected to the nrf52 
add_library(watchx_kernel_lib  STATIC 
${WATCHX_KERNEL_SRC}
)



target_compile_definitions(watchx_kernel_lib PUBLIC
    ${COMMON_DEFINES}   
#     ${NIMBLE_STACK_DEFINES}

)

target_compile_options(watchx_kernel_lib PUBLIC
       ${COMMON_C_FLAGS}
       $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized  >
       $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti >
       $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp >
         )


target_include_directories(watchx_kernel_lib PUBLIC SYSTEM 
    ${WATCH_KERNEL_INCLUDE_DIR}
    ${SDK_INCLUDE_DIR}
    ${FREERTOS_INCLUDE}
    ${NRF_HAL_DIR_INC}
        )


# we have to link this static library 
# target_link_libraries( watchx_kernel_lib
#         nrf_hal_lib
# )

# set(EXCLUDE_KERNEL_FILES_FOR_OPTIMISATION
# ${CMAKE_SOURCE_DIR}/libs/components/kernel/device_drivers/display/nrf_oled_driver.c
# )
# # Set specific compiler flags for "special_file.cpp"
# set_source_files_properties(${EXCLUDE_KERNEL_FILES_FOR_OPTIMISATION}  PROPERTIES COMPILE_FLAGS "-O3 -g0")

###################################################################################################
##################################################################################################
############################ Build the BLE library ####################################


file(GLOB_RECURSE ble_nordic_files 
        ${PROJECT_BLE_FOLDER}/*/*.c 
)


# this contains the directories of the ble library 
set(BLE_LIB_INCLUDE_DIR)

foreach(ble_sources ${ble_nordic_files})

get_filename_component(dirname ${ble_sources} DIRECTORY)

list(APPEND BLE_LIB_INCLUDE_DIR ${dirname})
endforeach(ble_sources ${ble_nordic_files})



# remove the duplicates directory from the includes 
list(REMOVE_DUPLICATES BLE_LIB_INCLUDE_DIR)

message("====================== BLE library dir are ========================== ")

foreach(item ${BLE_LIB_INCLUDE_DIR})
message(${item})
endforeach(item ${BLE_LIB_INCLUDE_DIR})


set(BLE_LIBS_SRC
        # this is the ble stack init files 
        ${ble_nordic_files}
        )


        # make a library of external 
add_library(ble_library STATIC 
${BLE_LIBS_SRC}
)


target_compile_definitions(ble_library PUBLIC
    ${COMMON_DEFINES}   
)

target_compile_options(ble_library PUBLIC
       ${COMMON_C_FLAGS}
       $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-return-type >
       $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-return-type -fno-rtti >
       $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp >
         )


target_include_directories(ble_library PUBLIC SYSTEM 
    ${BLE_LIB_INCLUDE_DIR}
    ${SDK_INCLUDE_DIR}
    ${NRF_HAL_DIR_INC}
    ${FREERTOS_INCLUDE} 
    
    # crypto library implementation 
#     ${TINY_CRYPTO_INC}
    ${MICROECC_CRYPTO_INC}
        )

# target_link_libraries(ble_library 
#                 freertos_lib)

###########################################################################################################################################################
##########################################################################################################################################################
#########################################################################################################################################################
############################################################## specify the main source files of your project here 

        # search for app files recursively 
        file ( GLOB_RECURSE main_app_files 
        ${CMAKE_SOURCE_DIR}/main/*.c 
        )


set(WATCH_APP_INCLUDE_DIR)

foreach(watchxapp_schd_srcs ${main_app_files})

get_filename_component(dirname ${watchxapp_schd_srcs} DIRECTORY)

list(APPEND WATCH_APP_INCLUDE_DIR ${dirname})
endforeach(watchxapp_schd_srcs ${main_app_files})

# remove the duplicates directory from the includes 
list(REMOVE_DUPLICATES WATCH_APP_INCLUDE_DIR)

message("============================== the watchx Main apps dir============================")
# display the content 
foreach(srcs ${WATCH_APP_INCLUDE_DIR})
message(${srcs})
endforeach(srcs ${WATCH_APP_INCLUDE_DIR})

set(PROJECT_FILES
        ${main_app_files}
)

set(PROJECT_INCLUDE_DIR
        ${WATCH_APP_INCLUDE_DIR}
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
             ${PROJECT_INCLUDE_DIR}
             ${WATCH_KERNEL_INCLUDE_DIR}
             ${FREERTOS_INCLUDE}
        )

        
target_compile_definitions(${EXECUTABLE} PRIVATE
    # specify the common definees     
    ${COMMON_DEFINES}

)


target_compile_options(${EXECUTABLE} PRIVATE
    
        # C flags common to all targets
        ${COMMON_C_FLAGS}
        ${RELEASE_Flags}

        )



target_link_libraries(${EXECUTABLE}
                       freertos_lib
                       ble_library
                       nrf_hal_lib 
                       watchx_kernel_lib
                #        tiny_Crypto
                        micro_ecc_Crypto_lib
                #        nimble_ble_stack
                #        gpio_lib # no library gpio_lib so cmake discard it 
                        )

                        # specify the linker options 
target_link_options(${EXECUTABLE} PRIVATE

        ${LINKER_FLAGS}
        -T${LINKER_SCRIPT_WITH_SD}
        )

     # set the target properties to make output file as .elf executable and linkable format 
set_target_properties(${EXECUTABLE}
        PROPERTIES SUFFIX  .elf
 )      


##################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
##########################################################   create extra files from the build hex and bin format  ##################################################################################

# ==============  add the custom directory to the ============================================
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests )

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


########################## these are Python app 
set(python_path 
        D:\\smart_watch\\watchx\\python)



# add_custom_target(merge_hex_files )
############################################################################################################################################################################
##############################################################################################################################################################################
############################################################################################################################################################################
##############################################################################################################################################################################
############################################################################################################################################################################
#########################################

# merge the binaries  things at once 
add_custom_target(mergebins
                COMMAND  echo mergining hex files BLE stack and Main Application
                COMMAND  echo using mergehex to merge differnt hex files 
                COMMAND  mergehex -m ${NRF_SDK_DIR}/components/softdevice/s132/hex/s132_nrf52_7.2.0_softdevice.hex ${PROJECT_NAME}.hex  -o ${PROJECT_NAME}_final.hex 
                COMMAND echo " the size of the Final FIrmware build is shown below :)"
                COMMAND  ${CMAKE_SIZE_UTIL} ${PROJECT_NAME}_final.hex 
                )

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

add_custom_target(flash_bin 
                COMMAND echo "flashing ${PROJECT_NAME}.elf into the nrf52832"
                COMMAND nrfjprog -f nrf52 --program ${PROJECT_NAME}_final.hex --sectorerase --verify
                COMMAND nrfjprog -f nrf52 --reset
                )