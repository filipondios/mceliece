file(GLOB_RECURSE SOURCES "*.c")

add_executable(mceliece_tests ${SOURCES})
target_include_directories(mceliece_tests PRIVATE ../source)
target_link_libraries(mceliece_tests PRIVATE mceliece_lib check)

add_test(NAME mceliece_tests COMMAND mceliece_tests)
