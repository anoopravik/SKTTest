# Build App and Lib
cmake_minimum_required(VERSION 2.8)
project(application)
add_subdirectory(httpstats)
add_executable(application application/src/application.c)
include_directories(${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(application httpstats curl)
install(TARGETS application DESTINATION ${CMAKE_SOURCE_DIR}/install)

# Test Cases
enable_testing()
add_test(NAME TestCase1 COMMAND application --help)
add_test(NAME TestCase2 COMMAND application)
add_test(NAME TestCase3 COMMAND application -v)
add_test(NAME TestCase4 COMMAND application -n 10)
add_test(NAME TestCase5 COMMAND application -n 5 -H "Name:SamKNows")
add_test(NAME TestCase6 COMMAND application -n 5 -H "Name:SamKNows" -H "Process:SamKnowsTechnicalTest")
add_test(NAME TestCase7 COMMAND application -v -n 5 -H "Name:SamKNows")
add_test(NAME TestCase8 COMMAND application -v -n 5 -H "Name:SamKNows" -H "Process:SamKnowsTechnicalTest")
add_test(NAME TestCase9 COMMAND application -v -H "Name:SamKNows")

# Set test case properties
set (passRegex "SKTEST")
set (failRegex "ERROR" "Fail")
set_property (TEST TestCase2 TestCase3 TestCase4 TestCase5 TestCase6
              PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}")
set_property (TEST TestCase1 TestCase2 TestCase3 TestCase4 TestCase5 TestCase6 TestCase7 TestCase8
              PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")

set (passRegex "Name:SamKNows")
set_property (TEST TestCase7 TestCase9 
              PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}")

set (passRegex "Process:SamKnowsTechnicalTest")
set_property (TEST TestCase8 
              PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}")

# Negative Testcases
add_test(NAME TestCase10 COMMAND application -n 0)
add_test(NAME TestCase11 COMMAND application -H "Name:SamKNows")

set (passRegex "Name:SamKNows")
set_property (TEST TestCase11 
              PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}")

set_tests_properties(TestCase10 TestCase11 PROPERTIES WILL_FAIL TRUE)
