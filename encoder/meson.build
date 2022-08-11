project('com.devguyahnaf.metube.encoder', 'cpp', version: '0.0.1')

compiler = meson.get_compiler('cpp')

assert(compiler.get_id() == 'gcc', 'This project only supports the GCC compiler')
assert(build_machine.cpu_family() == 'x86_64', 'You need a 64-bit processor to build or run this project')


vendor_incdir = include_directories('include')
src_incdir = include_directories('src')

# Dependencies
libdir = meson.current_source_dir() + '/lib'

deps = [
    compiler.find_library('libavformat'), # FFMPEG AV Format
    compiler.find_library('libamqpcpp', dirs: libdir), # AMQP-CPP
    compiler.find_library('libevent', dirs: libdir), # LibEvent,
    compiler.find_library('libminiocpp', dirs: libdir), # Minio SDK
    compiler.find_library('libssl', dirs: libdir), # OpenSSL [Minio Dep]
    compiler.find_library('libcrypto', dirs: libdir), # Crypto [Minio dep]
    compiler.find_library('libcurl', dirs: libdir), # libcurl [Minio Dep]
    compiler.find_library('libcurlpp', dirs: libdir), # Curl wrapper [Minio dep]
    compiler.find_library('libz', dirs: libdir),
    compiler.find_library('libpugixml', dirs: libdir),
    compiler.find_library('pthread'), # PThread
    compiler.find_library('dl'), # dl
]

executable('encoder', sources: files('src/main.cpp'), include_directories: [vendor_incdir, src_incdir], dependencies: deps)
