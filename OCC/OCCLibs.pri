
CASROOT = C:\OpenCASCADE-7.5.0-vc14-64\opencascade-7.5.0

win32 {
    DEFINES +=  \
        WNT
    INCLUDEPATH +=  \
        $$CASROOT/inc \
        message("CASROOT:" $$CASROOT/inc)

    win32-msvc2010 {
        compiler=vc10
    }

    win32-msvc2012 {
        compiler=vc11
    }

    win32-msvc2013 {
        compiler=vc12
    }

    win32-msvc2015 {
        compiler=vc14
    }

    win32-msvc {
        compiler=vc14
    }


    # Determine 32 / 64 bit and debug / release build
    !contains(QMAKE_TARGET.arch, x86_64) {
        CONFIG(debug, debug|release) {
            message("Debug 32 build")
            LIBS += -L$$CASROOT/win32/$$compiler/lib
            DEPENDPATH += -L$$CASROOT/win32/$$compiler/lib
        }
        else {
            message("Release 32 build")
            LIBS += -L$$CASROOT/win32/$$compiler/lib
        }
    }
    else {
        CONFIG(debug, debug|release) {
           message("Debug 64 build")
           LIBS += -L$$CASROOT/win64/$$compiler/lib
           message("$$(CASROOT)/win64/$$compiler/lib")
        }
        else {
            message("Release 64 build")
            LIBS += -L$$CASROOT/win64/$$compiler/lib
        }
        DEPENDPATH += -L$$CASROOT/win64/$$compiler/bin
    }
}

linux-g++ {
    INCLUDEPATH +=  \
        /usr/local/include/opencascade

    LIBS += \
        -L/usr/local/lib/
}

LIBS +=         \
    -lTKernel   \
    -lTKMath    \
    -lTKPrim    \
    -lTKBool    \
    -lTKOffset  \
    -lTKStl     \
    -lTKMesh    \
    -lTKSTEP    \
    -lTKXSBase  \
    -lTKBRep    \
    -lTKG3d     \
    -lTKTopAlgo


