cmake_minimum_required(VERSION 3.26.0)

project(DEMIplatformer VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENTIONS off)

file(
  GLOB_RECURSE
  SOURCES
  CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)






set(SDL2_DIR "D:/SDL/SDL2-2.26.4/cmake")
find_package(SDL2 REQUIRED)

set(SDL2_image_DIR "D:/SDL_image/SDL2_image-2.6.3/cmake")
find_package(SDL2_image CONFIG REQUIRED)

set(SDL2_ttf_DIR "D:/SDL_ttf/SDL2_ttf-2.20.2/cmake")
find_package(SDL2_ttf CONFIG REQUIRED)

find_package(box2d REQUIRED)
include_directories("D:/dev/vcpkg/vcpkg/installed/x64-windows/include")


include_directories("D:/fmod/FMOD Studio API Windows/api/core/inc")

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES} "src/main.cpp")

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/src
    )

target_link_libraries(
    ${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    "D:/SDL_image/SDL2_image-2.6.3/lib/x64/SDL2_image.lib"
    ${SDL2_ttf_LIBRARIES}
    "D:/fmod/FMOD Studio API Windows/api/core/lib/x64/fmod_vc.lib"
    "D:/fmod/FMOD Studio API Windows/api/core/lib/x64/fmodL_vc.lib"
    "D:/dev/vcpkg/vcpkg/installed/x64-windows/lib/box2d.lib"
    ) 