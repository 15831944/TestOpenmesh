#include "NCPath.h"


CVector3d &NCPath::operator[](size_t i)
{
    return m_points[i];
}

void NCPath::push_back(const CVector3d &point)
{
    return m_points.push_back(point);
}

void NCPath::fromCircle(const double &radius, const double &eps, const CVector3d &center)
{
    CVector3d pts[4] =
    {
        CVector3d(-radius, 0, 0),
        CVector3d(0, radius, 0),
        CVector3d(radius, 0, 0),
        CVector3d(0, -radius, 0),
    };

    clear();

    double reps = (radius-eps)*(radius-eps);

    for(int i =0, j = 3 ; i < 4; j = i++)
    {
        push_back(pts[j]);
        GetNextCirclePt( radius, reps, pts[j], pts[i]);
    }
    push_back(pts[3]);

    for(size_t i = 0; i < size(); ++i)
    {
        m_points[i][0] += center[0];
        m_points[i][1] += center[1];
        m_points[i][2] += center[2];
    }
}

void NCPath::GetNextCirclePt(double r, double reps, const CVector3d &p1, const CVector3d &p2)
{
    CVector3d pt = 0.5 * (p1 + p2);
    if((r*r - (pt - p1).LengthSquared()) > reps)
    {
        return;
    }
    pt.Normalize();
    pt *= r;
    GetNextCirclePt(r, reps, p1, pt);
    m_points.push_back(pt);
    GetNextCirclePt(r, reps, pt, p2);
}
