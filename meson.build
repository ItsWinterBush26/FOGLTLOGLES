# FOGLTOGLES (Fast OpenGL translation (to) OpenGLES)
project('FOGLTOGLES', 'cpp',
 version: '0.0.1',
 default_options: ['cpp_std=c++17']
)

cmake = import('cmake')
ndk_root = get_option('ndk_root')
android_arch = get_option('android_arch')
android_api = 21

cc = meson.get_compiler('cpp')

sources = {
  'main': [
    'src/layer.cpp'
  ],
  'gles20': [
    'translations/gles20/glclear.cpp',
    'translations/gles20/glclearcolor.cpp',
    'translations/gles20/glcleardepth.cpp',
    'translations/gles20/gldisable.cpp',
    'translations/gles20/glenable.cpp',
    'translations/gles20/glscissor.cpp',
    'translations/gles20/glviewport.cpp'
  ],
  'gles30': [
    'translations/gles30/glclientwaitsync.cpp',
    'translations/gles30/glfencesync.cpp',
    'translations/gles30/glwaitsync.cpp',
    'translations/gles30/gldeletesync.cpp'
  ]
}

include_dirs = ['src', 'translations']
src_files = [
  sources['main'] + sources['gles20'] + sources['gles30']
]

if ndk_root == '' or android_arch == ''
 error('Missing NDK options. Use: -Dndk_root=... -Dandroid_arch=...')
endif

sysroot = '@0@/toolchains/llvm/prebuilt/linux-@1@/sysroot'.format(ndk_root, host_machine.cpu_family())
abi = android_arch == 'arm' ? 'armeabi-v7a' : 'arm64-v8a'

header_dir = '@0@/sysroot/usr/include'.format(ndk_root)
# '@0@/platforms/android-@1@/arch-@2@/usr/include'.format(ndk_root, android_api, android_arch == 'arm' ? android_arch : 'arm64')

lib_dirs = [
 '@0@/usr/lib/@1@-linux-androideabi'.format(sysroot, android_arch),
 '@0@/usr/lib/@1@-linux-androideabi/@2@'.format(sysroot, android_arch, android_api),
 '@0@/sources/cxx-stl/llvm-libc++/libs/@1@'.format(ndk_root, abi),
 '/system/lib'
]

normal_deps = [
  dependency('threads'),
  cc.find_library('GLESv3', dirs: lib_dirs),
  cc.find_library('GLESv2', dirs: lib_dirs),
  cc.find_library('EGL', dirs: lib_dirs),
  cc.find_library('android', dirs: lib_dirs),
  cc.find_library('log', dirs: lib_dirs),
  cc.find_library('c', dirs: lib_dirs),
  cc.find_library('dl', dirs: lib_dirs),
  cc.find_library('m', dirs: lib_dirs),
  cc.find_library('c++_static', dirs: lib_dirs, static: true)
]

deps = normal_deps
include_dirs += header_dir

link_args = [
 '-static-libstdc++',
 '-Wl,--whole-archive',
 '@0@/libc++_static.a'.format(lib_dirs[2]),
 '-Wl,--no-whole-archive',
 '-L/system/lib@0@'.format(abi == 'armeabi-v7a' ? '' : '64')
]

lib = shared_library('FOGLTOGLES', src_files,
   include_directories: include_directories(include_dirs),
   dependencies: deps,
   link_args: link_args)

