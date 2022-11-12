if(SSL_EAY_DEBUG AND SSL_EAY_RELEASE)
  set(LIB_FOUND 1)
endif(SSL_EAY_DEBUG AND SSL_EAY_RELEASE)

if(APPLE)
  find_path(
    CRYPTO_INCLUDE_DIR
    NAMES openssl/crypto.h
    PATHS /usr/local/opt/openssl/include
    NO_DEFAULT_PATH)
  find_library(
    CRYPTO_LIBRARIES
    NAMES crypto
    PATHS /usr/local/opt/openssl/lib
    NO_DEFAULT_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_CMAKE_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
    CMAKE_FIND_FRAMEWORK
    NEVER)
else()
  find_path(CRYPTO_INCLUDE_DIR NAMES openssl/crypto.h)
  find_library(CRYPTO_LIBRARIES NAMES crypto libcrypto)
endif(APPLE)

if(CRYPTO_INCLUDE_DIR AND CRYPTO_LIBRARIES)
  set(CRYPTO_FOUND TRUE)
else(CRYPTO_INCLUDE_DIR AND CRYPTO_LIBRARIES)
  set(CRYPTO_FOUND FALSE)
endif(CRYPTO_INCLUDE_DIR AND CRYPTO_LIBRARIES)

if(CRYPTO_FOUND)
  if(NOT Crypto_FIND_QUIETLY)
    message(
      STATUS "Found libcrypto: ${CRYPTO_LIBRARIES}, ${CRYPTO_INCLUDE_DIR}")
  endif(NOT Crypto_FIND_QUIETLY)
else(CRYPTO_FOUND)
  if(Crypto_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find libcrypto")
  endif(Crypto_FIND_REQUIRED)
endif(CRYPTO_FOUND)
