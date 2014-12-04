project(O3PRM_EDITOR)

cmake_minimum_required(VERSION 2.8)

set(o3prmeditor_VERSION_MAJOR "0")
set(o3prmeditor_VERSION_MINOR "4")
set(o3prmeditor_VERSION_PATCH "0")

set(CMAKE_VERBOSE_MAKEFILE "ON")

# ~~~~~~~~~~~~ LIBRARIES ~~~~~~~~~~~~ #

##Cette ligne force l'exécution de Qt4 dans un Qt-creator qui utilise Qt5 par défaut 
set(QT_QMAKE_EXECUTABLE "/usr/local/bin/qmake")

## On trouve la librairie Qt
find_package (Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})

## On trouve la librairie QScintilla qui doit être installée dans le répertoire de Qt
find_library(QSCINTILLA2 qscintilla2 PATHS ${QT_LIBRARY_DIR})
if(QSCINTILLA2)
	message(STATUS "Found QScintilla : ${QSCINTILLA2}")
else(QSCINTILLA2)
	message(FATAL_ERROR "---> QScintilla2 not found ! Visit http://www.riverbankcomputing.co.uk/")
endif(QSCINTILLA2)

## On trouve la librairie aGrUM
find_library(AGRUM agrum PATHS "/usr/local/lib")
if (AGRUM)
    message(STATUS "Found aGrUM : ${AGRUM}")
else()
    message(FATAL_ERROR "---> aGrUM not found !")
endif()

# ~~~~~~~~~~~~ SOURCES ~~~~~~~~~~~~ #

## On trouve tous les fichiers sources
file(GLOB_RECURSE O3PRM_SOURCES *.cpp)
file(GLOB_RECURSE O3PRM_INCLUDES *.h)
file(GLOB RESOURCES ../rsrc/*.qrc)
file(GLOB_RECURSE FORMS *.ui)
file(GLOB RC_FILE ../rsrc/*.rc)

# ~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~ #

## Pour les moc et ui_*.h
include_directories ( ../../../src/ )
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# ~~~~~~~~~~~~ PREPROCESSING ~~~~~~~~~~~~ #

qt4_wrap_cpp( MOCS ${O3PRM_INCLUDES} )
qt4_wrap_ui( UIS ${FORMS} )
qt4_add_resources( QRC ${RESOURCES} )

# Executable icon
if(MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  enable_language(RC)
  #SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  set(GUI_TYPE WIN32)
endif(MINGW)

# ~~~~~~~~~~~ COMPILATION FLAGS ~~~~~~~~~~~~~~~~~~~~~~~ #
set(O3PRM_OPTIMIZATION "-O3")
set(O3PRM_INLINING_POLICY "") # "-fno-inline-small-functions")

list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package (CXX11)
if (CXX11_FOUND)
  set (AGRUM_CXX_FLAGS "${CXX11_FLAGS} ${AGRUM_CXX_FLAGS}" )
else ()
  message(FATAL_ERROR "aGrUM is now using C++11. Please find a compiler (for instance GCC) C++11 compliant")
endif ()

set(CMAKE_CXX_FLAGS_DEBUG " -O0 -pg -g -DGUM_NO_INLINE")
set(CMAKE_C_FLAGS_DEBUG "-O0 -pg -g -DGUM_NO_INLINE")

set(CMAKE_CXX_FLAGS_RELEASE "${O3PRM_OPTIMIZATION} -DNDEBUG ${O3PRM_INLINING_POLICY}")
set(CMAKE_C_FLAGS_RELEASE "${O3PRM_OPTIMIZATION} -DNDEBUG ${O3PRM_INLINING_POLICY}")

set(CMAKE_CXX_FLAGS "${AGRUM_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -lstdc++")

# ~~~~~~~~~~~~ PROCESSING ~~~~~~~~~~~~ #
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE} )
add_executable(o3prmeditor ${GUI_TYPE} ${O3PRM_SOURCES} ${INCLUDES} ${MOCS} ${UIS} ${QRC} ${AGRUM_INCLUDES} ${RC_FILE})

#set_target_properties( o3prmeditor PROPERTIES LINK_FLAGS "--enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc" )
set_target_properties( o3prmeditor PROPERTIES LINK_FLAGS "-lstdc++" )
# I do not know what is enable-stdcall-fixup (neither do G++ ???)
target_link_libraries( o3prmeditor ${QT_LIBRARIES} ${QSCINTILLA2} ${AGRUM})

