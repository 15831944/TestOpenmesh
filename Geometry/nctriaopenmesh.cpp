#include "nctriaopenmesh.h"

// == openmesh ==========================
#include <OpenMesh/Core/IO/MeshIO.hh> // 读取文件
#include <OpenMesh/Core/IO/importer/BaseImporter.hh>
#include <OpenMesh/Core/IO/reader/STLReader.hh>

#include "Quaternion.h"

// == OCC ================================
#include <RWStl_Reader.hxx>
#include <STEPControl_Reader.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <StlAPI_Reader.hxx>

#include <TopoDS.hxx>
//#include <BRepMesh.hxx>
#include <BRepLib.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Interface_Static.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <XBRepMesh.hxx>

class CmpVec {
public:
    explicit CmpVec(float _eps = FLT_MIN)
        : eps_(_eps)
    {
    }

    bool operator()(const Vec3f& _v0, const Vec3f& _v1) const
    {
        if (fabs(_v0[0] - _v1[0]) <= eps_) {
            if (fabs(_v0[1] - _v1[1]) <= eps_) {
                return (_v0[2] < _v1[2] - eps_);
            } else
                return (_v0[1] < _v1[1] - eps_);
        } else
            return (_v0[0] < _v1[0] - eps_);
    }

private:
    float eps_;
};

//using namespace OpenMesh;

static const size_t THE_STL_HEADER_SIZE = 84;
static const size_t THE_STL_SIZEOF_FACET = 50;
static const size_t THE_STL_MIN_FILE_SIZE = THE_STL_HEADER_SIZE + THE_STL_SIZEOF_FACET;

OpenMesh::IO::STL_Type checkType(const std::string& fileName);

NCTriaOpenMesh::NCTriaOpenMesh()
{
    OpenMesh::IO::STLReader().setCheckFunc(&checkType);
    OpenMesh::IO::STLReader().set_epsilon(FLT_EPSILON);

    //    if(!Interface_Static::SetIVal("read.step.product.mode", 0))
    //    {
    //        cerr << "ERROR" << std::endl;
    //    }
}

bool NCTriaOpenMesh::LoadFile(const string& fileName)
{
    if (check(fileName, "stp") || check(fileName, "step")) {
        return ReadStepFile(fileName);
    }

    // 请求顶点法线 vertex normals
    m_mesh.request_vertex_normals();
    m_mesh.request_face_normals();
    //如果不存在顶点法线，则报错
    if (!m_mesh.has_vertex_normals()) {
        cout << "错误：标准定点属性 “法线”不存在" << std::endl;
        return false;
    }
    // 如果有顶点发现则读取文件
    OpenMesh::IO::Options opt;
    bool res = OpenMesh::IO::read_mesh(m_mesh, fileName, opt);
    if (!res) {
        cout << "无法读取文件:" << fileName << std::endl;
        return false;
    }

    m_meshName = getBasicName(fileName);

    // 通过面法线计算顶点法线
    m_mesh.request_face_normals();
    // mesh计算出顶点法线
    m_mesh.update_normals();
    // 释放面法线
    m_mesh.release_face_normals();

    return true;
}

bool NCTriaOpenMesh::SaveFile(const string& fileName)
{
    return OpenMesh::IO::write_mesh(m_mesh, fileName);
}

