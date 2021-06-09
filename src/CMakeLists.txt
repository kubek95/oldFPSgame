file(GLOB SOURCES
     *.hpp
     *.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE compiler_warnings
    )
