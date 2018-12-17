add_subdirectory(utils)
add_subdirectory(pf)
add_subdirectory(fileio)
add_subdirectory(rm)
add_subdirectory(parser)
add_subdirectory(sm)

add_definitions(-DDEBUG)

add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCE} ${HEADERS})
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} SqlParser ${CMAKE_PROJECT_NAME}_lib)