bool NCTriaOpenMesh::ReadStepFile(const string& fileName)
{
    m_mesh.request_vertex_normals();
    m_mesh.request_face_normals();
    m_mesh.clear();
    Standard_CString thePath = Standard_CString(fileName.c_str());
    auto reader = STEPControl_Reader();
    auto status = reader.ReadFile(thePath);
    reader.PrintCheckLoad(Standard_False, IFSelect_ListByItem);

    if (status == IFSelect_RetDone) {
        Standard_Integer num = reader.TransferRoots();
        reader.PrintCheckTransfer(Standard_False, IFSelect_ListByItem);

        Standard_Integer nb = reader.NbShapes();
        TopExp_Explorer faceExplorer;

        std::map<Vec3f, VertexHandle, CmpVec> vMap;
        std::map<Vec3f, VertexHandle, CmpVec>::iterator vMapIt;
        OpenMesh::Vec3f v[3], n, norm[3];
        OpenMesh::IO::BaseImporter::VHandles vhandles;
        OpenMesh::IO::ImporterT<MyMesh> _bi(m_mesh);
        _bi.prepare();

        auto aShape = reader.OneShape();

        //生成三角面片
        BRepMesh_FastDiscret::Parameters aMeshParams;
        aMeshParams.Deflection = 0.1;
        aMeshParams.Angle = 0.1;
        aMeshParams.Relative = Standard_False;
        aMeshParams.InParallel = Standard_True;
        aMeshParams.MinSize = 0.1;
        aMeshParams.InternalVerticesMode = Standard_True;
        aMeshParams.ControlSurfaceDeflection = Standard_True;
        //        aMeshParams.AdaptiveMin = Standard_False;

        BRepMesh_IncrementalMesh aMesher(aShape, aMeshParams);
        cout << "Meshing statuses: ";
        Standard_Integer statusFlags = aMesher.GetStatusFlags();
        if (!statusFlags) {
            cout << "NoError";
        } else {
            Standard_Integer i;
            for (i = 0; i < 4; i++) {
                if ((statusFlags >> i) & (Standard_Integer)1) {
                    switch (i + 1) {
                    case 1:
                        cout << "OpenWire ";
                        break;
                    case 2:
                        cout << "SelfIntersectingWire ";
                        break;
                    case 3:
                        cout << "Failure ";
                        break;
                    case 4:
                        cout << "ReMesh ";
                        break;
                    }
                }
            }
        }
        BRepLib::EnsureNormalConsistency(aShape);

        for (faceExplorer.Init(aShape, TopAbs_FACE); faceExplorer.More(); faceExplorer.Next()) {
            TopoDS_Face aFace = TopoDS::Face(faceExplorer.Current());

            TopLoc_Location loc = aFace.Location();
            Handle_Poly_Triangulation triFace = BRep_Tool::Triangulation(aFace, loc);
            if (triFace.IsNull())
                continue;
            Standard_Size nTriangles = triFace->NbTriangles();

            Standard_Integer nVertexIndex1 = 0;
            Standard_Integer nVertexIndex2 = 0;
            Standard_Integer nVertexIndex3 = 0;

            TColgp_Array1OfPnt nodes(1, triFace->NbNodes());
            Poly_Array1OfTriangle triangles(1, triFace->NbTriangles());
            nodes = triFace->Nodes();
            triangles = triFace->Triangles();

            for (Standard_Size i = 1; i <= nTriangles; i++) {
                const Poly_Triangle& aTriangle = triangles.Value(i);
                aTriangle.Get(nVertexIndex1, nVertexIndex2, nVertexIndex3);

                auto occV1 = nodes.Value(nVertexIndex1);
                auto occV2 = nodes.Value(nVertexIndex2);
                auto occV3 = nodes.Value(nVertexIndex3);

                auto occN1 = triFace->Normal(nVertexIndex1);
                auto occN2 = triFace->Normal(nVertexIndex2);
                auto occN3 = triFace->Normal(nVertexIndex3);

                v[0] = OpenMesh::Vec3d(occV1.X(), occV1.Y(), occV1.Z());
                v[1] = OpenMesh::Vec3d(occV2.X(), occV2.Y(), occV2.Z());
                v[2] = OpenMesh::Vec3d(occV3.X(), occV3.Y(), occV3.Z());

                norm[0] = OpenMesh::Vec3d(occN1.X(), occN1.Y(), occN1.Z());
                norm[1] = OpenMesh::Vec3d(occN2.X(), occN2.Y(), occN2.Z());
                norm[2] = OpenMesh::Vec3d(occN3.X(), occN3.Y(), occN3.Z());

                auto v10 = v[1] - v[0];
                auto v12 = v[2] - v[1];
                n = v10.cross(v12);
                n.normalized();

                vhandles.clear();
                for (size_t j = 0; j < 3; ++j) {
                    // has vector been referenced before?
                    if ((vMapIt = vMap.find(v[j])) == vMap.end()) {
                        // No : add vertex and remember idx/vector mapping
                        VertexHandle handle = _bi.add_vertex(v[j]);
                        _bi.set_normal(handle, norm[i]);
                        vhandles.push_back(handle);
                        vMap[v[j]] = handle;
                    } else
                        // Yes : get index from map
                        vhandles.push_back(vMapIt->second);
                }

                // Add face only if it is not degenerated
                if ((vhandles[0] != vhandles[1]) && (vhandles[0] != vhandles[2]) && (vhandles[1] != vhandles[2])) {
                    FaceHandle fh = _bi.add_face(vhandles);

                    if (fh.is_valid())
                        _bi.set_normal(fh, n);
                }
            }
        }

        _bi.finish();
        m_meshName = getBasicName(fileName);

        // 通过面法线计算顶点法线
        m_mesh.request_face_normals();
        // mesh计算出顶点法线
        m_mesh.update_normals();
        // 释放面法线
        m_mesh.release_face_normals();
        return true;
    }

    return false;
}

bool NCTriaOpenMesh::ReadStlFile(const string& fileName)
{
    return true;
}

void NCTriaOpenMesh::ComputerBox()
{
    KWBox box;
    bool bFirst = true;
    for (MyMesh::HalfedgeIter he_it = m_mesh.halfedges_begin(); he_it != m_mesh.halfedges_end(); ++he_it) {
        //链接这个有向边的起点和终点
        auto& from = m_mesh.point(m_mesh.from_vertex_handle(*he_it));
        auto& to = m_mesh.point(m_mesh.to_vertex_handle(*he_it));

        if (bFirst) {
            bFirst = false;
            box.xmax = box.xmin = from[0];
            box.ymax = box.ymin = from[1];
            box.zmax = box.zmin = from[2];
            addPointToBox(box, to);
        } else {
            addPointToBox(box, from);
        }
        addPointToBox(box, to);
    }

    m_box = box;
}

