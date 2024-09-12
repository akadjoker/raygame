project(main)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(
    -Wall
    -Wextra
    -Wno-missing-field-initializers
    -Wundef
    -Wuninitialized
    -Wunused
    -Wswitch
    -Wno-unused-parameter
    -Wwrite-strings
    -Wchar-subscripts
    -funsigned-char
)


file(GLOB SOURCES "src/*.cpp")
add_executable(main   ${SOURCES})


target_include_directories(main PUBLIC  include src)



if(CMAKE_BUILD_TYPE MATCHES Debug)

target_compile_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -Winvalid-pch -D_DEBUG)
target_link_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -Winvalid-pch -D_DEBUG) 
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(main PRIVATE -O3 -march=native -flto -funroll-loops -DNDEBUG)
    target_link_options(main PRIVATE -O3 -march=native -flto -funroll-loops -DNDEBUG)
endif()

target_link_libraries(main bulang )

if (WIN32)
    target_link_libraries(main Winmm.lib)
endif()


if (UNIX)
    target_link_libraries(main  m )
endif()
