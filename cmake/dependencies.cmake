macro(fetch TARGET REPO TAG)
    Include(FetchContent)

    FetchContent_Declare(${TARGET}
        GIT_REPOSITORY ${REPO}
        GIT_TAG        ${TAG}
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(${TARGET})
    FetchContent_GetProperties(${TARGET})
endmacro()

macro(add_3rdparty TARGET_NAME REPO TAG)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries")

    fetch(${TARGET_NAME} ${REPO} ${TAG})

    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
    if (${TARGET_TYPE} STREQUAL "INTERFACE_LIBRARY")
        target_compile_options(${TARGET_NAME} INTERFACE "-w")
    else()
        target_compile_options(${TARGET_NAME} PRIVATE "-w")
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${TARGET_NAME},INTERFACE_INCLUDE_DIRECTORIES>)
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_CLANG_TIDY "")
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_CPPCHECK "")
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER 3rdparty)
endmacro()

# ============================= 3RDPARTY ================================

find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(tgbot REQUIRED)
find_package(fmt REQUIRED)

macro(add_proto_target TARGET FOLDER)
    file(GLOB_RECURSE FILES "${FOLDER}/*.proto")

    set(INCLUDE_PROTO_BINARY_DIR ${CMAKE_BINARY_DIR}/generated/${TARGET})
    set(PROTO_BINARY_DIR ${INCLUDE_PROTO_BINARY_DIR}/${TARGET})
    make_directory(${PROTO_BINARY_DIR})

    get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION )
    get_target_property(PROTOC_PATH protobuf::protoc LOCATION)

    set(RES_FILES)
    foreach(f ${FILES})
        file(RELATIVE_PATH REL_PATH ${FOLDER} ${f})
        string(REPLACE ".proto" ".pb.cc" OUTPUT_PB_SOURCE "${PROTO_BINARY_DIR}/${REL_PATH}")
        string(REPLACE ".proto" ".grpc.pb.cc" OUTPUT_GRPC_SOURCE "${PROTO_BINARY_DIR}/${REL_PATH}")
        string(REPLACE ".proto" ".pb.h" OUTPUT_PB_HEADER "${PROTO_BINARY_DIR}/${REL_PATH}")
        string(REPLACE ".proto" ".grpc.pb.h" OUTPUT_GRPC_HEADER "${PROTO_BINARY_DIR}/${REL_PATH}")
        add_custom_command(
            OUTPUT ${OUTPUT_PB_SOURCE} ${OUTPUT_PB_HEADER} ${OUTPUT_GRPC_SOURCE} ${OUTPUT_GRPC_HEADER} 
            COMMAND protobuf::protoc 
            ARGS ${f} --cpp_out=${PROTO_BINARY_DIR} --grpc_out=${PROTO_BINARY_DIR} --python_out=${PROTO_BINARY_DIR} --proto_path=${FOLDER} --plugin=protoc-gen-grpc=${grpc_cpp_plugin_location}
            COMMAND ${CMAKE_COMMAND} 
            ARGS -DTARGET_FILE=${OUTPUT_PB_SOURCE} -P ${CMAKE_SOURCE_DIR}/cmake/rename_tinkoff_protobufs.cmake
            COMMAND ${CMAKE_COMMAND} 
            ARGS -DTARGET_FILE=${OUTPUT_GRPC_SOURCE} -P ${CMAKE_SOURCE_DIR}/cmake/rename_tinkoff_protobufs.cmake
            COMMAND ${CMAKE_COMMAND} 
            ARGS -DTARGET_FILE=${OUTPUT_PB_HEADER} -P ${CMAKE_SOURCE_DIR}/cmake/rename_tinkoff_protobufs.cmake
            COMMAND ${CMAKE_COMMAND} 
            ARGS -DTARGET_FILE=${OUTPUT_GRPC_HEADER} -P ${CMAKE_SOURCE_DIR}/cmake/rename_tinkoff_protobufs.cmake
            )
        LIST(APPEND RES_FILES ${OUTPUT_PB_SOURCE})
        LIST(APPEND RES_FILES ${OUTPUT_GRPC_SOURCE})
        LIST(APPEND RES_FILES ${OUTPUT_PB_HEADER})
        LIST(APPEND RES_FILES ${OUTPUT_GRPC_HEADER})
    endforeach()

    set_source_files_properties(${RES_FILES} PROPERTIES GENERATED TRUE)

    add_library(${TARGET} STATIC ${RES_FILES} ${FILES})

    target_link_libraries(${TARGET}
        PUBLIC
            gRPC::grpc++
            protobuf::libprotobuf
            ${grpc_LIBRARIES_TARGETS}
    )
    target_include_directories(${TARGET} PUBLIC ${INCLUDE_PROTO_BINARY_DIR} ${PROTO_BINARY_DIR})

    set_target_properties(${TARGET} PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:${TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
    set_target_properties(${TARGET} PROPERTIES CXX_CLANG_TIDY "")
    set_target_properties(${TARGET} PROPERTIES CXX_CPPCHECK "")
    set_target_properties(${TARGET} PROPERTIES FOLDER "generated")
  endmacro()
# find_package(OpenSSL REQUIRED)

add_3rdparty(rpp https://github.com/victimsnino/ReactivePlusPlus v2)
fetch(investAPI https://github.com/RussianInvestments/investAPI main)

add_proto_target(investapi_proto ${investapi_SOURCE_DIR}/src/docs/contracts)

foreach(TARGET_NAME "rppqt;rppgrpc")
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER 3rdparty)
endforeach()