vector<KWPolyline> NCTriaOpenMesh::ProcessPolyLine3(double offset)
{
    vector<KWPolyline> polys;
    double eps = 1;
    /***
     * Circle
     ***/
    KWPolyline circlePoly;
    CVector3d center;
    CVector3d dir;
    double radius;
    double height;
    if (m_box.Width() > m_box.Depth() && m_box.Width() > m_box.Height()) {

        center = CVector3d(m_box.xmin,
            0.5 * (m_box.ymin + m_box.ymin),
            0.5 * (m_box.zmin + m_box.zmax));
        dir = XDIR;
        height = m_box.Width();
        radius = sqrt(m_box.Depth() * m_box.Depth() + m_box.Height() * m_box.Height()) / 2;
    } else if (m_box.Height() > m_box.Width() && m_box.Height() > m_box.Depth()) {

        center = CVector3d(0.5 * (m_box.xmin + m_box.xmax),
            m_box.ymin,
            0.5 * (m_box.zmin + m_box.zmax));
        dir = YDIR;
        height = m_box.Height();
        radius = sqrt(m_box.Depth() * m_box.Depth() + m_box.Width() * m_box.Width()) / 2;
    } else {
        center = CVector3d(0.5 * (m_box.xmin + m_box.xmax),
            0.5 * (m_box.ymin + m_box.ymin),
            m_box.zmin);
        dir = ZDIR;
        height = m_box.Depth();
        radius = sqrt(m_box.Width() * m_box.Width() + m_box.Height() * m_box.Height()) / 2;
    }

    circlePoly.fromCircle(radius, eps, CVector3d(0, 0, 0));

    //Circle
    CQuaternion qua(ZDIR, dir);
    auto mat = qua.GetMatrix();

    vector<CVector3d> vxList, vzList;
    auto vy = dir;
    for (size_t i = 0; i < circlePoly.size(); ++i) {
        circlePoly[i] = circlePoly[i] * mat + center;
        auto vx = (circlePoly[i] - center);
        vx.Normalize();
        vxList.push_back(vx);
        auto vz = vx.Cross(vy);
        vz.Normalize();
        vzList.push_back(vz);
    }

    //    return polys;
    polys.resize(circlePoly.size());

    SmartHalfedgeHandle heHandle, heFirst;
    //每个面找轨迹
    for (size_t i = 0; i < circlePoly.size(); ++i) {
        double res[3] = { -1, -1, -1 };
        double resFirst[3] { -1, -1, -1 };
        map<OpenMesh::SmartEdgeHandle, bool> hMaps, hMapsTmp;

        KWPolyline poly1, poly2;

        VertexHandle vertexhandle;
        bool bNext = false, bNextFirst = false;
        //1. 找第一个半边
        for (auto he_it = m_mesh.halfedges_begin(); he_it != m_mesh.halfedges_end(); ++he_it) {
            if (!GetRes(*he_it, res, vxList[i], vzList[i], center, hMapsTmp))
                continue;

            if ((res[2] >= 0 && res[2] <= 1) || ((fabs(res[0]) < FLT_EPSILON && fabs(res[1]) < FLT_EPSILON))) {
                resFirst[0] = res[0];
                resFirst[1] = res[1];
                resFirst[2] = res[2];
                bNextFirst = bNext = true;
                auto ed_ha = m_mesh.edge_handle(*he_it);
                hMaps[ed_ha] = true;
                heFirst = heHandle = *he_it;
                break;
            }
        }
        //2. 搜寻下一个边 到找不到为止
        while (bNext) {
            bNext = false;
            auto pt = m_mesh.point(m_mesh.from_vertex_handle(heHandle));
            CVector3d pt0(pt[0], pt[1], pt[2]);
            pt = m_mesh.point(m_mesh.to_vertex_handle(heHandle));
            CVector3d pt1(pt[0], pt[1], pt[2]);

            if ((fabs(res[0]) <= FLT_EPSILON && fabs(res[1]) <= FLT_EPSILON)) { // res[0]==0 && res[1]==0
                poly1.push_back(pt0);
                poly1.push_back(pt1);
            } else if (res[2] >= 0 && res[2] <= 1) { // 0 <= res[2] <= 1
                poly1.push_back(pt0 + res[2] * (pt1 - pt0));
            }

            if ((fabs(res[0]) <= FLT_EPSILON && fabs(res[1]) <= FLT_EPSILON)
                || fabs(res[2] - 1) <= FLT_EPSILON) { //2.1 res[0] == 0 || res[2] == 1
                vertexhandle = m_mesh.to_vertex_handle(heHandle);
            }
            if (fabs(res[2]) < FLT_EPSILON) { //2.2 res[2] == 0
                vertexhandle = m_mesh.from_vertex_handle(heHandle);
            } else if (res[2] > 0 && res[2] <= 1) {
                auto heInit = heHandle = heHandle.opp();
                while ((heHandle = heHandle.next()).is_valid() && heInit != heHandle) {
                    if (!GetRes(heHandle, res, vxList[i], vzList[i], center, hMaps))
                        continue;
                    if (res[2] <= 1 && 0 <= res[2]) {
                        bNext = true;
                        break;
                    }
                }
                continue;
            }

            for (auto voh_iter = m_mesh.voh_iter(vertexhandle); voh_iter.is_valid(); ++voh_iter) {
                if (!GetRes(*voh_iter, res, vxList[i], vzList[i], center, hMaps))
                    continue;
                //a == 0 && b == 0
                if ((fabs(res[0]) <= FLT_EPSILON && fabs(res[1]) <= FLT_EPSILON)) {
                    heHandle = *voh_iter;
                    bNext = true;
                    break;
                }
                if (!GetRes(voh_iter->next(), res, vxList[i], vzList[i], center, hMaps))
                    continue;
                if (res[2] <= 1 && 0 <= res[2]) {
                    heHandle = voh_iter->next();
                    bNext = true;
                    break;
                }
            }
        }

        heHandle = heFirst.opp();
        res[0] = resFirst[0];
        res[1] = resFirst[1];
        res[2] = resFirst[2];
        bNext = bNextFirst;
        while (bNext) {
            bNext = false;

            if (!bNextFirst) {
                auto pt = m_mesh.point(m_mesh.from_vertex_handle(heHandle));
                CVector3d pt0(pt[0], pt[1], pt[2]);
                pt = m_mesh.point(m_mesh.to_vertex_handle(heHandle));
                CVector3d pt1(pt[0], pt[1], pt[2]);

                if ((fabs(res[0]) < FLT_EPSILON && fabs(res[1]) < FLT_EPSILON)) { // res[0]==0 && res[1]==0
                    poly2.push_back(pt0);
                    poly2.push_back(pt1);
                } else if (res[2] >= 0 && res[2] <= 1) { // 0 <= res[2] <= 1
                    poly2.push_back(pt0 + res[2] * (pt1 - pt0));
                }
            }
            bNextFirst = false;

            if ((fabs(res[0]) < FLT_EPSILON && fabs(res[1]) < FLT_EPSILON)
                || fabs(res[2] - 1) < FLT_EPSILON) { //2.1 res[0] == 0 || res[2] == 1
                vertexhandle = m_mesh.to_vertex_handle(heHandle);
            }
            if (fabs(res[2]) < FLT_EPSILON) { //2.2 res[2] == 0
                vertexhandle = m_mesh.from_vertex_handle(heHandle);
            } else if (res[2] > 0 && res[2] <= 1) {
                auto heInit = heHandle = heHandle.opp();
                while ((heHandle = heHandle.next()).is_valid() && heInit != heHandle) {
                    if (!GetRes(heHandle, res, vxList[i], vzList[i], center, hMaps))
                        continue;
                    if (res[2] <= 1 && 0 <= res[2]) {
                        bNext = true;
                        break;
                    }
                }
                continue;
            }

            for (auto voh_iter = m_mesh.voh_iter(vertexhandle); voh_iter.is_valid(); ++voh_iter) {
                if (!GetRes(*voh_iter, res, vxList[i], vzList[i], center, hMaps))
                    continue;
                //a == 0 && b == 0
                if ((fabs(res[0]) < FLT_EPSILON && fabs(res[1]) < FLT_EPSILON)) {
                    heHandle = *voh_iter;
                    bNext = true;
                    break;
                }
                if (!GetRes(voh_iter->next(), res, vxList[i], vzList[i], center, hMaps))
                    continue;
                if (res[2] <= 1 && 0 <= res[2]) {
                    heHandle = voh_iter->next();
                    bNext = true;
                    break;
                }
            }
        }

        poly1.reverse();
        polys[i].insert(polys[i].end(), poly1.begin(), poly1.end());
        cout << poly1.front().y() << poly2.back().y();
        polys[i].insert(polys[i].end(), poly2.begin(), poly2.end());
    }

    ///偏置、反转
    bool bReserve = false;
    for (size_t i = 0; i < polys.size(); ++i) {
        if (polys[i].empty())
            continue;

        //映射到XY平面
        for (size_t j = 0; j < polys[i].size(); ++j) {
            auto pt = polys[i][j] - center;
            polys[i][j][0] = pt.Dot(vxList[i]);
            polys[i][j][1] = pt.Dot(vy);
            polys[i][j][2] = pt.Dot(vzList[i]);
        }

        //        //偏移
        //        vector<NCPath> res;
        //        //offset
        //        GeometricUtil::OffsetPath(polys[i], offset, res);
        //        polys[i] = res[0];

        for (size_t j = 0; j < polys[i].size(); ++j) {
            //恢复3D图形
            polys[i][j] = vy * polys[i][j][1] + vxList[i] * polys[i][j][0] + vzList[i] * polys[i][j][2];
            polys[i][j] = polys[i][j] + center;
        }
        if (bReserve ^ (polys[i].front().x() - polys[i].back().x() > 0))
            polys[i].reverse();
        bReserve = !bReserve;
    }
    //    polys.resize(1);
    return polys;
}

