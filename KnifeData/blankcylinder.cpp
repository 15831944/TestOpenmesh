#include "blankcylinder.h"

#include<QStringList>

#include "Geometry/cylinder.h"

BlankCylinder::BlankCylinder(const QString &blankName)
    :ToolData(blankName, CYLINDER_END)
{

}

QString BlankCylinder::GetDes(size_t i)
{
    QString desStr;
    switch (i)
    {
    case BlankCylinder::CYLINDER_HEIGHT:
        desStr = QObject::tr("Cylinder Height");
        break;
    case BlankCylinder::CYLINDER_DIA:
        desStr = QObject::tr("Cylinder Dia");
        break;
    case BlankCylinder::CYLINDER_XCENTER:
        desStr = QObject::tr("Cylinder XCenter");
        break;
    case BlankCylinder::CYLINDER_YCENTER:
        desStr = QObject::tr("Cylinder YCenter");
        break;
    case BlankCylinder::CYLINDER_ZCENTER:
        desStr = QObject::tr("Cylinder ZCenter");
        break;
    default:
        break;
    }
    return desStr;
}

void BlankCylinder::GetDataDes(std::vector<QString> &dataDes)
{
    dataDes.resize(CYLINDER_END);
    for(size_t i = 0; i < CYLINDER_END; i++)
    {
        dataDes[i] = GetDes(i);
    }
}

void BlankCylinder::Draw()
{
    CVector3d center;
    center[0] = GetValue(CYLINDER_XCENTER).toDouble();
    center[1] = GetValue(CYLINDER_YCENTER).toDouble();
    center[2] = GetValue(CYLINDER_ZCENTER).toDouble();
    CVector3d dir(0, 1, 0);
    double radius = GetValue(CYLINDER_DIA).toDouble()/2;
    double height = GetValue(CYLINDER_HEIGHT).toDouble();
    Cylinder c(radius, height, center, dir);
    c.draw();
}
