
file(GLOB_RECURSE COVERAGE_PROFRAW_FILES ${PROJECT_BINARY_DIR} *.profraw)
file(GLOB_RECURSE COVERAGE_OBJECTS ${PROJECT_BINARY_DIR}/build/bin *)
set(COVERAGE_OBJECTS_ARGS "")
foreach(f ${COVERAGE_OBJECTS})
    list(APPEND COVERAGE_OBJECTS_ARGS -object=${f})
endforeach()
set(COVERAGE_ADDITIONAL_ARGS -instr-profile=${PROJECT_BINARY_DIR}/coverage.profdata ${COVERAGE_OBJECTS_ARGS})

add_custom_target(
    coverage
    COMMAND llvm-profdata merge -sparse=true ${COVERAGE_PROFRAW_FILES} -o ${PROJECT_BINARY_DIR}/coverage.profdata

    # COMMAND llvm-cov report --ignore-filename-regex=build|tests ${RPP_COVERAGE_TARGETS}
    COMMAND llvm-cov show --ignore-filename-regex=build|tests --show-branches=count --show-expansions --show-line-counts --show-line-counts-or-regions --show-regions ${COVERAGE_ADDITIONAL_ARGS} > ${PROJECT_BINARY_DIR}/coverage.txt
    COMMENT "Generating coverage report"
    VERBATIM
)
