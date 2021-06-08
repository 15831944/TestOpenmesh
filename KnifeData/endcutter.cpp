#include "endcutter.h"

EndCutter::EndCutter(const QString& name)
    :ToolData(name, ENDCUT_END)
{
    Initial();
}

QString EndCutter::GetDes(size_t i)
{
    QString desStr;
    switch (i)
    {
    case EndCutter::ENDCUT_LENGTH:
        desStr = QObject::tr("EndMilling Length");
        break;
    case EndCutter::ENDCUT_DIA:
        desStr = QObject::tr("EndMilling Dia");
        break;
    case EndCutter::ENDCUT_HANDLE_LEN:
        desStr = QObject::tr("EndMillingHandle Length");
        break;
    case EndCutter::ENDCUT_HANDLE_DIA:
        desStr = QObject::tr("EndMillingHandle Dia");
        break;
    case EndCutter::ENDCUT_HANDLE_DIS:
        desStr = QObject::tr("EndMillingHandle Dis");
        break;
    default:
        break;
    }
    return desStr;
}

void EndCutter::GetDataDes(std::vector<QString> &dataDes)
{
    dataDes.resize(ENDCUT_END);
    for(size_t i = 0; i < ENDCUT_END; i++)
    {
        dataDes[i] = GetDes(i);
    }
}

void EndCutter::Initial()
{
    SetValue(ENDCUT_LENGTH, "60");
    SetValue(ENDCUT_DIA, "10");
    SetValue(ENDCUT_HANDLE_LEN, "30");
    SetValue(ENDCUT_HANDLE_DIA, "45");
    SetValue(ENDCUT_HANDLE_DIS, "5");
}