vector<KWPolyline> NCTriaOpenMesh::ProcessPolyLine2()
{
    vector<KWPolyline> polys;

    double epsH = 10;

    /***
     * Circle
     ***/
    CVector3d center;
    CVector3d dir;
    double height;
    if (m_box.Width() > m_box.Depth() && m_box.Width() > m_box.Height()) {

        center = CVector3d(m_box.xmin,
            0.5 * (m_box.ymin + m_box.ymin),
            0.5 * (m_box.zmin + m_box.zmax));
        dir = XDIR;
        height = m_box.Width();
    } else if (m_box.Height() > m_box.Width() && m_box.Height() > m_box.Depth()) {

        center = CVector3d(0.5 * (m_box.xmin + m_box.xmax),
            m_box.ymin,
            0.5 * (m_box.zmin + m_box.zmax));
        dir = YDIR;
        height = m_box.Height();
    } else {

        center = CVector3d(0.5 * (m_box.xmin + m_box.xmax),
            0.5 * (m_box.ymin + m_box.ymin),
            m_box.zmin);
        dir = ZDIR;
        height = m_box.Depth();
    }

    //Circle

    CQuaternion qua(ZDIR, dir);
    auto mat = qua.GetMatrix();

    vector<CVector3d> centerList;
    for (double h = 0.000; h < height; h += epsH) {
        centerList.push_back(dir * h + center);
    }

    polys.resize(centerList.size());

    for (auto he_it = m_mesh.halfedges_begin(); he_it != m_mesh.halfedges_end(); ++he_it) {
        if (!m_mesh.face_handle(*he_it).is_valid())
            continue;

        const MyMesh::Point& p0 = m_mesh.point(m_mesh.to_vertex_handle(*he_it));
        const MyMesh::Point& p1 = m_mesh.point(m_mesh.from_vertex_handle(*he_it));

        double a = (p1[0] - p0[0]) * dir[0]
            + (p1[1] - p0[1]) * dir[1]
            + (p1[2] - p0[2]) * dir[2];

        if (fabs(a) < FLT_EPSILON) {
            continue;
        }
        for (size_t i = 0; i < centerList.size(); ++i) {
            double b = (p0[0] - centerList[i].x()) * dir[0]
                + (p0[1] - centerList[i].y()) * dir[1]
                + (p0[2] - centerList[i].z()) * dir[2];
            double r = -b / a;

            if (r >= 0 && r <= 1) {
                CVector3d v0(p0[0], p0[1], p0[2]), v1(p1[0], p1[1], p1[2]);
                CVector3d pt = v0 + (v1 - v0) * r;
                polys[i].push_back(pt);
            }
        }
    }

    for (size_t i = 0; i < polys.size(); ++i) {
        sort(polys[i].begin(), polys[i].end(),
            [=](CVector3d& p1, CVector3d& p2) -> bool {
                auto v1 = p1 - centerList[i];
                auto v2 = p2 - centerList[i];

                auto vn = v1.Cross(v2);

                if (dir.Dot(vn) < 0)
                    return true;
                return false;
            });
    }

    return polys;
}

