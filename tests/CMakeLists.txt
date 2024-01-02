# This Cmake script will add an extra target for the test programs
# will help us to made a test program and 


###3 add the Test executable in the current targets 


#  Define the sources for the Test executables
set(Executable_sources  
  ${CMAKE_PROJECT_DIR}/tests/nrf_gfx/python_gfx.c 
  ${CMAKE_PROJECT_DIR}/src/main/kernel_task.c
   )


set(TEST_EXECUTABLE test)

# define the includes of the Test Executable 
set(Executable_includes 
  ${CMAKE_PROJECT_DIR}/tests/nrf_gfx
  ${CMAKE_PROJECT_DIR}/src/main
)

    #### add the test target in the build. so we can have test files running as diff target  
add_executable(${TEST_EXECUTABLE}  EXCLUDE_FROM_ALL
  ${Executable_sources} )


  # add the compile definations for the test target 
target_compile_definitions(${TEST_EXECUTABLE} PUBLIC
                            ${COMMON_DEFINES}
                            )

# add the compiler options for the test target 
target_compile_options(${TEST_EXECUTABLE}  PUBLIC
              ${COMMON_C_FLAGS}
              $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized  >
              $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti >
              $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp >
  )

# add the include directories for the Test target 
target_include_directories(${TEST_EXECUTABLE}  PUBLIC SYSTEM 
    ${Executable_includes}  
    ${WATCH_KERNEL_INCLUDE_DIR}
    ${SDK_INCLUDE_DIR}
    ${FREERTOS_INCLUDE}
    ${BLE_LIB_INCLUDE_DIR}
    ) 

# link the test target 
target_link_libraries(${TEST_EXECUTABLE}
freertos_lib
ble_library
nrf_hal_lib 
watchx_kernel_lib
micro_ecc_Crypto_lib
#        tiny_Crypto
  )

target_link_options(${TEST_EXECUTABLE} PRIVATE
          ${LINKER_FLAGS}
        )
set_target_properties(${TEST_EXECUTABLE}
          PROPERTIES SUFFIX .elf
          )

# ======================== adding the custom target file ===========================

# Create hex file
add_custom_command(TARGET ${TEST_EXECUTABLE}
        POST_BUILD

        COMMAND echo  "Succesfully build the test program  :)"
        COMMAND echo  "BUilding Extra Files for test program  Hex, BIN "
        
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${TEST_EXECUTABLE}.elf   ${TEST_EXECUTABLE}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${TEST_EXECUTABLE}.elf  ${TEST_EXECUTABLE}.bin
        )

  #Print executable size
add_custom_command(TARGET ${TEST_EXECUTABLE}
            POST_BUILD
            COMMAND echo "The Size of your project is shown below :) "
            COMMAND ${CMAKE_SIZE_UTIL} ${TEST_EXECUTABLE}.elf
  )
      
### adding custom target to flash the test programs 

add_custom_target(flash_test        
COMMAND echo "flashing ${TEST_EXECUTABLE}.elf into the nrf52832"
COMMAND nrfjprog -f nrf52 --program ${TEST_EXECUTABLE}.hex --sectorerase --verify
COMMAND nrfjprog -f nrf52 --reset
)

#### ===============================================================================================

