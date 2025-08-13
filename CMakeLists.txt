cmake_minimum_required(VERSION 3.21)
project(mceliece VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Static library" FORCE)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c")
add_library(mceliece_lib STATIC ${SOURCES})

set_target_properties(mceliece_lib PROPERTIES
  OUTPUT_NAME mceliece
  POSITION_INDEPENDENT_CODE ON)

target_include_directories(mceliece_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

add_subdirectory(external/libsodium)
add_subdirectory(external/check)
target_link_libraries(mceliece_lib PRIVATE sodium)

enable_testing()
add_subdirectory(testing)

# Function to apply security flags to targets
function(apply_security_flags)
  foreach(TARGET ${ARGV})
    if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
      target_compile_options(${TARGET} PRIVATE 
        /GS /guard:cf /DYNAMICBASE /NXCOMPAT /W4 /wd4996
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

apply_security_flags(mceliece_lib)
set_property(TARGET sodium PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET mceliece_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