vector<KWPolyline> NCTriaOpenMesh::ProcessPolyLine(double offset)
{
    vector<KWPolyline> polys;
    if (!m_mesh.n_faces())
        return polys;
    double eps = 1;
    /***
     * Circle
     ***/
    KWPolyline circlePoly;
    CVector3d center;
    CVector3d dir;
    double radius;
    double height;
    if (m_box.Width() > m_box.Depth() && m_box.Width() > m_box.Height()) {

        center = CVector3d(m_box.xmin,
            0.5 * (m_box.ymin + m_box.ymin),
            0.5 * (m_box.zmin + m_box.zmax));
        dir = XDIR;
        height = m_box.Width();
        radius = sqrt(m_box.Depth() * m_box.Depth() + m_box.Height() * m_box.Height()) / 2;
    } else if (m_box.Height() > m_box.Width() && m_box.Height() > m_box.Depth()) {

        center = CVector3d(0.5 * (m_box.xmin + m_box.xmax),
            m_box.ymin,
            0.5 * (m_box.zmin + m_box.zmax));
        dir = YDIR;
        height = m_box.Height();
        radius = sqrt(m_box.Depth() * m_box.Depth() + m_box.Width() * m_box.Width()) / 2;
    } else {
        center = CVector3d(0.5 * (m_box.xmin + m_box.xmax),
            0.5 * (m_box.ymin + m_box.ymin),
            m_box.zmin);
        dir = ZDIR;
        height = m_box.Depth();
        radius = sqrt(m_box.Width() * m_box.Width() + m_box.Height() * m_box.Height()) / 2;
    }

    circlePoly.fromCircle(radius, eps, CVector3d(0, 0, 0));

    //Circle
    CQuaternion qua(ZDIR, dir);
    auto mat = qua.GetMatrix();

    vector<CVector3d> vxList, vzList;
    auto vy = dir;
    for (size_t i = 0; i < circlePoly.size(); ++i) {
        circlePoly[i] = circlePoly[i] * mat + center;
        auto vx = (circlePoly[i] - center);
        vx.Normalize();
        vxList.push_back(vx);
        auto vz = vx.Cross(vy);
        vz.Normalize();
        vzList.push_back(vz);
    }

    //    return polys;
    polys.resize(circlePoly.size());

    CVector3d cutFaceNormal;
    map<OpenMesh::SmartEdgeHandle, bool> hMaps;

    for (auto he_it = m_mesh.halfedges_begin(); he_it != m_mesh.halfedges_end(); ++he_it) {
        auto fa_ha = m_mesh.face_handle(*he_it);
        auto ed_ha = m_mesh.edge_handle(*he_it);
        if (!fa_ha.is_valid() || hMaps.find(ed_ha) != hMaps.end())
            continue;
        hMaps[ed_ha] = true;

        auto fa_normal = m_mesh.normal(fa_ha);
        CVector3d faNormal(fa_normal[0], fa_normal[1], fa_normal[2]);

        const MyMesh::Point& p0 = m_mesh.point(m_mesh.to_vertex_handle(*he_it));
        const MyMesh::Point& p1 = m_mesh.point(m_mesh.from_vertex_handle(*he_it));

        for (size_t i = 0; i < circlePoly.size(); ++i) {
            cutFaceNormal = (circlePoly[i] - center).Cross(dir);
            if ((circlePoly[i] - center).Dot(faNormal) < FLT_EPSILON) { //不同向
                continue;
            }

            double a = (p1[0] - p0[0]) * cutFaceNormal[0]
                + (p1[1] - p0[1]) * cutFaceNormal[1]
                + (p1[2] - p0[2]) * cutFaceNormal[2];

            if (fabs(a) < FLT_EPSILON) {
                continue;
            }
            double b = (p0[0] - center.x()) * cutFaceNormal[0]
                + (p0[1] - center.y()) * cutFaceNormal[1]
                + (p0[2] - center.z()) * cutFaceNormal[2];
            double r = -b / a;

            if (r >= 0 && r <= 1) {
                CVector3d v0(p0[0], p0[1], p0[2]), v1(p1[0], p1[1], p1[2]);
                CVector3d pt = v0 + (v1 - v0) * r;
                polys[i].push_back(pt);
            }
        }
    }

    bool bReserve = polys.front()[0].x() - polys.back()[0].x() > 0;
    for (size_t i = 0; i < polys.size(); ++i) {
        if (polys[i].empty())
            continue;
        sort(polys[i].begin(), polys[i].end(),
            [=](CVector3d& p1, CVector3d& p2) -> bool {
                auto v1 = p1.Dot(dir);
                auto v2 = p2.Dot(dir);

                if (v1 < v2)
                    return true;
                return false;
            });

        for (size_t j = 0; j < polys[i].size(); ++j) {
            auto pt = polys[i][j] - center;
            //映射到XY平面
            polys[i][j][0] = pt.Dot(vxList[i]);
            polys[i][j][1] = pt.Dot(vy);
            polys[i][j][2] = pt.Dot(vzList[i]);
        }

        //偏移
        vector<NCPath> res;
        //offset
        GeometricUtil::OffsetPath(polys[i], offset, res);
        polys[i] = res[0];

        for (size_t j = 0; j < polys[i].size(); ++j) {
            //恢复3D图形
            polys[i][j] = vy * polys[i][j][1] + vxList[i] * polys[i][j][0] + vzList[i] * polys[i][j][2];
            polys[i][j] = polys[i][j] + center;
        }
        if (bReserve)
            polys[i].reverse();
        bReserve = !bReserve;
    }
    //    polys.resize(1);
    return polys;
}

