# This Cmake script will add an extra target for the test programs
# will help us to made a test program and 


###3 add the Test executable in the current targets 

set(Executable_sources  
    )


    #### add the test target in the build. so we can have test files running as diff target  
add_executable(Test EXCLUDE_FROM_ALL
  Executable_sources )

  enable_testing()

  add_test()

#### ===============================================================================================

