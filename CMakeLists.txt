cmake_minimum_required(VERSION 3.2)
project(joguin)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQURIED ON)

# Simple DirectMedia Layer and its standalone components
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakefiles)
find_package(SDL2)
find_package(SDL2_image)
find_package(SDL2_ttf)
find_package(SDL2_mixer)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})

# PhysicsFS: a filesystem abstraction layer
find_package(PhysFS)

include_directories(${PHYSFS_INCLUDE_DIR})

add_library ( physfsextras 3rdparty/physicsfs/extras/physfsrwops.c )
add_library ( asprintf 3rdparty/asprintf/asprintf.c 3rdparty/asprintf/vasprintf-c99.c )
add_library ( parson 3rdparty/parson/parson.c )
add_library ( z_containers containers/vector.c )

add_executable(joguin main.c common.c entity.c stage.c states/credits_state.c settings.c states/menu_state.c ui.c anim.c sprite.c states/game_state.c vecmath.c )
target_link_libraries( joguin physfsextras asprintf parson z_containers ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} ${PHYSFS_LIBRARY} m)
