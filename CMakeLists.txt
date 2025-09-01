cmake_minimum_required(VERSION 3.10)

#Set the project name
project(dmxSerial)

#Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (VERBOSE 1)

# Add the executable
add_executable(dmxSerial src/main.cpp)

# Add the libftdi subdirectory
# set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/libftdi/cmake)
# set(STATICLIBS ON CACHE BOOL "Build static libraries for libftdi")
# add_subdirectory(${CMAKE_SOURCE_DIR}/libs/libftdi)
include (${CMAKE_SOURCE_DIR}/libs/libftdi/install/lib/cmake/libftdi1/LibFTDI1Config.cmake)
include (${CMAKE_SOURCE_DIR}/libs/libftdi/install/lib/cmake/libftdi1/UseLibFTDI1.cmake)
target_link_directories(dmxSerial PUBLIC ${LIBFTDI_LIBRARY_DIRS})
target_link_libraries(dmxSerial ${LIBFTDI_STATIC_LIBRARIES} )
message (">>> stat :"${LIBFTDI_STATIC_LIBRARIES})
message (">>> dir :"${LIBFTDI_LIBRARY_DIRS})



include_directories(libs/libftdi/install/include/libftdi1/)

# Link libraries
if(APPLE)
    target_link_libraries(dmxSerial
        # ${CMAKE_SOURCE_DIR}/libs/libftdi/install/lib/libftdi1.a
        # /opt/homebrew/lib/libusb-1.0.a
        "-framework Security"
        "-framework CoreFoundation"
        "-framework IOKit"
    )
elseif(UNIX)
    
endif()
