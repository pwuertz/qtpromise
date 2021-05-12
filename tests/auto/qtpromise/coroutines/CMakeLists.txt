function(add_coroutine_tests)
    set(CMAKE_CXX_STANDARD 20)
    add_compile_definitions(QTPROMISE_CORO)
    qtpromise_add_test(coroutines SOURCES tst_coroutines.cpp)
endfunction()

if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND
        (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.3))
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        add_compile_options(-fcoroutines)
    endif()
    add_coroutine_tests()
endif()

if ((CMAKE_CXX_COMPILER_ID MATCHES "MSVC") AND
        (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.28.29913))
    add_coroutine_tests()
endif()
