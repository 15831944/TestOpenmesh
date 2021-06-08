#include "geometricutil.h"


void GeometricUtil::Clipper2Polygon(Path &path, NCPath &poly)
{
    poly.clear();
    Path::iterator iter;
    for (iter = path.begin(); iter != path.end(); ++iter)
    {
        poly.push_back(CVector3d(iter->X/CLIPPERSCALE, iter->Y/CLIPPERSCALE, 0));
    }
}

void GeometricUtil::Polygon2Clipper(const NCPath &poly, Path &path)
{
    path.clear();

    for (auto iter= poly.cbegin(); iter != poly.cend(); ++iter)
    {
        path.push_back(IntPoint(iter->x() * CLIPPERSCALE, iter->y() * CLIPPERSCALE));
    }
}

void GeometricUtil::OffsetPath(const NCPath &poly, double offset, vector<NCPath>& res)
{
    res.clear();
    if(offset < FLT_EPSILON)
    {
        res.push_back(poly);
        return;
    }
    Path path, resPath;
    Polygon2Clipper(poly, path);

    IntPoint p0 = path.front();
    IntPoint p1 = path.back();
    p0.X -= 500*CLIPPERSCALE;
    p1.X -= 500*CLIPPERSCALE;
    path.push_back(p1);
    path.push_back(p0);

    Paths sol;
    ClipperOffset co(0.1);
    co.AddPath(path, jtMiter, etClosedPolygon);
    co.Execute(sol, offset*CLIPPERSCALE);
    res.resize(sol.size());

    //---------------------------
    cInt xmin = max(p0.X, p1.X);

    sol[0].push_back(sol[0].front());

    enum{
        NONE,
        PUSH,
        INSERT,
    };
    int nRes = NONE;

    IntPoint p2 = sol[0][0];
    bool preCross = p2.X > xmin;

    if(preCross)
    {
        nRes++;
        resPath.push_back(p2);
    }

    size_t index = 0;
    for(size_t i = 1; i < sol[0].size(); i++)
    {
        IntPoint p2 = sol[0][i];
        bool  curCrossRes = p2.X > xmin;

        if(curCrossRes == true &&  preCross == false)
        {
            nRes++;
        }
        preCross = curCrossRes;

        if(curCrossRes)
        {
            if(nRes == PUSH)
            {
                resPath.push_back(p2);
            }
            else if(nRes == INSERT)
            {
                resPath.insert(resPath.begin()+index++, p2);
            }
        }
    }

    Clipper2Polygon(resPath, res[0]);
//    Clipper2Polygon(sol[0], res[0]);

    //-----------------------------




//    for (size_t i = 0; i < sol.size(); i++)
//    {
//        sol[i].push_back(sol[i].front());
//        Clipper2Polygon(sol[i], res[i]);
//    }

}
