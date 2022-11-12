# AVRO_FOUND - true if library and headers were found AVRO_INCLUDE_DIRS -
# include directories AVRO_LIBRARIES - library directories

find_package(PkgConfig)
pkg_check_modules(PC_AVRO QUIET json-c)

find_path(AVRO_INCLUDE_DIR avro.h /usr/pkgs64/include /usr/include
          /usr/local/include)

find_library(
  AVRO_LIBRARY
  NAMES avro
  PATHS /usr/lib/x86_64-linux-gnu/
        /usr/pkgs64/lib
        /usr/lib64
        /usr/lib
        /usr/local/lib
        /usr/lib/arm-linux-gnueabihf/
        /usr/lib/aarch64-linux-gnu
  NO_DEFAULT_PATH)

if(AVRO_LIBRARY AND AVRO_INCLUDE_DIR)
  set(AVRO_LIBRARIES ${AVRO_LIBRARY})
  set(AVRO_FOUND "YES")
else(AVRO_LIBRARY AND AVRO_INCLUDE_DIR)
  set(AVRO_FOUND "NO")
endif(AVRO_LIBRARY AND AVRO_INCLUDE_DIR)

if(AVRO_FOUND)
  if(NOT AVRO_FIND_QUIETLY)
    message(STATUS "Found AVRO: ${AVRO_LIBRARIES}")
  endif(NOT AVRO_FIND_QUIETLY)
else(AVRO_FOUND)
  if(AVRO_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find AVRO library")
  endif(AVRO_FIND_REQUIRED)
endif(AVRO_FOUND)

mark_as_advanced(AVRO_LIBRARY AVRO_INCLUDE_DIR)
