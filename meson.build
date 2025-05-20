project('chip8', 'c',
  version: '0.1',
  default_options: ['c_std=c17'])
subdir('src')

inc_dir = include_directories('include')

libsdl3 = dependency('sdl3', required : true)

deps = [libsdl3]

is_dev = get_option('dev')
if is_dev
  message('Dev build enabled')
  add_global_arguments('-DEVELOPER', language : 'c')
endif

executable('chip8', src, include_directories: inc_dir, dependencies : deps)
