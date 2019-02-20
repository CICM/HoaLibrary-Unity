# Copyright 2019 Eliott PARIS, CICM, ArTec.

#--------------------------------------
# Project settings
#--------------------------------------

cmake_minimum_required(VERSION 3.2)

project(HoaLibraryVR CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#--------------------------------------
# Compiler settings
#--------------------------------------

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

#--------------------------------------
# Sources
#--------------------------------------

set(HOA_UNITY_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(HOA_UNITY_SOURCES
        ${HOA_UNITY_SOURCE_DIR}/AudioPluginInterface.h
        ${HOA_UNITY_SOURCE_DIR}/AudioPluginUtil.h
        ${HOA_UNITY_SOURCE_DIR}/AudioPluginUtil.cpp
        ${HOA_UNITY_SOURCE_DIR}/pluginList.h
        ${HOA_UNITY_SOURCE_DIR}/UnityCallbacks.hpp
        ${HOA_UNITY_SOURCE_DIR}/Plugin_HoaLibraryVR_Renderer.cpp
        ${HOA_UNITY_SOURCE_DIR}/Plugin_HoaLibraryVR_Spatializer.cpp
        )
source_group(UnityPlugin FILES ${HOA_UNITY_SOURCES})

#--------------------------------------

set(HOALIBRARY_DIR "${PROJECT_SOURCE_DIR}/HoaLibrary" CACHE PATH "Path to HoaLibrary")
file(GLOB HOALIBRARY_SOURCES_DIR "${HOALIBRARY_DIR}/Sources")
file(GLOB HOALIBRARY_SOURCES ${HOALIBRARY_SOURCES_DIR}/*.hpp)
source_group(HoaLibrary FILES ${HOALIBRARY_SOURCES})

#--------------------------------------

add_library(AudioPluginHoaLibraryVR SHARED ${HOA_UNITY_SOURCES} ${HOALIBRARY_SOURCES})
target_include_directories(AudioPluginHoaLibraryVR PUBLIC ${HOALIBRARY_SOURCES_DIR})

#--------------------------------------
# Properties
#--------------------------------------

set(WHOLE_ARCHIVE_FLAGS "")
if (APPLE)
    set_target_properties(AudioPluginHoaLibraryVR PROPERTIES LINK_FLAGS "-Wl,-all_load")
    set_target_properties(AudioPluginHoaLibraryVR PROPERTIES PREFIX "" SUFFIX ".bundle")
elseif (MSVC)
    set_target_properties(AudioPluginHoaLibraryVR PROPERTIES LINK_FLAGS "/MANIFEST:NO ${WHOLE_ARCHIVE_FLAGS}")
endif ()

#--------------------------------------
# INSTALL
#--------------------------------------

set(HOA_UNITY_INSTALL_DIR "${PROJECT_SOURCE_DIR}/../Assets/Plugins")

set(INSTALL_BIN_DIR "")
if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(INSTALL_BIN_DIR "x86_64")
    else ()
        set(INSTALL_BIN_DIR "x86")
    endif ()
else ()
    set(INSTALL_BIN_DIR "x86_64")
endif ()

install(TARGETS AudioPluginHoaLibraryVR
        LIBRARY DESTINATION "${HOA_UNITY_INSTALL_DIR}/${INSTALL_BIN_DIR}/"
        ARCHIVE DESTINATION "${HOA_UNITY_INSTALL_DIR}/${INSTALL_BIN_DIR}/"
        RUNTIME DESTINATION "${HOA_UNITY_INSTALL_DIR}/${INSTALL_BIN_DIR}/")
