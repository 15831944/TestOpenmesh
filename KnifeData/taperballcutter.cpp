#include "taperballcutter.h"

TaperBallCutter::TaperBallCutter(const QString &name)
    :ToolData(name, TBC_END)
{

}

QString TaperBallCutter::GetDes(size_t i)
{
    QString desStr;
    switch (i)
    {
    case TaperBallCutter::TBC_LENGTH:
        desStr = QObject::tr("TBC Length");
        break;
    case TaperBallCutter::TBC_DIA:
        desStr = QObject::tr("TBC Dia");
        break;
    case TaperBallCutter::TBC_HANDLE_LEN:
        desStr = QObject::tr("TBCHandle Length");
        break;
    case TaperBallCutter::TBC_HANDLE_DIA:
        desStr = QObject::tr("TBCHandle Dia");
        break;
    case TaperBallCutter::TBC_HANDLE_DIS:
        desStr = QObject::tr("TBCHandle Dis");
        break;
    case TaperBallCutter::TBC_ANGLE:
        desStr = QObject::tr("TBC Angle");
        break;
    case TaperBallCutter::TBC_TIP_RADIUS:
        desStr = QObject::tr("TBCTip Angle");
        break;
    default:
        break;
    }
    return desStr;
}

void TaperBallCutter::GetDataDes(std::vector<QString> &dataDes)
{
    dataDes.resize(TBC_END);
    for(size_t i = 0; i < TBC_END; i++)
    {
        dataDes[i] = GetDes(i);
    }
}

void TaperBallCutter::Initial()
{
    SetValue(TBC_LENGTH, "60");
    SetValue(TBC_DIA, "10");
    SetValue(TBC_HANDLE_LEN, "30");
    SetValue(TBC_HANDLE_DIA, "45");
    SetValue(TBC_HANDLE_DIS, "5");
    SetValue(TBC_ANGLE, "10");
    SetValue(TBC_TIP_RADIUS, "1");
}
