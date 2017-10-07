cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (mhttp CXX)

# The version number.
set (mhttp_VERSION_MAJOR 0)
execute_process(COMMAND ./generateVersion.sh OUTPUT_VARIABLE mhttp_VERSION_MINOR)

add_definitions(-std=c++11)
add_definitions(-Wall)
add_definitions(-DVER=${mhttp_VERSION_MAJOR}.${mhttp_VERSION_MINOR})

FIND_PACKAGE( OpenMP)
  if(OPENMP_FOUND)
     message("OPENMP FOUND")
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  endif()
 include_directories(SYSTEM ${OpenMP_INCLUDE_PATH})

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/catch")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/fakeit")


file(GLOB_RECURSE mhttplib_SOURCES "src/*.cpp")
file(GLOB_RECURSE mhttptest_SOURCES "tests/*.cpp")
set(mhttp_SOURCES main.cpp)



# add_library(mhttp ${mhttplib_SOURCES})
add_library(mhttp SHARED ${mhttplib_SOURCES})
install(TARGETS mhttp DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/mhttp
        FILES_MATCHING PATTERN "*.hpp")

add_executable(mhttpapp ${mhttp_SOURCES})
target_link_libraries(mhttpapp mhttp)

add_executable(mhttp_tests ${mhttptest_SOURCES})
target_link_libraries(mhttp_tests mhttp)


enable_testing()

add_test( mhttp_tests mhttp_tests )

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tadeusz Puźniakowski")
 
INCLUDE(CPack)
 
# To use this:
# make package

