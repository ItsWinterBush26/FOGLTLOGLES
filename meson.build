# FOGLTOGLES (Fast OpenGL translation (to) OpenGLES)
project('FOGLTOGLES', 'cpp',
 version: '0.0.1',
 default_options: ['cpp_std=c++17']
)

cc = meson.get_compiler('cpp')

sources = {
  'main': [
    'src/layer.cpp'
  ],
  'gles20': [
    'translations/gles20/direct.cpp',
    'translations/gles20/emulated.cpp',
  ],
  'gles30': [
    'translations/gles30/glclientwaitsync.cpp',
    'translations/gles30/glfencesync.cpp',
    'translations/gles30/glwaitsync.cpp',
    'translations/gles30/gldeletesync.cpp'
  ]
}

include_dirs = ['src', 'translations', 'utils']
src_files = [
  sources['main'] + sources['gles20'] + sources['gles30']
]

normal_deps = [
  dependency('threads'),
  cc.find_library('GLESv3'),
  cc.find_library('GLESv2'),
  cc.find_library('EGL'),
  cc.find_library('android'),
  cc.find_library('log'),
  cc.find_library('c'),
  cc.find_library('c++_static', static: true)
]

deps = normal_deps

lib = shared_library('FOGLTOGLES', src_files,
   include_directories: include_directories(include_dirs),
   dependencies: deps)

