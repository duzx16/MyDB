add_subdirectory(rm)

add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCE} ${HEADERS})
add_executable(${CMAKE_PROJECT_NAME} main.cpp)