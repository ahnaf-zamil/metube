project('com.ahnafzamil.metube.encoder', 'cpp', version: '0.0.12')

compiler = meson.get_compiler('cpp')

assert(compiler.get_id() == 'gcc', 'This project only supports the GCC compiler')
assert(build_machine.cpu_family() == 'x86_64', 'You need a 64-bit processor to build or run this project')


vendor_incdir = include_directories('include')
src_incdir = include_directories('src')

# Dependencies
libdir = meson.current_source_dir() + '/lib'

deps = [
    dependency('libavformat', method: 'pkg-config'), # FFMPEG AV Format
    compiler.find_library('amqpcpp', dirs: libdir), # AMQP-CPP
    compiler.find_library('event', dirs: libdir), # LibEvent,
    compiler.find_library('miniocpp', dirs: libdir), # Minio SDK
    compiler.find_library('ssl', dirs: libdir), # OpenSSL [Minio Dep]
    compiler.find_library('crypto', dirs: libdir), # Crypto [Minio dep]
    compiler.find_library('curl', dirs: libdir), # libcurl [Minio Dep]
    compiler.find_library('curlpp', dirs: libdir), # Curl wrapper [Minio dep]
    compiler.find_library('z', dirs: libdir),
    compiler.find_library('pugixml', dirs: libdir),
    compiler.find_library('pthread'), # PThread
    compiler.find_library('dl'), # dl
]

executable(
    'encoder', 
    sources: files('src/main.cpp', 'src/worker.cpp', 'src/encoder.cpp', 'src/consumer.cpp'), 
    include_directories: [vendor_incdir, src_incdir], 
    dependencies: deps
)
