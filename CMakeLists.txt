# Declaring the project and generale info about it
PROJECT(O3PRMEDITOR)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(O3PRMEDITOR_VERSION_MAJOR "0")
SET(O3PRMEDITOR_VERSION_MINOR "5")
SET(O3PRMEDITOR_VERSION_PATCH "0")

# Configuring cmake
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Dependencies
#set(Boost_USE_STATIC_LIBS ON) 
#set(Boost_USE_MULTITHREADED OFF)  
#set(Boost_USE_STATIC_RUNTIME OFF) 
#find_package(Boost 1.45.0 REQUIRED COMPONENTS filesystem) 
#include_directories(${Boost_INCLUDE_DIRS}) 

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTXML TRUE)

FIND_PACKAGE(Qscintilla REQUIRED)
INCLUDE_DIRECTORIES(${QSCINTILLA_INCLUDE_DIRS})

INCLUDE(ExternalProject)
ExternalProject_Add(AGRUM-SRC
        PREFIX aGrUM
        GIT_REPOSITORY file:///Users/lto/Projets/aGrUM-dev
        GIT_TAG static_lib
        #GIT_REPOSITORY git://forge.lip6.fr/aGrUM 
        #GIT_TAG 0.9.0
        CONFIGURE_COMMAND ""
        UPDATE_COMMAND ""
        BUILD_COMMAND ./act install release -p linux -j 7 -m PRM -d <INSTALL_DIR> # call with python instead of ./
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        )

SET(AGRUM_INCLUDE_DIR "${PROJECT_BINARY_DIR}/aGrUM/include/")
SET(AGRUM_LIB_DIR "${PROJECT_BINARY_DIR}/aGrUM/lib/")

INCLUDE_DIRECTORIES(${AGRUM_INCLUDE_DIR})
LINK_DIRECTORIES(${AGRUM_LIB_DIR})

ExternalProject_add(GMOCK
    PREFIX gmock
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    URL_HASH SHA1=f9d9dd882a25f4069ed9ee48e70aff1b53e3c5a5
    INSTALL_COMMAND mkdir -p <INSTALL_DIR>/lib <INSTALL_DIR>/include
        COMMAND cp -R -f <INSTALL_DIR>/src/GMOCK/include/gmock <INSTALL_DIR>/include
        COMMAND cp -f <BINARY_DIR>/libgmock.a <INSTALL_DIR>/lib
        COMMAND cp -R -f <INSTALL_DIR>/src/GMOCK/gtest/include/gtest <INSTALL_DIR>/include
        COMMAND cp -f <BINARY_DIR>/gtest/libgtest.a <INSTALL_DIR>/lib
)

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/gmock/include)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/gmock/lib)

# Reproducing what qmake does
SET(O3PRMEDITOR_MAIN src/main.cpp)
SET(SOURCES src/controllers/buildcontroller.cpp
            src/controllers/editcontroller.cpp
            src/controllers/filecontroller.cpp
            src/controllers/projectcontroller.cpp
            src/controllers/searchcontroller.cpp
            src/controllers/viewcontroller.cpp
            src/controllers/editorcontroller.cpp
            src/lexers/qscilexero3prml.cpp
            src/lexers/qscilexero3prml2.cpp
            src/lexers/qscilexero3prmr.cpp
            src/models/project.cpp
            src/models/projectitem.cpp
            src/models/buildmodel.cpp
            src/parsers/abstractparser.cpp
            src/parsers/o3prmlinterpretation.cpp
            src/parsers/o3prmrinterpretation.cpp
            src/uis/advancedsearch.cpp
            src/uis/mainwindow.cpp
            src/uis/newprojectdialog.cpp
            src/uis/projectproperties.cpp
            src/uis/properties.cpp
            src/prmcompleter.cpp
            src/prmtreemodel.cpp
            src/qcommandlineedit.cpp
            src/qkeysequencewidget.cpp
            src/qsciscintillaextended.cpp
            )
SET(TEST_SOURCES src/test/main.cpp
                 src/test/projectTest.cpp
                 src/test/buildmodelTest.cpp
   )
SET(HEADERS src/controllers/buildcontroller.h
            src/controllers/editcontroller.h
            src/controllers/filecontroller.h
            src/controllers/projectcontroller.h
            src/controllers/searchcontroller.h
            src/controllers/viewcontroller.h
            src/controllers/editorcontroller.h
            src/lexers/qscilexero3prml.h
            src/lexers/qscilexero3prml2.h
            src/lexers/qscilexero3prmr.h
            src/models/project.h
            src/models/projectitem.h
            src/models/buildmodel.h
            src/parsers/abstractparser.h
            src/parsers/o3prmlinterpretation.h
            src/parsers/o3prmrinterpretation.h
            src/uis/advancedsearch.h
            src/uis/mainwindow.h
            src/uis/newprojectdialog.h
            src/uis/projectproperties.h
            src/uis/properties.h
            src/prmcompleter.h
            src/prmtreemodel.h
            src/qcommandlineedit.h
            src/qkeysequencewidget.h
            src/qsciscintillaextended.h
    )
FILE(GLOB_RECURSE FORMS *.ui)
FILE(GLOB_RECURSE RESSOURCES *.qrc)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_ADD_RESOURCES(RESOURCES_RCC ${RESSOURCES})

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Checking for a C++11 compiler and adding flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DNDEBUG -O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
FIND_PACKAGE(CXX11)
IF(CXX11_FOUND)
  SET(CMAKE_CXX_FLAGS "${CXX11_FLAGS} ${CMAKE_CXX_FLAGS}")
ELSE()
  MESSAGE(FATAL_ERROR "The aGrUM framework requires C++11. Please use a C++11 compliant compiler.")
ENDIF()

## Building the executable
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

ADD_EXECUTABLE(o3prmeditor ${O3PRMEDITOR_MAIN} ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS} ${RESOURCES_RCC})
TARGET_LINK_LIBRARIES(o3prmeditor ${QT_LIBRARIES} ${QSCINTILLA_LIBRARIES} agrum)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET_TARGET_PROPERTIES(o3prmeditor PROPERTIES LINK_FLAGS "-lstdc++")
endif()

# Test related stuff
ADD_EXECUTABLE(o3prmeditor_test ${TEST_SOURCES} ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS})
TARGET_LINK_LIBRARIES(o3prmeditor_test gmock gtest pthread ${QT_LIBRARIES} ${QSCINTILLA_LIBRARIES} agrum)  #${Boost_LIBRARIES}
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET_TARGET_PROPERTIES(o3prmeditor_test PROPERTIES LINK_FLAGS "-lstdc++")
endif()