SmartFaceHandle NCTriaOpenMesh::SelectFace(const OpenMesh::Vec3f& pts, const OpenMesh::Vec3f& pte)
{
    SmartFaceHandle res;
    //    OpenMesh::Vec3f _pts(pts[0], pts[1], pts[2]), _pte(pte[0], pte[1], pte[2]);
    OpenMesh::Vec3f l[2] { pts, pte };
    auto dir = pts - pte;
    cout << "Face Number" << m_mesh.n_faces() << '\n';
    for (auto iterF = m_mesh.faces_begin(); iterF != m_mesh.faces_end(); ++iterF) {
        auto iterV = m_mesh.cfv_iter(*iterF);

        OpenMesh::Vec3f tria[3];
        tria[0] = m_mesh.point(*iterV);
        ++iterV;

        tria[1] = m_mesh.point(*iterV);
        ++iterV;

        tria[2] = m_mesh.point(*iterV);
        ++iterV;

        auto normal = m_mesh.normal(*iterF);

        if (CheckLineTria(l, tria) == 0 && dir.dot(normal) > 0) {
            cout << "Tria:-------------\n"
                 << tria[0] << '\n'
                 << tria[1] << '\n'
                 << tria[2] << '\n'
                 << "------------------\n";
            res = *iterF;
        }
    }

    return res;
}

OpenMesh::Vec3f NCTriaOpenMesh::SelectPoint(const OpenMesh::Vec3f& pts, const OpenMesh::Vec3f& pte)
{
    OpenMesh::Vec3f res;
    return res;
}

