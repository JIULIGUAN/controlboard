include(CMakeForceCompiler)

if (EXISTS ${PROJECT_SOURCE_DIR}/project/$ENV{CFG_PROJECT}/config.cmake)
    include(project/${CMAKE_PROJECT_NAME}/config.cmake)
else()
    include($ENV{CMAKE_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/$ENV{CFG_PROJECT}/config.cmake)
endif()

# use ccache
#find_program(CCACHE_FOUND ccache)
#if(CCACHE_FOUND)
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
#endif(CCACHE_FOUND)

# specify the cross compiler
if (DEFINED CFG_CPU_FA626)
    if (DEFINED ENV{DISTCC_HOSTS})
        CMAKE_FORCE_C_COMPILER("distcc;arm-none-eabi-gcc" GNU 4)
        CMAKE_FORCE_CXX_COMPILER("distcc;arm-none-eabi-g++" GNU)
        set(_CMAKE_TOOLCHAIN_PREFIX arm-none-eabi-)
    else()
        CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU 4)
        CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)
    endif()
    set(CPP     arm-none-eabi-cpp)
    set(OBJCOPY arm-none-eabi-objcopy)
    set(OBJDUMP arm-none-eabi-objdump)
    set(READELF arm-none-eabi-readelf)
    set(ADDR2LINE arm-none-eabi-addr2line)
    set(GDB arm-none-eabi-gdb)
    set(INSIGHT arm-none-eabi-insight)
elseif (DEFINED CFG_CPU_SM32)
    if (DEFINED ENV{DISTCC_HOSTS})
        CMAKE_FORCE_C_COMPILER("distcc;sm32-elf-gcc" GNU 4)
        CMAKE_FORCE_CXX_COMPILER("distcc;sm32-elf-g++" GNU)
        set(_CMAKE_TOOLCHAIN_PREFIX sm32-elf-)
    else()
        CMAKE_FORCE_C_COMPILER(     sm32-elf-gcc GNU 4)
        CMAKE_FORCE_CXX_COMPILER(   sm32-elf-g++ GNU)
    endif()
    set(CPP sm32-elf-cpp)
    set(OBJCOPY sm32-elf-objcopy)
    set(OBJDUMP sm32-elf-objdump)
    set(READELF sm32-elf-readelf)
    set(ADDR2LINE sm32-elf-addr2line)
    set(GDB sm32-elf-gdb)
    set(INSIGHT sm32-elf-insight)
endif()

# Get GCC version
execute_process(COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ARG1} -dumpversion
                OUTPUT_VARIABLE GCC_VERSION)

# this one is important
set(CMAKE_SYSTEM_NAME Generic)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH  C:/ITEGCC)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
