QT       += core gui opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 precompile_header console

#指定Moc_xxx.cxx文件，xxx.obj文件目录
MOC_DIR = $$OUT_PWD/MocDir
OBJECTS_DIR = $$OUT_PWD/ObjectsDir

#Release生成时，exe文件生成目录设置
CONFIG(release, debug|release): DESTDIR = $$PWD/Bin/Release
CONFIG(debug, debug|release): DESTDIR = $$PWD/Bin/Debug


PRECOMPILED_HEADER = pch.h

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(ArcBall/ArcBall.pri)
include(OpenMesh/OpenMesh.pri)
include(LibClipper/Clipper.pri)
include(OCC/OCCLibs.pri)

SOURCES += \
    GCode/NCAction.cpp \
    Geometry/NCPath.cpp \
    Geometry/Util/geometricutil.cpp \
    Geometry/cylinder.cpp \
    Geometry/ncball.cpp \
    Geometry/nccube.cpp \
    Geometry/nctriamesh.cpp \
    Geometry/nctriaopenmesh.cpp \
    KnifeData/ballendcutter.cpp \
    KnifeData/blankbox.cpp \
    KnifeData/blankcylinder.cpp \
    KnifeData/endcutter.cpp \
    KnifeData/nctoolset.cpp \
    KnifeData/taperballcutter.cpp \
    KnifeData/tooldata.cpp \
    Widget/BlankDlg/blankboxdlg.cpp \
    Widget/BlankDlg/blankcylinderdlg.cpp \
    Widget/MeshViewer/MeshViewerWidget.cpp \
    Widget/MeshViewer/QGLViewerWidget.cpp \
    Widget/kwctrlview.cpp \
    Widget/ncoccctrlview.cpp \
    Widget/ncpathdialog.cpp \
    Widget/nctreewidgetitem.cpp \
    main.cpp \
    mainwindow.cpp \
    pch.cpp

HEADERS += \
    GCode/NCAction.h \
    Geometry/NCPath.h \
    Geometry/Util/geometricutil.h \
    Geometry/cylinder.h \
    Geometry/ncball.h \
    Geometry/nccube.h \
    Geometry/nctriamesh.h \
    Geometry/nctriaopenmesh.h \
    KnifeData/ballendcutter.h \
    KnifeData/blankbox.h \
    KnifeData/blankcylinder.h \
    KnifeData/endcutter.h \
    KnifeData/nctoolset.h \
    KnifeData/taperballcutter.h \
    KnifeData/tooldata.h \
    Widget/BlankDlg/blankboxdlg.h \
    Widget/BlankDlg/blankcylinderdlg.h \
    Widget/MeshViewer/MeshViewerWidget.h \
    Widget/MeshViewer/MeshViewerWidgetT.h \
    Widget/MeshViewer/MeshViewerWidgetT_impl.h \
    Widget/MeshViewer/QGLViewerWidget.h \
    Widget/kwctrlview.h \
    Widget/ncoccctrlview.h \
    Widget/ncpathdialog.h \
    Widget/nctreewidgetitem.h \
    mainwindow.h \
    pch.h

FORMS += \
    Widget/BlankDlg/blankboxdlg.ui \
    Widget/BlankDlg/blankcylinderdlg.ui \
    Widget/ncpathdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore



win32:LIBS += -lopengl32 -lglu32
