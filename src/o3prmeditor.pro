#-------------------------------------------------
#
# Project created by QtCreator 2010-11-05T11:42:49
#
#-------------------------------------------------

TEMPLATE = app

QT     += core gui

CONFIG  += qt thread resources uic windows x11 exceptions stl debug_and_release warn_on

win32 {
	# Pour la dll d'agrum
	LIBS += -L$${PWD}/../bin/
	INCLUDEPATH += $${PWD}/../../../src/
}

!win32:debug {
	LIBS += -lqscintilla2 -lagrum-dbg
} else {
	LIBS += -lqscintilla2 -lagrum
}

TARGET = o3prmeditor
DESTDIR = $${PWD}/../bin/Release

SOURCES += \
    controllers/buildcontroller.cpp \
    controllers/editcontroller.cpp \
    controllers/filecontroller.cpp \
    controllers/projectcontroller.cpp \
    controllers/searchcontroller.cpp \
    controllers/viewcontroller.cpp \
    uis/advancedsearch.cpp \
	uis/mainwindow.cpp \
	uis/newprojectdialog.cpp \
	uis/projectproperties.cpp \
	uis/properties.cpp \
	lexers/qscilexero3prml2.cpp \
	lexers/qscilexero3prmr.cpp \
	parsers/abstractparser.cpp \
	parsers/o3prmlinterpretation.cpp \
	parsers/o3prmrinterpretation.cpp \
	main.cpp\
	prmcompleter.cpp \
	prmtreemodel.cpp \
	project.cpp \
	qcommandlineedit.cpp \
	qkeysequencewidget.cpp \
	qsciscintillaextended.cpp

HEADERS  += \
    controllers/buildcontroller.h \
    controllers/editcontroller.h \
    controllers/filecontroller.h \
    controllers/projectcontroller.h \
    controllers/searchcontroller.h \
    controllers/viewcontroller.h \
    uis/advancedsearch.h \
    uis/mainwindow.h \
    uis/newprojectdialog.h \
    uis/projectproperties.h \
    uis/properties.h \
    lexers/qscilexero3prml2.h \
    lexers/qscilexero3prmr.h \
    parsers/abstractparser.h \
    parsers/o3prmlinterpretation.h \
    parsers/o3prmrinterpretation.h \
    prmcompleter.h \
    prmtreemodel.h \
    project.h \
    qcommandlineedit.h \
    qkeysequencewidget.h \
    qsciscintillaextended.h

FORMS    += \
    uis/advancedsearch.ui \
    uis/mainwindow.ui \
    uis/newprojectdialog.ui \
    uis/projectproperties.ui
    uis/properties.ui \

RESOURCES += \
	../rsrc/resources.qrc
