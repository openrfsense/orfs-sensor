# Find the OpenSSL includes and libraries
#
# The following variables are set if OpenSSL is found. If it is not found,
# OpenSSL_FOUND is set to false.
#
# OPENSSL_FOUND            - True when OpenSSL is found OPENSSL_INCLUDE_DIRS -
# The path to where the OpenSSL include files are located OPENSSL_LIBRARIES -
# The libraries to link to OPENSSL_VERSION_MAJOR    - The major version of the
# library (e.g. 0 for 0.9.3) OPENSSL_VERSION_MINOR    - The minor version of the
# library (e.g. 9 for 0.9.3) OPENSSL_VERSION_PATCH    - The patch/subminor
# version of the library (e.g. 3 for 0.9.3)
#
# To specify an additional directory to search, set OPENSSL_ROOT.
#
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org> Modified and
# extended by Siddhartha Chaudhuri, 2008
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

set(OPENSSL_FOUND FALSE)

find_path(
  OPENSSL_INCLUDE_DIRS openssl/ssl.h
  PATHS ${OPENSSL_ROOT} ${OPENSSL_ROOT}/include
  NO_DEFAULT_PATH)
if(NOT OPENSSL_INCLUDE_DIRS)
  find_path(OPENSSL_INCLUDE_DIRS openssl/ssl.h)
endif(NOT OPENSSL_INCLUDE_DIRS)

if(OPENSSL_INCLUDE_DIRS)
  if(WIN32)

    find_library(
      SSL_EAY_DEBUG
      NAMES ssleay32MDd
      PATH_SUFFIXES "" Debug
      PATHS ${OPENSSL_ROOT} ${OPENSSL_ROOT}/lib
      NO_DEFAULT_PATH)
    if(NOT SSL_EAY_DEBUG) # now look in system locations
      find_library(
        SSL_EAY_DEBUG
        NAMES ssleay32MDd
        PATH_SUFFIXES "" Debug)
    endif(NOT SSL_EAY_DEBUG)

    find_library(
      SSL_EAY_RELEASE
      NAMES ssleay32MD ssl ssleay32
      PATH_SUFFIXES "" Release
      PATHS ${OPENSSL_ROOT} ${OPENSSL_ROOT}/lib
      NO_DEFAULT_PATH)
    if(NOT SSL_EAY_RELEASE) # now look in system locations
      find_library(
        SSL_EAY_RELEASE
        NAMES ssleay32MD ssl ssleay32
        PATH_SUFFIXES "" Release)
    endif(NOT SSL_EAY_RELEASE)

    find_library(
      LIB_EAY_DEBUG
      NAMES libeay32MDd
      PATH_SUFFIXES "" Debug
      PATHS ${OPENSSL_ROOT} ${OPENSSL_ROOT}/lib
      NO_DEFAULT_PATH)
    if(NOT LIB_EAY_DEBUG) # now look in system locations
      find_library(
        LIB_EAY_DEBUG
        NAMES libeay32MDd
        PATH_SUFFIXES "" Debug)
    endif(NOT LIB_EAY_DEBUG)

    find_library(
      LIB_EAY_RELEASE
      NAMES libeay32MD libeay32
      PATH_SUFFIXES "" Release
      PATHS ${OPENSSL_ROOT} ${OPENSSL_ROOT}/lib
      NO_DEFAULT_PATH)
    if(NOT LIB_EAY_RELEASE) # now look in system locations
      find_library(
        LIB_EAY_RELEASE
        NAMES libeay32MD libeay32
        PATH_SUFFIXES "" Release)
    endif(NOT LIB_EAY_RELEASE)

    set(SSL_EAY_LIBRARY)
    if(SSL_EAY_DEBUG AND SSL_EAY_RELEASE)
      set(SSL_EAY_LIBRARY debug ${SSL_EAY_DEBUG} optimized ${SSL_EAY_RELEASE})
    elseif(SSL_EAY_DEBUG)
      set(SSL_EAY_LIBRARY ${SSL_EAY_DEBUG})
    elseif(SSL_EAY_RELEASE)
      set(SSL_EAY_LIBRARY ${SSL_EAY_RELEASE})
    endif(SSL_EAY_DEBUG AND SSL_EAY_RELEASE)

    set(LIB_EAY_LIBRARY)
    if(LIB_EAY_DEBUG AND LIB_EAY_RELEASE)
      set(LIB_EAY_LIBRARY debug ${LIB_EAY_DEBUG} optimized ${LIB_EAY_RELEASE})
    elseif(LIB_EAY_DEBUG)
      set(LIB_EAY_LIBRARY ${LIB_EAY_DEBUG})
    elseif(LIB_EAY_RELEASE)
      set(LIB_EAY_LIBRARY ${LIB_EAY_RELEASE})
    endif(LIB_EAY_DEBUG AND LIB_EAY_RELEASE)

    set(OPENSSL_LIBRARIES)
    if(SSL_EAY_LIBRARY AND LIB_EAY_LIBRARY)
      set(OPENSSL_LIBRARIES ${SSL_EAY_LIBRARY} ${LIB_EAY_LIBRARY})
    endif(SSL_EAY_LIBRARY AND LIB_EAY_LIBRARY)

    mark_as_advanced(SSL_EAY_DEBUG SSL_EAY_RELEASE LIB_EAY_DEBUG
                     LIB_EAY_RELEASE)

  else(WIN32)

    find_library(
      OPENSSL_LIBRARIES
      NAMES ssl ssleay32 ssleay32MD
      PATHS ${OPENSSL_ROOT} ${OPENSSL_ROOT}/lib
      NO_DEFAULT_PATH)
    if(NOT OPENSSL_LIBRARIES) # now look in system locations
      find_library(OPENSSL_LIBRARIES NAMES ssl ssleay32 ssleay32MD)
    endif(NOT OPENSSL_LIBRARIES)

    # The Crypto library is not always included in the list of SSL libraries
    find_library(
      CRYPTO_LIBRARY
      NAMES crypto
      PATHS ${OPENSSL_ROOT} ${OPENSSL_ROOT}/lib
      NO_DEFAULT_PATH)
    if(NOT CRYPTO_LIBRARY) # now look in system locations
      find_library(CRYPTO_LIBRARY NAMES crypto)
    endif(NOT CRYPTO_LIBRARY)

    if(NOT CRYPTO_LIBRARY)
      message(FATAL_ERROR "OpenSSL Crypto library missing")
    else(NOT CRYPTO_LIBRARY)
      set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} ${CRYPTO_LIBRARY})
    endif(NOT CRYPTO_LIBRARY)

  endif(WIN32)

  if(OPENSSL_LIBRARIES)
    set(OPENSSL_FOUND TRUE)
  endif(OPENSSL_LIBRARIES)
