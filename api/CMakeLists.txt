CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(dhs)

SET(GPU_ARCH "52" CACHE STRING "Cuda architecture number, one of ('20', '30', '32', '35', '37', '50', '52', '53')")
SET(GCC_CUDA_VERSION "gcc-5" CACHE STRING "GCC compiler to be used with cuda")

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cuhe/cmake)

FIND_PACKAGE(NTL REQUIRED)
FIND_PACKAGE(OpenMP REQUIRED)
FIND_PACKAGE(CUDA REQUIRED)
FIND_PACKAGE(GMP REQUIRED)
FIND_PACKAGE(Boost REQUIRED COMPONENTS iostreams)

FIND_LIBRARY(CUHE_LIBRARY NAMES cuHEShared PATHS ${CMAKE_SOURCE_DIR}/../cuhe)

INCLUDE(FindCUDA)

FILE(GLOB SOURCES "*.cu" "*.h")

LIST(APPEND CMAKE_CXX_FLAGS "-m64 -O3 -ffast-math -Wall")

LIST(APPEND CUDA_NVCC_FLAGS "-D_FORCE_INLINES -ccbin ${GCC_CUDA_VERSION} -std=c++11 -gencode arch=compute_${GPU_ARCH},code=sm_${GPU_ARCH}")

SET(LIB_NAME "dhs")

CUDA_BUILD_CLEAN_TARGET()

CUDA_ADD_LIBRARY( dhs
    DHS.cu
    STATIC
)
TARGET_LINK_LIBRARIES(dhs)

CUDA_ADD_LIBRARY( dhsShared
    DHS.cu
    SHARED
)

TARGET_LINK_LIBRARIES(dhsShared ${CUHE_LIBRARY} ${NTL_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})

SET(LIB_NAME "he")

set(JAVA_INCLUDE_PATH $ENV{JAVA_HOME}/include)
set(JAVA_INCLUDE_PATH2 ${JAVA_INCLUDE_PATH}/linux)
include_directories(${JAVA_INCLUDE_PATH})
include_directories(${JAVA_INCLUDE_PATH2})

CUDA_ADD_LIBRARY( heShared
    org_wso2_siddhi_extension_he_api_HomomorphicEncryptionEvaluation.cpp
    org_wso2_siddhi_extension_he_api_HomomorphicEncDecService.cpp
    SHARED
)

TARGET_LINK_LIBRARIES(heShared dhsShared ${CUHE_LIBRARY} ${NTL_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})