vector<SmartFaceHandle> NCTriaOpenMesh::SelectFace(const OLine& line1, const OLine& line2)
{
    vector<SmartFaceHandle> res;
    //    OpenMesh::Vec3f _pts(pts[0], pts[1], pts[2]), _pte(pte[0], pte[1], pte[2]);
    cout << "Face Number" << m_mesh.n_faces() << '\n';
    auto vline1 = line1[1] - line1[0];
    vline1.normalize();
    auto vline2 = line2[0] - line2[1];

    for (auto iterF = m_mesh.faces_begin(); iterF != m_mesh.faces_end(); ++iterF) {
        auto iterV = m_mesh.cfv_iter(*iterF);
        auto normal = m_mesh.normal(*iterF);
        for (; iterV.is_valid(); ++iterV) {
            auto v1 = line1[0] - m_mesh.point(*iterV);
            auto v2 = line2[1] - m_mesh.point(*iterV);

            //            auto normal = m_mesh.normal(*iterF);

            if (v1.cross(vline1).dot(v2.cross(vline2)) > 0
                && vline1.dot(normal) < 0) {
                res.push_back(*iterF);
            }
        }
    }

    return res;
}

bool NCTriaOpenMesh::GetRes(SmartHalfedgeHandle he,
    double res[],
    const CVector3d& cutDir,
    const CVector3d& cutFaceDir,
    const CVector3d& ptC,
    std::map<SmartEdgeHandle, bool>& edgeMaps)
{
    auto ed_ha = m_mesh.edge_handle(he);
    auto fa_ha = m_mesh.face_handle(he);
    //    cout << he.is_boundary()  << " " <<  !fa_ha.is_valid() << " " << (edgeMaps.find(ed_ha) != edgeMaps.end()) << endl;
    if (!fa_ha.is_valid() || edgeMaps.find(ed_ha) != edgeMaps.end()) { //如果是边界，或者找不到所属三角面，或者已经搜索过，就跳过
        return false;
    }
    //设置查找标记
    edgeMaps[ed_ha] = true;

    //当前三角面片法向量
    auto fa_normal = m_mesh.normal(fa_ha);
    CVector3d faNormal(fa_normal[0], fa_normal[1], fa_normal[2]);
    //当前半边的起始点
    const MyMesh::Point& p0 = m_mesh.point(m_mesh.from_vertex_handle(he));
    const MyMesh::Point& p1 = m_mesh.point(m_mesh.to_vertex_handle(he));
    CVector3d pt0(p0[0], p0[1], p0[2]);
    CVector3d pt1(p1[0], p1[1], p1[2]);

    GetResult(pt0, pt1, ptC, cutFaceDir, res);

    if ((0 <= res[2] && res[2] <= 1 && cutDir.Dot(faNormal) > 0)
        || (fabs(res[0] < FLT_EPSILON) && fabs(res[1] < FLT_EPSILON))) {
        return true;
    }
    return false;
}

string NCTriaOpenMesh::GetMeshName()
{
    return m_meshName;
}

void NCTriaOpenMesh::Output()
{
    cout << "//----" << m_meshName << "------------\n"
         << "Face Number:" << m_mesh.n_faces() << '\n'
         << "Vertex Number:" << m_mesh.n_vertices() << '\n'
         << "Edge Number:" << m_mesh.n_edges() << '\n'
         << "---------------------------------------\n";
}

int NCTriaOpenMesh::CheckLineTria(OpenMesh::Vec3f l[], OpenMesh::Vec3f tria[])
{
    auto e1 = Plucker(tria[1], tria[0]);
    auto e2 = Plucker(tria[2], tria[1]);
    auto e3 = Plucker(tria[0], tria[2]);
    auto L = Plucker(l[0], l[1]);

    auto s1 = SideOp(L, e1);
    auto s2 = SideOp(L, e2);
    auto s3 = SideOp(L, e3);

    if ((s1 == 0 && s2 == 0 && s3 == 0) //"线和三角形共面"
        || ((s1 > 0 && s2 > 0 && s3 > 0) || (s1 < 0 && s2 < 0 && s3 < 0)) //  "线穿过三角形"
        || ((s1 == 0 && s2 * s3 > 0) || (s2 == 0 && s1 * s3 > 0) || (s3 == 0 && s1 * s2 > 0)) //"线穿过三角形边缘"
        || ((s1 == 0 && (s2 == 0)) || (s1 == 0 && (s3 == 0)) || (s2 == 0 && (s3 == 0))) //"线穿过三角形顶点"
    ) {
        return 0;
    }
    return -1;
}

OpenMesh::Vec6f NCTriaOpenMesh::Plucker(OpenMesh::Vec3f& a, OpenMesh::Vec3f& b)
{
    OpenMesh::Vec6f l;
    l[0] = a[0] * b[1] - b[0] * a[1];
    l[1] = a[0] * b[2] - b[0] * a[2];
    l[2] = a[0] - b[0];
    l[3] = a[1] * b[2] - b[1] * a[2];
    l[4] = a[2] - b[2];
    l[5] = b[1] - a[1];
    return l;
}

float NCTriaOpenMesh::SideOp(OpenMesh::Vec6f& a, OpenMesh::Vec6f& b)
{
    return a[0] * b[4]
        + a[1] * b[5]
        + a[2] * b[3]
        + a[3] * b[2]
        + a[4] * b[0]
        + a[5] * b[1];
}

