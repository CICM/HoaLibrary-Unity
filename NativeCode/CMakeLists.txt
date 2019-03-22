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

if (MSVC)
  option(STATIC_MSVC_RUNTIME "Static linkage of MSVC runtime" ON)
  SET(MSVC_RUNTIME_FLAG "/MT" CACHE STRING "MSVC Runtime flag")
  if (STATIC_MSVC_RUNTIME)
      SET(MSVC_RUNTIME_FLAG "/MT")
  else ()
      SET(MSVC_RUNTIME_FLAG "/MD")
  endif ()
  message(STATUS "MSVC Runtime flag: ${MSVC_RUNTIME_FLAG}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_RUNTIME_FLAG}" CACHE INTERNAL "" FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${MSVC_RUNTIME_FLAG}d" CACHE INTERNAL "" FORCE)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${MSVC_RUNTIME_FLAG}" CACHE INTERNAL "" FORCE)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${MSVC_RUNTIME_FLAG}d" CACHE INTERNAL "" FORCE)
endif ()

#--------------------------------------
# Sources
#--------------------------------------

set(HOA_UNITY_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(HOA_UNITY_SOURCES
        ${HOA_UNITY_SOURCE_DIR}/AudioPluginInterface.h
        ${HOA_UNITY_SOURCE_DIR}/AudioPluginUtil.h
        ${HOA_UNITY_SOURCE_DIR}/AudioPluginUtil.cpp
        ${HOA_UNITY_SOURCE_DIR}/PluginList.h
        ${HOA_UNITY_SOURCE_DIR}/HoaLibraryApi.h
        ${HOA_UNITY_SOURCE_DIR}/HoaLibraryApi.cpp
        ${HOA_UNITY_SOURCE_DIR}/HoaLibraryVR.h
        ${HOA_UNITY_SOURCE_DIR}/HoaLibraryVR.cpp
        ${HOA_UNITY_SOURCE_DIR}/UnityCallbacks.hpp
        ${HOA_UNITY_SOURCE_DIR}/Plugin_HoaLibraryVR_Renderer.cpp
        ${HOA_UNITY_SOURCE_DIR}/Plugin_HoaLibraryVR_Spatializer.cpp
        )
source_group(UnityPlugin FILES ${HOA_UNITY_SOURCES})

# activate optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -msse -msse2 -msse3 -mfpmath=sse -march=native")

#--------------------------------------
# HoaLibrary

set(HOA_BUILD_TESTS OFF CACHE INTERNAL "")
add_subdirectory(HoaLibrary)

#--------------------------------------

add_library(AudioPluginHoaLibraryVR SHARED ${HOA_UNITY_SOURCES})
target_link_libraries(AudioPluginHoaLibraryVR PRIVATE HoaLibrary::HoaLibrary)

#--------------------------------------
# Properties
#--------------------------------------

if (APPLE)
    set_target_properties(AudioPluginHoaLibraryVR PROPERTIES LINK_FLAGS "-Wl,-all_load")
    set_target_properties(AudioPluginHoaLibraryVR PROPERTIES PREFIX "" SUFFIX ".bundle")
elseif (MSVC)
    set_target_properties(AudioPluginHoaLibraryVR PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif ()

#--------------------------------------
# INSTALL
#--------------------------------------

set(HOA_UNITY_INSTALL_DIR "${PROJECT_SOURCE_DIR}/../Assets/HoaLibraryVR/Plugins")

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
