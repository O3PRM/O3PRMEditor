# Declaring the project and generale info about it
PROJECT(O3PRMEDITOR)


set(AGRUM_INSTALLATION_DIRECTORY "/home/phw/usr")
set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/cmake/aGrUM/")

find_package(aGrUM)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(O3PRMEDITOR_VERSION_MAJOR "0")
SET(O3PRMEDITOR_VERSION_MINOR "4")
SET(O3PRMEDITOR_VERSION_PATCH "0")

SET(CMAKE_VERBOSE_MAKEFILE "ON")

# Configuring cmake
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Dependencies
FIND_PACKAGE(Qt4 REQUIRED)
SET(LIBS ${LIBS} ${QT_LIBRARIES})
FIND_PACKAGE(Qscintilla REQUIRED)
INCLUDE_DIRECTORIES(${QSCINTILLA_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${QSCINTILLA_LIBRARIES})

#FIND_LIBRARY(QSCINTILLA2 qscintilla2 PATHS ${QT_LIBRARY_DIR})
#IF(QSCINTILLA2)
#	MESSAGE(STATUS "Found QScintilla : ${QSCINTILLA2}")
#ELSE(QSCINTILLA2)
#	MESSAGE(FATAL_ERROR "---> QScintilla2 not found ! Visit http://www.riverbankcomputing.co.uk/")
#ENDIF(QSCINTILLA2)
#SET(LIBS ${LIBS} ${QSCINTILLA2})

# Reproducing what qmake does
FILE(GLOB_RECURSE O3PRM_SOURCES *.cpp)
FILE(GLOB_RECURSE O3PRM_HEADERS *.h)
FILE(GLOB_RECURSE O3PRM_FORMS *.ui)
FILE(GLOB_RECURSE O3PRM_RESOURCES *.qrc)
FILE(GLOB_RECURSE O3PRM_RC_FILE *.rc)

QT4_WRAP_CPP(O3PRM_HEADERS_MOC ${O3PRM_HEADERS})
QT4_WRAP_UI(O3PRM_FORMS_HEADERS ${O3PRM_FORMS})
QT4_ADD_RESOURCES(O3PRM_RESOURCES_RCC ${O3PRM_RESOURCES})

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(${O3PRMEDITOR_SOURCE_DIR})

# Checking for a C++11 compiler
FIND_PACKAGE(CXX11)
IF(CXX11_FOUND)
  SET(O3PRMEDITOR_CXX_FLAGS "${CXX11_FLAGS} ${O3PRM_CXX_FLAGS}" )
ELSE()
  MESSAGE(FATAL_ERROR "The aGrUM framework requires C++11. Please use a C++11 compliant compiler.")
ENDIF()
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lstdc++")

IF(AGRUM_FOUND)
  MESSAGE(STATUS "Found aGrUM")
  MESSAGE(STATUS "aGrUM include : ${AGRUM_INCLUDE_DIR}")
  MESSAGE(STATUS "aGrUM lib : ${AGRUM_LIB_DIR}")
  include_directories(${AGRUM_INCLUDE_DIR})
  link_directories(${AGRUM_LIB_DIR})
ELSE()
    MESSAGE(FATAL_ERROR "---> aGrUM not found !")
ENDIF()
SET(LIBS ${LIBS} ${AGRUM_LIB_DIR})

# Compiling flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${O3PRMEDITOR_CXX_FLAGS}")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

SET(CMAKE_CXX_FLAGS_DEBUG "${O3PRMEDITOR_CXX_FLAGS} -O0 -g -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "${O3PRMEDITOR_CXX_FLAGS} -O3 -DNDEBUG")

# Building the executable
ADD_EXECUTABLE(o3prmeditor ${O3PRM_SOURCES} ${O3PRM_HEADERS_MOC} ${O3PRM_FORMS_HEADERS} ${O3PRM_RESOURCES_RCC})
#ADD_EXECUTABLE(o3prmeditor ${O3PRM_SOURCES})
TARGET_LINK_LIBRARIES(o3prmeditor ${QT_LIBRARIES} ${QSCINTILLA_LIBRARIES} agrum)
SET_TARGET_PROPERTIES(o3prmeditor PROPERTIES LINK_FLAGS "-lstdc++")
 
### # ~~~~~~~~~~~~ LIBRARIES ~~~~~~~~~~~~ #
### 
### ##Cette ligne force l'exécution de Qt4 dans un Qt-creator qui utilise Qt5 par défaut 
### set(QT_QMAKE_EXECUTABLE "/usr/local/bin/qmake")
### 
### ## On trouve la librairie Qt
### include(${QT_USE_FILE})
### 
### ## On trouve la librairie QScintilla qui doit être installée dans le répertoire de Qt
### find_library(QSCINTILLA2 qscintilla2 PATHS ${QT_LIBRARY_DIR})
### if(QSCINTILLA2)
### 	message(STATUS "Found QScintilla : ${QSCINTILLA2}")
### else(QSCINTILLA2)
### 	message(FATAL_ERROR "---> QScintilla2 not found ! Visit http://www.riverbankcomputing.co.uk/")
### endif(QSCINTILLA2)
### 
### ## On trouve la librairie aGrUM
### find_library(AGRUM agrum PATHS "/usr/local/lib")
### if (AGRUM)
###     message(STATUS "Found aGrUM : ${AGRUM}")
### else()
###     message(FATAL_ERROR "---> aGrUM not found !")
### endif()
### 
### # ~~~~~~~~~~~~ SOURCES ~~~~~~~~~~~~ #
### 
### ## On trouve tous les fichiers sources
### file(GLOB_RECURSE O3PRM_SOURCES *.cpp)
### file(GLOB_RECURSE O3PRM_INCLUDES *.h)
### file(GLOB RESOURCES ../rsrc/*.qrc)
### file(GLOB_RECURSE FORMS *.ui)
### file(GLOB RC_FILE ../rsrc/*.rc)
### 
### # ~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~ #
### 
### ## Pour les moc et ui_*.h
### include_directories ( ../../../src/ )
### include_directories (${CMAKE_CURRENT_BINARY_DIR})
### include_directories (${CMAKE_CURRENT_SOURCE_DIR})
### 
### # ~~~~~~~~~~~~ PREPROCESSING ~~~~~~~~~~~~ #
### 
### qt4_wrap_cpp( MOCS ${O3PRM_INCLUDES} )
### qt4_wrap_ui( UIS ${FORMS} )
### qt4_add_resources( QRC ${RESOURCES} )
### 
### # Executable icon
### if(MINGW)
###   set(CMAKE_RC_COMPILER_INIT windres)
###   enable_language(RC)
###   #SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
###   set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
###   set(GUI_TYPE WIN32)
### endif(MINGW)
### 
### # ~~~~~~~~~~~ COMPILATION FLAGS ~~~~~~~~~~~~~~~~~~~~~~~ #
### SET(O3PRM_OPTIMIZATION "-O3")
### SET(O3PRM_INLINING_POLICY "") # "-fno-inline-small-functions")
### 
### LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
### 
### FIND_PACKAGE(CXX11)
### IF(CXX11_FOUND)
###   SET(O3PRM_CXX_FLAGS "${CXX11_FLAGS} ${O3PRM_CXX_FLAGS}" )
### ELSE()
###   MESSAGE(FATAL_ERROR "The aGrUM framework requires C++11. Please use a C++11 compliant compiler.")
### ENDIF()
### 
### set(CMAKE_CXX_FLAGS_DEBUG " -O0 -pg -g -DGUM_NO_INLINE")
### set(CMAKE_C_FLAGS_DEBUG "-O0 -pg -g -DGUM_NO_INLINE")
### 
### set(CMAKE_CXX_FLAGS_RELEASE "${O3PRM_OPTIMIZATION} -DNDEBUG ${O3PRM_INLINING_POLICY}")
### set(CMAKE_C_FLAGS_RELEASE "${O3PRM_OPTIMIZATION} -DNDEBUG ${O3PRM_INLINING_POLICY}")
### 
### set(CMAKE_CXX_FLAGS "${AGRUM_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
### set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg -lstdc++")
### 
### # ~~~~~~~~~~~~ PROCESSING ~~~~~~~~~~~~ #
### set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE} )
### add_executable(o3prmeditor ${GUI_TYPE} ${O3PRM_SOURCES} ${INCLUDES} ${MOCS} ${UIS} ${QRC} ${AGRUM_INCLUDES} ${RC_FILE})
### 
### #set_target_properties( o3prmeditor PROPERTIES LINK_FLAGS "--enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc" )
### set_target_properties( o3prmeditor PROPERTIES LINK_FLAGS "-lstdc++" )
### # I do not know what is enable-stdcall-fixup (neither do G++ ???)
### target_link_libraries( o3prmeditor ${QT_LIBRARIES} ${QSCINTILLA2} ${AGRUM})
### 
