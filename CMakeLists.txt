cmake_minimum_required(VERSION 3.21)
project(mceliece VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "source/*.c")
file(GLOB_RECURSE HEADERS "source/*.h")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/main.c")
add_library(mceliece_lib ${SOURCES} ${HEADERS})
add_executable(mceliece "source/main.c")
target_link_libraries(mceliece PRIVATE mceliece_lib)

function(apply_security_flags)
  foreach(TARGET ${ARGV})
    if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
      target_compile_options(${TARGET} PRIVATE 
        /GS /guard:cf /DYNAMICBASE /NXCOMPAT /HIGHENTROPYVA /W4 /wd4996
        $<$<CONFIG:Debug>:/Zi /Od /DDEBUG>
        $<$<CONFIG:Release>:/O2 /DNDEBUG /WX>
      )
    else()
      target_compile_options(${TARGET} PRIVATE
        -fstack-protector-strong
        -fstack-clash-protection
        -D_FORTIFY_SOURCE=2 
        -D_GLIBCXX_ASSERTIONS
        -fcf-protection=full
        -fPIE
        -Wextra
        -Wpedantic 
        -Wcast-align
        -Wconversion
        -Wunused
        -Wuninitialized
        -fno-strict-aliasing 
        -fwrapv
        -fno-strict-overflow
        -fno-builtin-memset
        -fno-builtin-memcpy
        $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
      )
      target_link_options(${TARGET} PRIVATE
        -Wl,-z,relro,-z,now -Wl,-z,noexecstack -Wl,--disable-new-dtags
      )
    endif()
  endforeach()
endfunction()

apply_security_flags(mceliece_lib mceliece)
set_property(TARGET mceliece_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET mceliece PROPERTY POSITION_INDEPENDENT_CODE ON)
