project('WILLDO', 'cpp',
    version: 'first',
    default_options: [ 'cpp_std=c++20', 'buildtype=release' ]
)

dep_libpqxx = dependency('libpqxx', required: true, method: 'pkg-config')
inc_libpqxx = include_directories('/opt/homebrew/Cellar/libpqxx/7.10.1/include')

executable('WILLDO', 'source/main.cpp', dependencies: [dep_libpqxx], include_directories: [inc_libpqxx])
