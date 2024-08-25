# ---- Developer mode ----

# Developer mode enables targets and code paths in the CMake scripts that are
# only relevant for the developer(s) of TradingFramework
# Targets necessary to build the project must be provided unconditionally, so
# consumers can trivially build and package the project
if(PROJECT_IS_TOP_LEVEL)
  option(DEVELOPER_MODE "Enable developer mode" OFF)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  find_program(CCACHE ccache)
  if(CCACHE)
      set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
      set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  endif(CCACHE)

endif()

# ---- Warning guard ----

# target_include_directories with the SYSTEM modifier will request the compiler
# to omit warnings from the provided paths, if the compiler supports that
# This is to provide a user experience similar to find_package when
# add_subdirectory or FetchContent is used to consume this project
set(warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      TradingFramework_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for TradingFramework's includes, disabling warnings"
      ON
  )
  mark_as_advanced(TradingFramework_INCLUDES_WITH_SYSTEM)
  if(TradingFramework_INCLUDES_WITH_SYSTEM)
    set(warning_guard SYSTEM)
  endif()
endif()
