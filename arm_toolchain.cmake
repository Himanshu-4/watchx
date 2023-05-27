set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

set(TOOLCHAIN_PREFIX arm-none-eabi-)

execute_process(
  COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
  OUTPUT_VARIABLE BINUTILS_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# find the compiler path by specifying the name of the compiler 
get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)

message("the arm compiler is ${ARM_TOOLCHAIN_DIR}")
# give the root path of the toolchain
# set(Tools_path "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.07/bin/")




set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
 



set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)


# specify the additional tools of the toolchains to helkp cmake to build additional debug files
set(CMAKE_AR      ${TOOLCHAIN_PREFIX}ar)
set(CMAKE_RANLIB  ${TOOLCHAIN_PREFIX}ranlib)

set(CMAKE_OBJCOPY   ${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")


# specify the output suffix 
set(CMAKE_C_OUTPUT_EXTENSION      .o)
# set(CMAKE_EXECUTABLE_SUFFIX_C     .o)
set(CMAKE_EXECUTABLE_SUFFIX_ASM   .o)
set(CMAKE_EXECUTABLE_SUFFIX_CXX   .o)
set(CMAKE_EXECUTABLE_SUFFIX .elf)


set(CMAKE_FIND_ROOT_PATH ${ARM_TOOLCHAIN_DIR})

set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)


