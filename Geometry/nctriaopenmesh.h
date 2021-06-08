#ifndef NCTRIAOPENMESH_H
#define NCTRIAOPENMESH_H

#include "nctriamesh.h"


#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"
#include "Widget/MeshViewer/MeshViewerWidget.h"

class NCTriaOpenMesh : public NCTriaMesh
{
public:
    NCTriaOpenMesh();

    bool LoadFile(const std::string &fileName) override;
    bool SaveFile(const std::string &fileName) override;


    bool ReadStepFile(const std::string &fileName);
    bool ReadStlFile(const std::string &fileName);

    void ComputerBox() override;

    MyMesh &GetMesh() {return m_mesh;}

    vector<KWPolyline> ProcessPolyLine3(double offset);

    //圈圈 算法
    vector<KWPolyline> ProcessPolyLine2();

    //曲线 算法
    vector<KWPolyline> ProcessPolyLine(double offset) override;

    SmartFaceHandle SelectFace(const OpenMesh::Vec3f &pts, const OpenMesh::Vec3f &pte);
    OpenMesh::Vec3f SelectPoint(const OpenMesh::Vec3f &pts, const OpenMesh::Vec3f &pte);

    typedef  OpenMesh::Vec3f OLine[2];
    vector<SmartFaceHandle> SelectFace(const OLine &line1, const OLine &line2);

    std::string GetMeshName() override;

    void Output() override;

private:
    int CheckLineTria(OpenMesh::Vec3f line[2], OpenMesh::Vec3f tria[3]);
    OpenMesh::Vec6f Plucker(OpenMesh::Vec3f &v1, OpenMesh::Vec3f &v2);
    float SideOp(OpenMesh::Vec6f &l1, OpenMesh::Vec6f& l2);

private:
    static bool isAscii(const std::string &fileName);
    static bool isAscii(std::fstream &ins);
    std::string getBasicName(const std::string &fileName);
    /**
     * @brief GetResult
     *      P0 P1为线段起始点 Pn为面法向量，Pc为面上的点,
     *      方程 (P0 + r*(P1-P0) - Pc).Dot(Pn) = 0
     *      结果：
     *          a = (p1.x-p0.x)*pN.x + (p1.y-p0.y)*pN.y + (p1.z-p0.z)*pN.z
     *          b = (p0.x-pc.x).pN.x + (p0.y-pc.y).pN.y + (p0.z-pc.z).pN.z
     *          r = -b/a
     * @param p0
     * @param p1
     * @param pc
     * @param pn
     * @param res = {a, b, r}
     */
    inline void GetResult(const CVector3d &p0, const CVector3d &p1, const CVector3d &pc, const CVector3d &pn, double res[3])
    {
        res[0] =(p1-p0).Dot(pn);

        res[1] = (p0-pc).Dot(pn);

        if(fabs(res[0]) <= FLT_EPSILON)
        {
            res[2] = -1;
        }
        else
        {
            res[2] =  -res[1]/res[0];
        }
    }

    bool GetRes(SmartHalfedgeHandle he, double res[3],
                const CVector3d& cutDir,
                const CVector3d& cutFaceDir,
                const CVector3d& ptC,
                std::map<OpenMesh::SmartEdgeHandle, bool> &edgeMaps);

private:
    void addPointToBox(KWBox &box, MyMesh::Point &pt);

private:
    MyMesh m_mesh;
    std::string  m_meshName;
    vector<CVector3d> m_select_point;

};



#endif // NCTRIAOPENMESH_H
