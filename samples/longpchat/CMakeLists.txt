cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (webapidemo CXX)

set (webapidemo_VERSION_MAJOR 0)
set (webapidemo_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include(ExternalProject)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

ExternalProject_Add(mhttp
    GIT_REPOSITORY https://github.com/pantadeusz/mhttp
    #GIT_TAG "050a4b4"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    BUILD_BYPRODUCTS ${EXTERNAL_INSTALL_LOCATION}/lib/libmhttp.so
    DOWNLOAD_NO_PROGRESS 1
)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE Debug)
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


FIND_PACKAGE( OpenMP )
  if(OPENMP_FOUND)
     message("OPENMP FOUND")
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  endif()
 include_directories(SYSTEM ${OpenMP_INCLUDE_PATH})


include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${EXTERNAL_INSTALL_LOCATION}/include")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/catch")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/json")

file(GLOB_RECURSE app_SOURCES "src/*.cpp")

add_executable(webapidemo ${app_MAIN} ${app_SOURCES} )
	# "${PROJECT_SOURCE_DIR}/thirdparty/lodepng/lodepng.cpp" 

add_dependencies(webapidemo  mhttp)

target_link_libraries(webapidemo 
    ${EXTERNAL_INSTALL_LOCATION}/lib/libmhttp.so 
    )

SET(CPACK_PACKAGE_VERSION_MAJOR ${webapidemo_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${webapidemo_VERSION_MINOR})
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_GENERATOR "DEB" "TGZ")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tadeusz Puźniakowski")
INCLUDE(CPack)


 
# To use this:
# make package


