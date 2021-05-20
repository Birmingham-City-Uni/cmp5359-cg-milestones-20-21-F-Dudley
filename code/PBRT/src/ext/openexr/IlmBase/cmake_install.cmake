# Install script for directory: D:/Software/PBRT/pbrt-v3/src/ext/openexr/IlmBase

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/PBRT-V3")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenEXR" TYPE FILE FILES "D:/Software/PBRT/PBRT Build/src/ext/openexr/IlmBase/config/IlmBaseConfig.h")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/Software/PBRT/PBRT Build/src/ext/openexr/IlmBase/Half/cmake_install.cmake")
  include("D:/Software/PBRT/PBRT Build/src/ext/openexr/IlmBase/Iex/cmake_install.cmake")
  include("D:/Software/PBRT/PBRT Build/src/ext/openexr/IlmBase/IexMath/cmake_install.cmake")
  include("D:/Software/PBRT/PBRT Build/src/ext/openexr/IlmBase/Imath/cmake_install.cmake")
  include("D:/Software/PBRT/PBRT Build/src/ext/openexr/IlmBase/IlmThread/cmake_install.cmake")

endif()

