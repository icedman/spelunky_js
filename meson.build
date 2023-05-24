project('template', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

pwd = meson.current_source_dir()
cc = meson.get_compiler('c')

src_files = [
    'src/util.c',
    'src/vector.c',
    'src/matrix.c',
    'src/context.c',
    'src/polygon.c',
    'src/font.c',
    'src/fastRandom.c',
    'src/entity.c',
    'src/data.c',
    'src/renderer.c',
    'src/game.c',
    'src/menu.c',
    'src/tests.c',
]

src_includes = include_directories(
    'include'
)

add_global_arguments('-DCONFIG_VERSION="2021-03-27"', language: 'c')
add_global_arguments('-DCONFIG_BIGNUM', language: 'c')
add_global_arguments('-Denviron="DEV"', language: 'c')
quickjs_src_files = [
    'quickjs/cutils.c',
    'quickjs/libbf.c',
    'quickjs/libregexp.c',
    'quickjs/libunicode.c',
    'quickjs/quickjs-libc.c',
    'quickjs/quickjs.c',
    # 'quickjs/repl.c',
]

quickjs_src_includes = include_directories(
    'quickjs'
)

m_dep = cc.find_library('m', required : true, disabler: true)
sdl2_dep = dependency('SDL2', required: true, disabler: true)
sdl2_image_dep = dependency('SDL2_image', required: true, disabler: true)

executable('spe',
    'src/main.c',
    src_files, quickjs_src_files,
    include_directories: [ src_includes, quickjs_src_includes ],
    dependencies: [ m_dep, sdl2_dep, sdl2_image_dep ]
)
