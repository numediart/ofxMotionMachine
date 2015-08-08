# CMake config for MoMa - written by alexis
#
# copyright Numediart Institute, UMONS (c) 2015
#
# Everything is in one file and should remain so. We do *NOT* want to
# spread CMakeLists.txt all across the project. This allows us to keep
# code and compilation cleanly separated (even more so since this config
# is made specifically for linux).

PROJECT(MOMA_PROJECT)

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

OPTION(BUILD_SHARED_LIBS "Build Shared libraries" ON)

IF(UNIX)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ELSE()
	ADD_DEFINITIONS(-DUNDER_WXP)
	SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHsc")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF()

SET(TARGET_NAME moma)

# Should we split the library in three instead? To be discussed.
# NB: 'core' depends on 'parsers', 'features' and 'parsers'
#     both depends on 'core'.
#SET(TARGET_CORE moma-core)
#SET(TARGET_PARSERS moma-parsers)
#SET(TARGET_FEATURES moma-features)

# Core
SET(CORE_DIR ${PROJECT_SOURCE_DIR}/../../../MoMa/core)
INCLUDE_DIRECTORIES(${CORE_DIR})

# Parsers
SET(PARSERS_DIR ${PROJECT_SOURCE_DIR}/../../../MoMa/parsers)
INCLUDE_DIRECTORIES(${PARSERS_DIR})

# Features
SET(FEATURES_DIR ${PROJECT_SOURCE_DIR}/../../../MoMa/features)
FILE(GLOB FEATURES_LIST ${FEATURES_DIR}/* )

FOREACH(FEATURE_DIR ${FEATURES_LIST})
    IF(IS_DIRECTORY ${FEATURE_DIR})
        INCLUDE_DIRECTORIES(${FEATURE_DIR})
    ENDIF() 
ENDFOREACH()

# Dependency on Armadillo --> find compatible install
# 4.100 is not compatible with MoMa, 4.650.3 is. Difficult to bisect.
# this will look for version >= 4.650
SET(Armadillo_FIND_VERSION 4.650)
# FindArmadillo.cmake is part of default cmake install
FIND_PACKAGE(Armadillo REQUIRED)
INCLUDE_DIRECTORIES(${ARMADILLO_INCLUDE_DIRS})

# Get source code files
FILE(GLOB SRC_CORE ${CORE_DIR}/*.cpp)
FILE(GLOB SRC_PARSERS ${PARSERS_DIR}/*.cpp)
FILE(GLOB SRC_FEATURES ${FEATURES_DIR}/*/*.cpp)

# Create libmoma.so (or libmoma.a if BUILD_SHARED_LIBS is OFF)
ADD_LIBRARY(${TARGET_NAME} ${SRC_CORE} ${SRC_PARSERS} ${SRC_FEATURES})

# Same as above, should we split?
#ADD_LIBRARY(${TARGET_CORE} ${SRC_CORE})
#ADD_LIBRARY(${TARGET_PARSERS} ${SRC_PARSERS})
#ADD_LIBRARY(${TARGET_FEATURES} ${SRC_FEATURES})

TARGET_LINK_LIBRARIES(${TARGET_NAME} m ${ARMADILLO_LIBRARIES})
#TARGET_LINK_LIBRARIES(${TARGET_CORE} m ${ARMADILLO_LIBRARIES})
#TARGET_LINK_LIBRARIES(${TARGET_PARSERS} ${TARGET_CORE})
#TARGET_LINK_LIBRARIES(${TARGET_FEATURES} ${TARGET_CORE})
