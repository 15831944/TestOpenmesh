#ifndef CYLINDER_H
#define CYLINDER_H

#include "NCPath.h"

class Cylinder
{
public:
    Cylinder(double radius, double height, const CVector3d &center, const CVector3d &dir);
    ~Cylinder();

    float *stripPoints() {return m_stripPoints;}
    float *stripNormals(){return m_stripNormals;}
    size_t stripSize() {return m_size;}

    void draw();

    void Set(double radius, double height, const CVector3d &center, const CVector3d &dir);

protected:
    void computerDate();

private:
    float *m_stripPoints = nullptr;
    float *m_stripNormals = nullptr;
    size_t m_size;

    double m_radius;
    double m_height;
    CVector3d m_center;
    CVector3d m_dir;
};

#endif // CYLINDER_H
