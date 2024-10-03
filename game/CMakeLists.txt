project(game)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(extern)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Dfsanitize=address -D_CRT_SECURE_NO_WARNINGS -DWINDOWS")
endif()


file(GLOB SOURCES "src/*.cpp")
add_executable(game   ${SOURCES})


target_include_directories(game PUBLIC  include src)
target_precompile_headers(bulang PRIVATE src/pch.h)


if(CMAKE_BUILD_TYPE MATCHES Debug)

if (UNIX)
target_compile_options(game PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG -DLINUX)
target_link_options(game PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG ) 
endif()


elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(game PRIVATE -O3   -DNDEBUG )
    target_link_options(game PRIVATE -O3   -DNDEBUG )
endif()

target_link_libraries(game bulang raylib)

if (WIN32)
    target_link_libraries(game Winmm.lib)
endif()


if (UNIX)
    target_link_libraries(game  m )
endif()
