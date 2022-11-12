cmake_minimum_required(VERSION 3.7)
project(openrfsense-sensor)

# Set version and other metadata
set(VERSION "1.0.0")
set(DEBVERSION "1")
add_definitions(-DAPP_VERSION="${VERSION}-${DEBVERSION}")
string(TIMESTAMP TIME_COMPILATION "%Y-%m-%d %H:%M")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -std=gnu++11 -O3 ")

# Add the local Find*.cmake scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Find dependencies
find_package(RTLSDR REQUIRED)
include_directories("${RTLSDR_INCLUDE_DIR}")

find_package(OpenSSL REQUIRED)
include_directories("OPENSSL_INCLUDE_DIR}")

find_package(libusb-1.0 REQUIRED)
find_package(Liquid REQUIRED)
find_package(JSON-C REQUIRED)
find_package(crypto REQUIRED)
find_package(Avro REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibLZMA REQUIRED)

# Generate version header
configure_file(version_config.h.in
               ${CMAKE_CURRENT_LIST_DIR}/generated/version_config.h)

# ADD_SUBDIRECTORY(drivers/rtlsdr/converter/)

set(SOURCE_FILES
    main.cpp
    drivers/Driver.h
    drivers/rtlsdr/rtlsdrDriver.cpp
    drivers/rtlsdr/rtlsdrDriver.h
    drivers/rtlsdr/converter/converter.c
    drivers/rtlsdr/converter/converter.h
    context/OpenRFSenseContext.cpp
    context/OpenRFSenseContext.h
    drivers/common/SequentialHopping.cpp
    drivers/common/SequentialHopping.h
    types/SpectrumSegment.cpp
    types/SpectrumSegment.h
    ProcessingBlocks/RemoveDC.cpp
    ProcessingBlocks/RemoveDC.h
    ProcessingBlocks/RemoveDCRTL.cpp
    ProcessingBlocks/Windowing.cpp
    ProcessingBlocks/Windowing.h
    ProcessingBlocks/FFT.cpp
    ProcessingBlocks/FFT.h
    ProcessingBlocks/Averaging.cpp
    ProcessingBlocks/Averaging.h
    MiscBlocks/FileSink.cpp
    MiscBlocks/FileSink.h
    MiscBlocks/AvroSerialization.cpp
    MiscBlocks/AvroSerialization.h
    MiscBlocks/Transmission.cpp
    MiscBlocks/Transmission.h
    misc/TLS.cpp
    misc/TLS.h
    misc/TCP.cpp
    misc/TCP.h
    MiscBlocks/IQSink.cpp
    MiscBlocks/IQSink.h
    ProcessingBlocks/PSDFast.cpp
    ProcessingBlocks/PSDFast.h
    MiscBlocks/IQStream.cpp
    MiscBlocks/IQStream.h
    MiscBlocks/Filter/FIR/FIRDesign.cpp
    MiscBlocks/Filter/Resampler/Downsampler.cpp)

# Executable and linking
add_executable(orfs_sensor ${SOURCE_FILES})
target_link_libraries(
  orfs_sensor
  pthread
  dl
  ${RTLSDR_LIBRARY}
  ${LIBUSB_1_LIBRARIES}
  ${LIQUID_LIBRARIES}
  ${AVRO_LIBRARY}
  ${ZLIB_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${CRYPTO_LIBRARIES}
  ${JSON-C_LIBRARIES})

# Installation directories
install(TARGETS orfs_sensor DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sample.avsc
        DESTINATION openrfsense/)

# Determine current architecture
macro(dpkg_arch VAR_NAME)
  find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
  if(DPKG_PROGRAM)
    execute_process(
      COMMAND ${DPKG_PROGRAM} --print-architecture
      OUTPUT_VARIABLE ${VAR_NAME}
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif(DPKG_PROGRAM)
endmacro(dpkg_arch)

# set (CPACK_DEBIAN_PACKAGE_DEPENDS "libfftw3-3, zlib1g, liblzma5,
# libusb-1.0-0")

# CPack data for distro package generation
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "DEB")
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_SET_DESTDIR "on")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/")

set(PKG_VERSION ${VERSION}-${DEBVERSION})
# CPack version numbers for release tarball name.
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 2)
set(CPACK_DEBIAN_PACKAGE_VERSION ${PKG_VERSION})

set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
set(CPACK_DEBIAN_PACKAGE_SECTION "net")
dpkg_arch(CPACK_DEBIAN_PACKAGE_ARCHITECTURE)

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/postinst"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/postrm")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Sensor manager for OpenRFSense-enabled sensors")
set(CPACK_PACKAGE_DESCRIPTION
    "OpenRFSense is an experimental software suite to explore the RF speectrum using remote devices and a centralized server."
)

set(CPACK_PACKAGE_CONTACT "Leonardo Baldin")
set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}_${PKG_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
set(CPACK_DEBIAN_PACKAGE_PROVIDES openrfsense-run)

set(CPACK_COMPONENTS_ALL Libraries ApplicationData)
include(CPack Documentation)
