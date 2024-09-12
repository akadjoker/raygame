
project(bulang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -Wswitch -frtti -fno-strict-aliasing -DUSE_GRAPHICS ")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)




set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



file(GLOB SOURCES "src/*.cpp")
add_library(bulang STATIC  ${SOURCES})


target_include_directories(bulang PUBLIC include src)
target_precompile_headers(bulang PRIVATE src/pch.h)

if(CMAKE_BUILD_TYPE MATCHES Debug)

 #target_compile_options(bulang PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -Winvalid-pch -D_DEBUG)
  #target_link_options(bulang PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -Winvalid-pch -D_DEBUG) 
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bulang PRIVATE -O3 -march=native -flto -funroll-loops -DNDEBUG)
    target_link_options(bulang PRIVATE -O3 -march=native -flto -funroll-loops -DNDEBUG)
endif()

