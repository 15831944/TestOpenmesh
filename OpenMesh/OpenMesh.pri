

INCLUDEPATH += $$PWD/. \
               $$PWD/..

DEFINES += _USE_MATH_DEFINES ACGMAKE_STATIC_BUILD

win32{
    CONFIG(release, debug|release){
        LIBS += -L$$PWD/lib/release/ -lOpenMeshTools \
                        -L$$PWD/lib/release/ -lOpenMeshCore

        DEPENDPATH += $$PWD/lib/release
    }
    CONFIG(debug, debug|release){
        LIBS += -L$$PWD/lib/debug/ -lOpenMeshToolsd \
                        -L$$PWD/lib/debug/ -lOpenMeshCored

        DEPENDPATH += $$PWD/lib/release
    }


    !win32-g++{
        CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/release/OpenMeshTools.lib
        CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/release/OpenMeshCore.lib
        CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/debug/OpenMeshCored.lib
        CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/debug/OpenMeshToolsd.lib
    }
}
