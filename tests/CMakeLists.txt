# This Cmake script will add an extra target for the test programs
# will help us to made a test program and 


##===================================================================================================================
##======================= this is a upload program target ===========================================================
##===================================================================================================================

#  Define the sources for the Test executables
set(Executable_sources  
  ${CMAKE_PROJECT_DIR}/tests/nrf_gfx/python_gfx.c 
  ${CMAKE_PROJECT_DIR}/src/main/kernel_task.c
   )


set(PY_GFX_TOOL upload_tool)

# define the includes of the Test Executable 
set(Executable_includes 
  ${CMAKE_PROJECT_DIR}/tests/nrf_gfx
  ${CMAKE_PROJECT_DIR}/src/main
)

    #### add the test target in the build. so we can have test files running as diff target  
add_executable(${PY_GFX_TOOL}  EXCLUDE_FROM_ALL
  ${Executable_sources} )


  # add the compile definations for the test target 
target_compile_definitions(${PY_GFX_TOOL} PUBLIC
                            ${COMMON_DEFINES}
                            )

# add the compiler options for the test target 
target_compile_options(${PY_GFX_TOOL}  PUBLIC
              ${COMMON_C_FLAGS}
              $<$<COMPILE_LANGUAGE:C>:  -Wno-unused-but-set-variable -Wno-maybe-uninitialized  >
              $<$<COMPILE_LANGUAGE:CXX>: -Wno-unused-but-set-variable -Wno-maybe-uninitialized -fno-rtti >
              $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp >
  )

# add the include directories for the Test target 
target_include_directories(${PY_GFX_TOOL}  PUBLIC SYSTEM 
    ${Executable_includes}  
    ${WATCH_KERNEL_INCLUDE_DIR}
    ${SDK_INCLUDE_DIR}
    ${FREERTOS_INCLUDE}
    ${BLE_LIB_INCLUDE_DIR}
    ) 

# link the test target 
target_link_libraries(${PY_GFX_TOOL}
freertos_lib
ble_library
nrf_hal_lib 
watchx_kernel_lib
micro_ecc_Crypto_lib
#        tiny_Crypto
  )

target_link_options(${PY_GFX_TOOL} PRIVATE
          ${LINKER_FLAGS}
        )
set_target_properties(${PY_GFX_TOOL}
          PROPERTIES SUFFIX .elf
          )

# ======================== adding the custom target file ===========================

# Create hex file
add_custom_command(TARGET ${PY_GFX_TOOL}
        POST_BUILD

        COMMAND echo  "Succesfully build the test program  :)"
        COMMAND echo  "BUilding Extra Files for test program  Hex, BIN "
        
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${PY_GFX_TOOL}.elf   ${PY_GFX_TOOL}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${PY_GFX_TOOL}.elf  ${PY_GFX_TOOL}.bin
        )

  #Print executable size
add_custom_command(TARGET ${PY_GFX_TOOL}
            POST_BUILD
            COMMAND echo "The Size of your project is shown below :) "
            COMMAND ${CMAKE_SIZE_UTIL} ${PY_GFX_TOOL}.elf
  )
      
### adding custom target to flash the test programs 

add_custom_target(flash_py_tool        
COMMAND echo "flashing ${TEST_EXECUTABLE}.elf into the nrf52832"
COMMAND nrfjprog -f nrf52 --program ${PY_GFX_TOOL}.hex --sectorerase --verify
COMMAND nrfjprog -f nrf52 --reset
)

#### ===============================================================================================

##===================================================================================================================
##======================= this is a program test target ===========================================================
##===================================================================================================================
###------------------ you should run it when u want to test something ------------------------------------------

set(TEST_DIRECTORY  ${CMAKE_PROJECT_DIR}/tests/test)
set(TEST_INCLUDES_DIR ${TEST_DIRECTORY})
set(TEST_SOURCE_FILES)

# search for app files recursively 
file ( GLOB_RECURSE TEST_SOURCE_FILES 
    ${TEST_DIRECTORY}/*.c 
)

list(APPEND TEST_INCLUDES_DIR 
    ${CMAKE_PROJECT_DIR}/src/main )
    
# list(APPEND TEST_SOURCE_FILES 
# ${CMAKE_PROJECT_DIR}/src/main/kernel_task.c 
# )

# foreach(test_srcs ${test_src_Files})

# get_filename_component(dirname ${test_srcs} DIRECTORY)

# list(APPEND WATCH_APP_INCLUDE_DIR ${dirname})

# endforeach(test_srcs ${test_src_Files})

# remove the duplicates directory from the includes 
# list(REMOVE_DUPLICATES WATCH_APP_INCLUDE_DIR)

message("============================== Test Files are listed below============================")
# display the content 
foreach(srcs ${TEST_SOURCE_FILES})
message(${srcs})
endforeach(srcs ${TEST_SOURCE_FILES})



set(TEST_EXECUTABLE my_test)


    #### add the test target in the build. so we can have test files running as diff target  
add_executable(${TEST_EXECUTABLE}  EXCLUDE_FROM_ALL
  ${TEST_SOURCE_FILES} )


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
    ${TEST_INCLUDES_DIR}  
    ${WATCH_KERNEL_INCLUDE_DIR}
    ${SDK_INCLUDE_DIR}
    ${FREERTOS_INCLUDE}
    ) 

# link the test target 
target_link_libraries(${TEST_EXECUTABLE}
freertos_lib
nrf_hal_lib 
watchx_kernel_lib
#        tiny_Crypto
  )

target_link_options(${TEST_EXECUTABLE} PRIVATE
          ${LINKER_FLAGS}
          -T${LINKER_SCRIPT_WITHOUT_SD}
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

