#include "ballendcutter.h"

BallEndCutter::BallEndCutter(const QString &name)
    :ToolData(name, BALLCUT_END)
{
    Initial();
}

QString BallEndCutter::GetDes(size_t i)
{
    QString desStr;
    switch (i)
    {
    case BallEndCutter::BALLCUT_LENGTH:
        desStr = QObject::tr("BallEndCutter Length");
        break;
    case BallEndCutter::BALLCUT_DIA:
        desStr = QObject::tr("BallEndCutter Dia");
        break;
    case BallEndCutter::BALLCUT_HANDLE_LEN:
        desStr = QObject::tr("BallEndCutterHandle Length");
        break;
    case BallEndCutter::BALLCUT_HANDLE_DIA:
        desStr = QObject::tr("BallEndCutterHandle Dia");
        break;
    case BallEndCutter::BALLCUT_HANDLE_DIS:
        desStr = QObject::tr("BallEndCutterHandle Dis");
        break;
    default:
        break;
    }
    return desStr;
}

void BallEndCutter::GetDataDes(std::vector<QString> &dataDes)
{
    dataDes.resize(BALLCUT_END);
    for(size_t i = 0; i < BALLCUT_END; i++)
    {
        dataDes[i] = GetDes(i);
    }
}

void BallEndCutter::Initial()
{
    SetValue(BALLCUT_LENGTH, "60");
    SetValue(BALLCUT_DIA, "10");
    SetValue(BALLCUT_HANDLE_LEN, "30");
    SetValue(BALLCUT_HANDLE_DIA, "45");
    SetValue(BALLCUT_HANDLE_DIS, "5");
}
