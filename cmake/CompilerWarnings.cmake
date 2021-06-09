function(enable_compiler_warnings project_name)
    option(COMPILER_WARNINGS "Enable extended compiler warnings" OFF)

    set(GCC_FLAGS
        -Wall #enables warnings
        -Werror #all warnings treated as errors
        -Wextra #more extra warnings
        -Wshadow
        -Wformat=2
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wnull-dereference
        -Wuseless-cast
        -Wdouble-promotion)
    set(PROJECT_WARNINGS ${GCC_FLAGS})
    target_compile_options(${project_name} INTERFACE ${PROJECT_WARNINGS})
endfunction()
