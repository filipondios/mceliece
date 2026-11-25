find_package(Catch2 3 REQUIRED)
add_executable(mceliece_tests
  test_bit_matrix.cpp
  test_gauss.cpp
  test_keygen.cpp
  test_mceliece.cpp)
target_link_libraries(mceliece_tests PRIVATE mceliece Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(mceliece_tests)
