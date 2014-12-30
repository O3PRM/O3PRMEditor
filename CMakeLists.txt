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
FIND_PACKAGE(Qt4 REQUIRED)
SET(LIBS ${LIBS} ${QT_LIBRARIES})

FIND_PACKAGE(Qscintilla REQUIRED)
INCLUDE_DIRECTORIES(${QSCINTILLA_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${QSCINTILLA_LIBRARIES})

INCLUDE(ExternalProject)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    ExternalProject_Add(AGRUM-SRC
        PREFIX aGrUM
        GIT_REPOSITORY file:////Users/lto/Projets/aGrUM-dev
        GIT_TAG static_lib
        CONFIGURE_COMMAND ""
        UPDATE_COMMAND ""
        BUILD_COMMAND ./act install release -p linux -j 7 -m PRM -d <INSTALL_DIR>
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
    )
else()
    ExternalProject_Add(AGRUM-SRC
        PREFIX aGrUM
        GIT_REPOSITORY file:////Users/lto/Projets/aGrUM-dev
        GIT_TAG master
        CONFIGURE_COMMAND ""
        UPDATE_COMMAND ""
        BUILD_COMMAND ./act install release -p linux -j 7 -m PRM -d <INSTALL_DIR>
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
    )
endif()

SET(AGRUM_INCLUDE_DIR "${PROJECT_BINARY_DIR}/aGrUM/include/")
SET(AGRUM_LIB_DIR "${PROJECT_BINARY_DIR}/aGrUM/lib/")
SET(LIBS ${LIBS} agrum)

INCLUDE_DIRECTORIES(${AGRUM_INCLUDE_DIR})
LINK_DIRECTORIES(${AGRUM_LIB_DIR})

ExternalProject_add(GMOCK
    PREFIX gmock
    SVN_REPOSITORY http://googlemock.googlecode.com/svn/trunk/
    INSTALL_COMMAND mkdir -p <INSTALL_DIR>/lib <INSTALL_DIR>/include
        COMMAND cp -R -f <INSTALL_DIR>/src/GMOCK/include/gmock <INSTALL_DIR>/include
        COMMAND cp -f <BINARY_DIR>/libgmock.a <INSTALL_DIR>/lib
        COMMAND cp -R -f <INSTALL_DIR>/src/GMOCK/gtest/include/gtest <INSTALL_DIR>/include
        COMMAND cp -f <BINARY_DIR>/gtest/libgtest.a <INSTALL_DIR>/lib
)

#ExternalProject_add(GMOCK
#    PREFIX gmock
#    SVN_REPOSITORY http://googlemock.googlecode.com/svn/trunk/
#    CONFIGURE_COMMAND /usr/local/bin/autoreconf -fvi COMMAND ./configure --prefix=<INSTALL_DIR>
#    UPDATE_COMMAND ""
#    BUILD_COMMAND make
#    BUILD_IN_SOURCE 1
#    INSTALL_COMMAND mkdir -p <INSTALL_DIR>/lib <INSTALL_DIR>/include
#        COMMAND cp -R -f include/gmock <INSTALL_DIR>/include
#        COMMAND cp -f lib/libgmock.la lib/libgmock_main.la <INSTALL_DIR>/lib
#        COMMAND cp -R -f gtest/include/gtest <INSTALL_DIR>/include
#        COMMAND cp -R -f gtest/lib/libgtest.la gtest/lib/libgtest_main.la <INSTALL_DIR>/lib
#)

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
            src/lexers/qscilexero3prml.cpp
            src/lexers/qscilexero3prml2.cpp
            src/lexers/qscilexero3prmr.cpp
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
            src/project.cpp
            src/qcommandlineedit.cpp
            src/qkeysequencewidget.cpp
            src/qsciscintillaextended.cpp
)
SET(HEADERS src/controllers/buildcontroller.h
            src/controllers/editcontroller.h
            src/controllers/filecontroller.h
            src/controllers/projectcontroller.h
            src/controllers/searchcontroller.h
            src/controllers/viewcontroller.h
            src/lexers/qscilexero3prml.h
            src/lexers/qscilexero3prml2.h
            src/lexers/qscilexero3prmr.h
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
            src/project.h
            src/qcommandlineedit.h
            src/qkeysequencewidget.h
            src/qsciscintillaextended.h
)
FILE(GLOB_RECURSE FORMS *.ui)
FILE(GLOB_RECURSE RESSOURCES *.qrc)
#FILE(GLOB_RECURSE RC_FILE *.rc)

# Isolating test files
#LIST(REMOVE_ITEM ${O3PRM_SOURCES} projectTest.cpp)
#FILE(GLOB_RECURSE O3PRM_TEST_SOURCES src/test/main.cpp src/test/projectTest.cpp)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_ADD_RESOURCES(RESOURCES_RCC ${RESSOURCES})

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Checking for a C++11 compiler
FIND_PACKAGE(CXX11)
IF(CXX11_FOUND)
  SET(O3PRMEDITOR_CXX_FLAGS "${CXX11_FLAGS} ${O3PRM_CXX_FLAGS}")
ELSE()
  MESSAGE(FATAL_ERROR "The aGrUM framework requires C++11. Please use a C++11 compliant compiler.")
ENDIF()

# Compiling flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${O3PRMEDITOR_CXX_FLAGS} -Wall -DNDEBUG")

## Building the executable
ADD_EXECUTABLE(o3prmeditor ${O3PRMEDITOR_MAIN} ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS})
TARGET_LINK_LIBRARIES(o3prmeditor ${QT_LIBRARIES} ${QSCINTILLA_LIBRARIES} agrum)
#TARGET_LINK_LIBRARIES(o3prmeditor ${LIBS})
SET_TARGET_PROPERTIES(o3prmeditor PROPERTIES LINK_FLAGS "-lstdc++")

# Test related stuff
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
ADD_EXECUTABLE(o3prmeditor_test src/test/main.cpp src/test/projectTest.cpp)
TARGET_LINK_LIBRARIES(o3prmeditor_test gmock)
TARGET_LINK_LIBRARIES(o3prmeditor_test gtest)
SET_TARGET_PROPERTIES(o3prmeditor_test PROPERTIES LINK_FLAGS "-lstdc++")
