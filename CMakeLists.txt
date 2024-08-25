cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    TradingFramework
    VERSION 0.0.1
    DESCRIPTION "C++ based set of tools for trading including connectors, different views, algo and etc"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)
include(cmake/dependencies.cmake)

if(DEVELOPER_MODE)
  include(cmake/dev-mode.cmake)
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of TradingFramework"
  )
endif()

add_subdirectory(source)

function(add_folders DIR)
    get_filename_component(ABSOLUTE_PATH ${DIR} ABSOLUTE)
    file(RELATIVE_PATH RELATIVE_PATH "${PROJECT_SOURCE_DIR}" ${ABSOLUTE_PATH})
    get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(TGT IN LISTS TGTS)
      get_property(folder TARGET "${TGT}" PROPERTY FOLDER)
      set_target_properties(${TGT} PROPERTIES FOLDER ${RELATIVE_PATH})
    endforeach()

    get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)
    foreach(SUBDIR IN LISTS SUBDIRS)
      add_folders("${SUBDIR}")
    endforeach()
endfunction()

add_folders("source")