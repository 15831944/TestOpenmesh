#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <string>
#include <memory>

using namespace std;

#include <QDebug>

#include "Geometry/nctriaopenmesh.h"
#include "Geometry/Util/geometricutil.h"
#include "KnifeData/nctoolset.h"
#include "Geometry/NCPath.h"

typedef shared_ptr<NCTriaOpenMesh> NCTriaMeshSPtr;

typedef shared_ptr<NCPath> NCPathSPtr;


extern NCTriaMeshSPtr   g_pMesh;
extern NCKnifeSetSPtr   g_pKnifeSet;
extern ToolDataSPtr     g_pBlank;
extern vector<NCPath>   g_paths;
extern CVector3d        XDIR;
extern CVector3d        YDIR;
extern CVector3d        ZDIR;
class Initial{

public:
    Initial();

    ~Initial();
};

enum LA
{
    CHINESE,
    ENGLISH,
};

bool SwitchLanguage(LA nLa);

enum IN_BIT
{
    IN_EMG,
    IN_START,
    IN_PAUSE,
    IN_STOP,
    IN_X_NIL,
    IN_Y_NIL,
    IN_Z_NIL,
    IN_7,
    IN_LIMIT_X_POS,
    IN_LIMIT_X_NEG,
    IN_LIMIT_Y_POS,
    IN_LIMIT_Y_NEG,
    IN_LIMIT_Z_POS,
    IN_LIMIT_Z_NEG,
    IN_14,
    IN_15,
    IN_MASTER_SPEED_REACHED,
    IN_MASTER_ERROR,
    IN_PRESSURE_ERROR,
    IN_Y_LOCATION,
    IN_20,
    IN_21,
    IN_22,
    IN_23,
    IN_SERVO_WARN_X,
    IN_SERVO_WARN_Y,
    IN_SERVO_WARN_Z,
    IN_27,
    IN_28,
    IN_HANDWHEEL_X1 = 38,
    IN_HANDWHEEL_X10,
    IN_HANDWHEEL_X100,
    IN_HAND_SELECT_X = 41,
    IN_HAND_SELECT_Y,
    IN_HAND_SELECT_Z,
};


enum OUT_BIT
{
    OUT_ROTATE_POS,
    OUT_ROTATE_NEG,
    OUT_RESET_WARN,
    OUT_WORK,
    OUT_WARN,
    OUT_IDLE,
    OUT_CLAMP,
    OUT_PRESS,
};

enum NCOCCMessage
{

};

bool check(const std::string &fileName,
           const std::string &fileSuffix);

#endif // PCH_H
