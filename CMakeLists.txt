
project(Game)
cmake_minimum_required(VERSION 3.25)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SRC_FILES
     "*.h"
     "*.cpp"
)
add_executable(${PROJECT_NAME}
    ${SRC_FILES}
)

find_package(SFML COMPONENTS graphics window system REQUIRED)

target_include_directories(${PROJECT_NAME} 
  PRIVATE
  .
)

target_link_libraries(${PROJECT_NAME} 
  #external 
  sfml-graphics sfml-window sfml-system 
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)