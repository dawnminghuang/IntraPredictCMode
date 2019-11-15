###############################################################
# copyright 2019-2019, dawnminghuang                          #
###############################################################

PROJECT(IntraPredict)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

SET(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)
SET(CMAKE_SUPPRESS_REGENERATION true)
#SET(CMAKE_EXE_LINKER_FLAGS "/NOLOGO /INCREMENTAL:NO /NXCOMPAT /DYNAMICBASE:NO")

SET(RuntimeOutputDir ${CMAKE_BINARY_DIR}/../../source/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RuntimeOutputDir})
# SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${RuntimeOutputDir})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RuntimeOutputDir})
# SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${RuntimeOutputDir})
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-DCMAKE_BUILD_TYPE=RELEASE)
# ADD_DEFINITIONS(/W3)
#SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
#SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")

SET(DIR_SRCS
	common/dirent.h
    common/Log.h
	common/OutputWriter.h
	common/DistanceData.h
	common/DistanceCalculator.h
	common/SrcData.h
	common/Avs2Common.h
	common/HevcCommon.h
	common/H264Common.h
	common/Vp9Common.h
	IntraPredicter.h
	Avs2Predicter.h
	Avs2PredicterCMode.h
	H264Predicter.h
	H264PredicterCMode.h
	HevcPredicter.h
	HevcPredicterCMode.h
	Vp9Predicter.h
	Vp9PredicterCMode.h
	common/Log.cpp
	common/OutputWriter.cpp
	common/DistanceData.cpp
	common/DistanceCalculator.cpp
	common/SrcData.cpp
	IntraPredicter.cpp
	Avs2Predicter.cpp
	Avs2PredicterCMode.cpp
	H264Predicter.cpp
	H264PredicterCMode.cpp
	HevcPredicter.cpp
	HevcPredicterCMode.cpp
	Vp9Predicter.cpp
	Vp9PredicterCMode.cpp
	main.cpp
)

ADD_EXECUTABLE(IntraPredict ${DIR_SRCS})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    TARGET_LINK_LIBRARIES(IntraPredict libm.so.6)
endif()