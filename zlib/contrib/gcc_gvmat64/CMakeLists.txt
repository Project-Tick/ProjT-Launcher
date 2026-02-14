enable_language(ASM)

# Need to push CMAKE_ASM_COMPILE_OBJECT to cache as otherwise it's not
# available in top-level

set(CMAKE_ASM_COMPILE_OBJECT ${CMAKE_ASM_COMPILE_OBJECT} CACHE INTERNAL "" FORCE)

if(MSVC)
    #cmake 4.1 doesn't consider cl an asm compiler
    cmake_policy(SET CMP0194 NEW)
endif(MSVC)

if(PTLIBZIPPY_BUILD_SHARED)
    target_sources(ptlibzippy PRIVATE gvmat64.S)
endif(PTLIBZIPPY_BUILD_SHARED)

if(PTLIBZIPPY_BUILD_STATIC)
    target_sources(ptlibzippystatic PRIVATE gvmat64.S)
endif(PTLIBZIPPY_BUILD_STATIC)