bool NCTriaOpenMesh::isAscii(const string& fileName)
{
    std::fstream ifs(fileName.c_str(), std::ios::binary | std::ios::in);
    bool res = isAscii(ifs);
    ifs.close();
    return res;
}

bool NCTriaOpenMesh::isAscii(fstream& ins)
{
    ins.seekg(0, ins.end);
    std::streampos theEnd = ins.tellg();
    ins.seekg(0, ins.beg);

    // binary STL files cannot be shorter than 134 bytes
    // (80 bytes header + 4 bytes facet count + 50 bytes for one facet);
    // thus assume files shorter than 134 as Ascii without probing
    // (probing may bring stream to fail state if EOF is reached)

    if ((size_t)theEnd < THE_STL_MIN_FILE_SIZE)
        return true;
    // read first 134 bytes to detect file format
    char aBuffer[THE_STL_MIN_FILE_SIZE];
    std::streamsize aNbRead = ins.read(aBuffer, THE_STL_MIN_FILE_SIZE).gcount();
    if (!aNbRead) {
        cerr << "Error: Cannot read file";
        return true;
    }

    // if file is shorter than size of binary file with 1 facet, it must be ascii
    if (aNbRead < std::streamsize(THE_STL_MIN_FILE_SIZE)) {
        return true;
    }

    // otherwise, detect binary format by presence of non-ascii symbols in first 128 bytes
    // (note that binary STL file may start with the same bytes "solid " as Ascii one)
    for (int aByteIter = 0; aByteIter < aNbRead; ++aByteIter) {
        if ((unsigned char)aBuffer[aByteIter] > (unsigned char)'~') {
            return false;
        }
    }
    return true;
}

string NCTriaOpenMesh::getBasicName(const string& fileName)
{
    string basicName;
    auto pos1 = fileName.find_last_of('\\');
    auto pos2 = fileName.find_last_of("//");

    auto pos = pos1;

    if (pos1 == string::npos) {
        if (pos2 == string::npos)
            pos = 0;
        else
            pos = pos2;
    } else {
        if (pos2 == string::npos)
            pos = pos1 > pos2 ? pos1 : pos2;
        else
            pos = pos1;
    }
    pos += 1;
    if (pos == string::npos)
        pos = 0;

    basicName = fileName.substr(pos);
    return basicName;
}

void NCTriaOpenMesh::addPointToBox(KWBox& box, MyMesh::Point& pt)
{
    if (box.xmin > pt[0]) {
        box.xmin = pt[0];
    } else if (box.ymin > pt[1]) {
        box.ymin = pt[1];
    } else if (box.zmin > pt[2]) {
        box.zmin = pt[2];
    } else if (box.xmin > pt[0]) {
        box.xmin = pt[0];
    } else if (box.xmax < pt[0]) {
        box.xmax = pt[0];
    } else if (box.ymax < pt[1]) {
        box.ymax = pt[1];
    } else if (box.zmax < pt[2]) {
        box.zmax = pt[2];
    }
}

OpenMesh::IO::STL_Type checkType(const std::string& fileName)
{
    std::fstream ins(fileName.c_str(), std::ios::binary | std::ios::in);
    ins.seekg(0, ins.end);
    std::streampos theEnd = ins.tellg();
    ins.seekg(0, ins.beg);

    // binary STL files cannot be shorter than 134 bytes
    // (80 bytes header + 4 bytes facet count + 50 bytes for one facet);
    // thus assume files shorter than 134 as Ascii without probing
    // (probing may bring stream to fail state if EOF is reached)

    if ((size_t)theEnd < THE_STL_MIN_FILE_SIZE)
        return OpenMesh::IO::STLA;
    // read first 134 bytes to detect file format
    char aBuffer[THE_STL_MIN_FILE_SIZE];
    std::streamsize aNbRead = ins.read(aBuffer, THE_STL_MIN_FILE_SIZE).gcount();
    if (!aNbRead) {
        cerr << "Error: Cannot read file";
        return OpenMesh::IO::NONE;
    }

    // if file is shorter than size of binary file with 1 facet, it must be ascii
    if (aNbRead < std::streamsize(THE_STL_MIN_FILE_SIZE)) {
        return OpenMesh::IO::STLA;
    }

    // otherwise, detect binary format by presence of non-ascii symbols in first 128 bytes
    // (note that binary STL file may start with the same bytes "solid " as Ascii one)
    for (int aByteIter = 0; aByteIter < aNbRead; ++aByteIter) {
        if ((unsigned char)aBuffer[aByteIter] > (unsigned char)'~') {
            return OpenMesh::IO::STLB;
        }
    }

    //    ins.seekg(80, ins.beg);
    //    int count = 0;
    //    ins.read((char*)&count, sizeof(count));
    //    if(THE_STL_HEADER_SIZE + THE_STL_SIZEOF_FACET*count == (size_t)theEnd )
    //        return OpenMesh::IO::STLB;

    return OpenMesh::IO::STLA;
    ins.close();
}
