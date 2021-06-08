#ifndef BLANKCYLINDER_H
#define BLANKCYLINDER_H

#include <QString>

#include "tooldata.h"

class BlankCylinder : public ToolData
{
public:
    enum CylinderData
    {
        CYLINDER_BEGIN,
        CYLINDER_HEIGHT = CYLINDER_BEGIN,
        CYLINDER_DIA,
        CYLINDER_XCENTER,
        CYLINDER_YCENTER,
        CYLINDER_ZCENTER,
        CYLINDER_END
    };
public:
    BlankCylinder(const QString& blankName = "Cylinder");

    QString GetDes(size_t i) override;
    void GetDataDes(std::vector<QString> &dataDes) override;
    void Draw() override;
};

#endif // BLANKCYLINDER_H
