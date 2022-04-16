cmake_minimum_required(VERSION 3.16)

project(SPWL VERSION "0.8.0" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(${PROJECT_NAME}_SOURCES
        lib/src/SPWL.cpp
        )

add_library(${PROJECT_NAME}
        STATIC
        ${${PROJECT_NAME}_SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE
        "cxx_std_17"
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        )

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ./lib/include/
)

## Testing
enable_testing()
add_executable(
       unit_tests
       test/src/SPWL-test.cpp
       ${${PROJECT_NAME}_SOURCES})
target_include_directories(
        unit_tests
        PRIVATE
        ./test/include/
        ./test/cute
)

target_compile_features(unit_tests PUBLIC
        cxx_std_17
        )

set_target_properties(unit_tests PROPERTIES
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )
target_compile_options(unit_tests PRIVATE -g -O0 -fprofile-arcs -ftest-coverage -fPIC)
target_link_libraries(
        unit_tests
        gcov
)
# End Testing