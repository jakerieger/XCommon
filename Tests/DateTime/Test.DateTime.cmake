add_executable(Test.DateTime
    ${TESTS_DIR}/DateTime/Test.DateTime.cpp
)

target_link_libraries(Test.DateTime PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(Test.DateTime)