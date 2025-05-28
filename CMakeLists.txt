cmake_minimum_required(VERSION 3.10)
project(Wizualizacja_ramienia_robota)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
    main.cpp
    glad.c
    shaderClass.cpp
    Texture.cpp
    Mesh.cpp
    Model.cpp
    VBO.cpp
    VAO.cpp
    EBO.cpp
    stb.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Libraries/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Libraries/lib
)

target_link_libraries(${PROJECT_NAME}
    glfw3
    assimp-vc143-mtd
    opengl32
)