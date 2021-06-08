#ifndef GEOMETRICUTIL_H
#define GEOMETRICUTIL_H

#include "clipper.hpp"
#include "Vector3d.h"

using namespace ClipperLib;

#define CLIPPERSCALE 100000000.0


class GeometricUtil
{
public:
    static void Clipper2Polygon(Path& path, NCPath& poly);
    static void Polygon2Clipper(const NCPath& poly, Path& path);

    static void OffsetPath(const NCPath &poly, double offset, vector<NCPath>& res);
};

#endif // GEOMETRICUTIL_H
