include(CTest)

set(TESTS

    testllcpp-heap tests/heap.cpp
    testllcpp-orderbook tests/orderbook.cpp

)

while(TESTS)
    list(POP_FRONT TESTS test_name test_file)

    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE llcpp)

    add_test(NAME ${test_name} COMMAND ${test_name})
endwhile()