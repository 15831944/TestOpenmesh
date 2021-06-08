#ifndef NCBALL_H
#define NCBALL_H

#include "Vector3d.h"

class NCBall
{
public:
    NCBall(const CVector3d &center, double radius);
    ~NCBall();

    float *stripPoints();
    float *stripNormals();
    size_t stripSize() {return m_size;}

    void draw();

protected:
    void computerDate();

protected:
    float *m_stripPoints = nullptr;
    float *m_stripNormals = nullptr;
    size_t m_size;
    double m_radius;
    CVector3d m_center;
};


#endif // NCBALL_H