endif(OPENSSL_INCLUDE_DIRS)

if(OPENSSL_FOUND)
  if(OPENSSL_FOUND)
    set(OPENSSL_VERSION)
    set(OPENSSL_LIB_VERSION)
    file(READ "${OPENSSL_INCLUDE_DIRS}/openssl/opensslv.h" _OPENSSLV_H_CONTENTS)

    string(REGEX MATCH "#define[ \t]+OPENSSL_VERSION_NUMBER[ \t]+0x[0-9]+"
                 OPENSSL_VERSION "${_OPENSSLV_H_CONTENTS}")
    string(REGEX REPLACE ".*0x([0-9][0-9][0-9][0-9][0-9]).*" "\\1"
                         OPENSSL_VERSION "${OPENSSL_VERSION}")

    string(REGEX MATCH "#define[ \t]+SHLIB_VERSION_NUMBER[ \t]+\"[0-9.]+"
                 OPENSSL_LIB_VERSION "${_OPENSSLV_H_CONTENTS}")
    string(REGEX MATCH "[0-9.]+$" OPENSSL_LIB_VERSION "${OPENSSL_LIB_VERSION}")

    set(OPENSSL_VERSION
        ${OPENSSL_VERSION}
        CACHE INTERNAL "The version number for the OpenSSL library")
    set(OPENSSL_LIB_VERSION
        ${OPENSSL_LIB_VERSION}
        CACHE INTERNAL
              "The shared library version string for the OpenSSL library")

    if(OPENSSL_VERSION)
      math(EXPR OPENSSL_VERSION_MAJOR "${OPENSSL_VERSION} / 10000")
      math(EXPR OPENSSL_VERSION_MINOR "${OPENSSL_VERSION} / 100 % 100")
      math(EXPR OPENSSL_VERSION_PATCH "${OPENSSL_VERSION} % 100")
    endif(OPENSSL_VERSION)
  endif(OPENSSL_FOUND)
endif(OPENSSL_FOUND)

if(OPENSSL_FOUND)
  if(NOT OPENSSL_FIND_QUIETLY)
    message(
      STATUS
        "Found OpenSSL: headers at ${OPENSSL_INCLUDE_DIRS}, libraries at ${OPENSSL_LIBRARIES}"
    )
  endif(NOT OPENSSL_FIND_QUIETLY)
else(OPENSSL_FOUND)
  # IF(OPENSSL_FIND_REQUIRED)
  message(FATAL_ERROR "Could not find OpenSSL")
  # ENDIF(OPENSSL_FIND_REQUIRED)
endif(OPENSSL_FOUND)

mark_as_advanced(OPENSSL_INCLUDE_DIR OPENSSL_LIBRARIES OPENSSL_VERSION
                 OPENSSL_LIB_VERSION)
