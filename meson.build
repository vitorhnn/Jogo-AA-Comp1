project('gunslinger', 'c')

sdl2_dep = dependency('sdl2')
sdl2_ttf_dep = dependency('SDL2_ttf')
sdl2_image_dep = dependency('SDL2_image')
sdl2_mixer_dep = dependency('SDL2_mixer')
physfs_dep = dependency('physfs')

# weird dance meson requires for libm
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

src = [
    'anim.c',
    'common.c',
    'effect.c',
    'entity.c',
    'main.c',
    'settings.c',
    'sprite.c',
    'stage.c',
    'ui.c',
    '3rdparty/asprintf/asprintf.c',
    '3rdparty/asprintf/vasprintf-c99.c',
    '3rdparty/parson/parson.c',
    '3rdparty/physicsfs/extras/physfsrwops.c',
    'containers/vector.c',
    'states/credits_state.c',
    'states/game_state.c',
    'states/menu_state.c',
    'drivers/vid_sdl.c',
]

gunslinger_exe = executable(
    'gunslinger',
    src,
    dependencies: [sdl2_dep, sdl2_ttf_dep, sdl2_image_dep, sdl2_mixer_dep, physfs_dep, m_dep],
)