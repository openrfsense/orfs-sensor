# * Find RTLSDR Find the native RTLSDR includes and library This module defines
#   RTLSDR_INCLUDE_DIR, where to find rtlsdr.h, etc. RTLSDR_LIBRARIES, the
#   libraries needed to use RTLSDR. RTLSDR_FOUND, If false, do not try to use
#   RTLSDR. also defined, but not for general use are RTLSDR_LIBRARY, where to
#   find the RTLSDR library.

# MESSAGE("RTLSDR_DIR set to ${RTLSDR_DIR}" )

find_path(RTLSDR_INCLUDE_DIR rtl-sdr.h ${RTLSDR_DIR}/include
          /usr/pkgs64/include /usr/include /usr/local/include)

find_library(
  RTLSDR_LIBRARY
  NAMES rtlsdr
  PATHS ${RTLSDR_DIR}/libs
        "${RTLSDR_DIR}\\win32\\lib"
        /usr/pkgs64/lib
        /usr/lib64
        /usr/lib
        /usr/local/lib
        /usr/lib/arm-linux-gnueabihf/
        /usr/lib/x86_64-linux-gnu/
        /usr/lib/aarch64-linux-gnu
  NO_DEFAULT_PATH)

if(RTLSDR_LIBRARY AND RTLSDR_INCLUDE_DIR)
  set(RTLSDR_LIBRARIES ${RTLSDR_LIBRARY})
  set(RTLSDR_FOUND "YES")
else(RTLSDR_LIBRARY AND RTLSDR_INCLUDE_DIR)
  set(RTLSDR_FOUND "NO")
endif(RTLSDR_LIBRARY AND RTLSDR_INCLUDE_DIR)

if(RTLSDR_FOUND)
  if(NOT RTLSDR_FIND_QUIETLY)
    message(STATUS "Found RTLSDR: ${RTLSDR_LIBRARIES}")
  endif(NOT RTLSDR_FIND_QUIETLY)
else(RTLSDR_FOUND)
  if(RTLSDR_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find RTLSDR library")
  endif(RTLSDR_FIND_REQUIRED)
endif(RTLSDR_FOUND)

# Deprecated declarations.
get_filename_component(NATIVE_RTLSDR_LIB_PATH ${RTLSDR_LIBRARY} PATH)

mark_as_advanced(RTLSDR_LIBRARY RTLSDR_INCLUDE_DIR)
