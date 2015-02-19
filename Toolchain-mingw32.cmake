# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER gcc.exe)
SET(CMAKE_CXX_COMPILER g++.exe)
SET(CMAKE_RC_COMPILER windres.exe)
SET(CMAKE_MAKE_PROGRAM mingw32-make.exe)

# here is the target environment located
#SET(CMAKE_FIND_ROOT_PATH  / /home/alex/mingw-install )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
