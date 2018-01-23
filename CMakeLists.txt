cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (mhttp CXX)

# The version number.
set (mhttp_VERSION_MAJOR 0)
execute_process(COMMAND ./generateVersion.sh OUTPUT_VARIABLE mhttp_VERSION_MINOR)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -DVER=${mhttp_VERSION_MAJOR}.${mhttp_VERSION_MINOR}")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

FIND_PACKAGE( OpenMP)
  if(OPENMP_FOUND)
     message("OPENMP FOUND")
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  endif()
 include_directories(SYSTEM ${OpenMP_INCLUDE_PATH})

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_INSTALL_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/catch")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/fakeit")


file(GLOB_RECURSE mhttplib_SOURCES "src/*.cpp")
file(GLOB_RECURSE mhttptest_SOURCES "tests/*.cpp")
set(mhttp_SOURCES main.cpp)



add_library(mhttp SHARED ${mhttplib_SOURCES})
install(TARGETS mhttp DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/mhttp
        FILES_MATCHING PATTERN "*.hpp")

add_executable(mhttpapp ${mhttp_SOURCES})
target_link_libraries(mhttpapp mhttp)

add_executable(mhttp_tests EXCLUDE_FROM_ALL ${mhttptest_SOURCES} )
target_link_libraries(mhttp_tests mhttp)


enable_testing()

#add_test( mhttp_tests mhttp_tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
#add_custom_target(check 
#      COMMAND ${CMAKE_CTEST_COMMAND}
#      DEPENDS mhttp_tests)

add_test( NAME mhttp_tests 
          COMMAND "${CMAKE_BINARY_DIR}/mhttp_tests"
          WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"  )


add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS mhttp_tests)


SET(CPACK_PACKAGE_VERSION_MAJOR ${mhttp_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${mhttp_VERSION_MINOR})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_GENERATOR "DEB" "TGZ")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tadeusz Puźniakowski")
INCLUDE(CPack)


 
# To use this:
# make package

