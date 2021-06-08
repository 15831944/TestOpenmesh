#include "ncoccctrlview.h"

#include <QKeyEvent>

NCOCCCtrlView::NCOCCCtrlView(QWidget *parent)
    : KWCtrlView(parent)
    , m_modelList(-1)
    , m_listCylinder(-1)
    , m_listCube(-1)
{

}

NCOCCCtrlView::~NCOCCCtrlView()
{

}

void NCOCCCtrlView::ResetView()
{
    CVector3d minB, maxB;

    if(!g_pMesh)
    {
        return;
    }

    KWBox box = g_pMesh->GetBouningBox();

    minB[0] = box.xmin;
    minB[1] = box.ymin;
    minB[2] = box.zmin;

    maxB[0] = box.xmax;
    maxB[1] = box.ymax;
    maxB[2] = box.zmax;

    SetViweBound(minB, maxB);
    KWCtrlView::ResetView();
}

void NCOCCCtrlView::DrawCircle(double radius, const CVector3d &center, const CVector3d &dir)
{
//    CVector3d _dir = dir;
//    _dir.Normalize();
//    KWPolyline poly;
//    NCTriaOpenMesh::Circle2Polyline(poly, radius, 1, center);

//    CQuaternion qua(ZDIR, _dir);
//    auto  mat = qua.GetMatrix();

//    glBegin(GL_TRIANGLE_FAN);           //扇形连续填充三角形串
//    glNormal3f(dir.x(), dir.y(), dir.z());
//    glVertex3f(center.x(), center.y(), center.z());
//    for(size_t i = 0; i < poly.size(); ++i)
//    {
//        auto pt= mat*poly[i];
//        glVertex3f(pt.x(), pt.y(), pt.z());
//    }
//    glEnd();
}

void NCOCCCtrlView::DrawCylinder(double radius, double height, const CVector3d &center, const CVector3d &dir)
{
//    CVector3d _dir = dir;

//    CQuaternion qua(ZDIR, _dir);
//    auto  mat = qua.GetMatrix();

//    _dir.Normalize();
//    KWPolyline poly1;
//    NCTriaOpenMesh::Circle2Polyline(poly1, radius, 1);

//    CVector3d center2 = center + (_dir * height);
//    KWPolyline poly2;
//    for(size_t i = 0; i < poly1.size(); ++i)
//    {
//        poly1[i] = mat*poly1[i] + center;
//        poly2.push_back(poly1[i]+_dir*height);
//    }

//    glBegin(GL_TRIANGLE_FAN);           //扇形连续填充三角形串
//    glNormal3f(-dir.x(), -dir.y(), -dir.z());
//    glVertex3f(center.x(), center.y(), center.z());
//    for(size_t i = 0; i < poly1.size(); ++i)
//    {
//        glVertex3f(poly1[i].x(), poly1[i].y(), poly1[i].z());
//    }
//    glEnd();

//    glBegin(GL_TRIANGLE_FAN);
//    glNormal3f(dir.x(), dir.y(), dir.z());
//    glVertex3f(center2.x(), center2.y(), center2.z());
//    for(size_t i = 0; i < poly2.size(); ++i)
//    {
//        glVertex3f(poly2[i].x(), poly2[i].y(), poly2[i].z());
//    }
//    glEnd();

//    glBegin(GL_TRIANGLE_STRIP);
//    for(size_t i = 0; i < poly1.size(); ++i)
//    {
//        auto n1 = poly1[i] - center;
//        glNormal3f(n1.x(), n1.y(), n1.z());
//        glVertex3f(poly1[i].x(), poly1[i].y(), poly1[i].z());

//        auto n2 = poly2[i] - center;
//        glNormal3f(n2.x(), n2.y(), n2.z());
//        glVertex3f(poly2[i].x(), poly2[i].y(), poly2[i].z());
//    }
//    glEnd();
}

void NCOCCCtrlView::SetModelVisiable(bool isVisiable)
{
    SetListVisiable(m_modelList, isVisiable);
    update();
}

void NCOCCCtrlView::SetCylinderVisiable(bool bShow)
{
    SetListVisiable(m_listCylinder, bShow);
}

void NCOCCCtrlView::DrawGLSence()
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1.f, .0f, .0f); glVertex3f(0, 0, 0);
    glColor3f(1.f, .0f, .0f); glVertex3f(500, 0, 0);
    glColor3f(.0f, 1.f, .0f); glVertex3f(0, 0, 0);
    glColor3f(.0f, 1.f, .0f); glVertex3f(0, 500, 0);
    glColor3f(.0f, .0f, 1.f); glVertex3f(0, 0, 0);
    glColor3f(.0f, .0f, 1.f); glVertex3f(0, 0, 500);
    glEnd();

    glDisable(GL_LIGHTING);
    glColor3f(1.f, .0f, .0f);
    for(size_t i = 0; i < polys.size(); ++i)
    {
        glBegin(GL_LINE_STRIP);
        for(size_t j = 0; j < polys[i].size(); ++j)
        {
            glVertex3d(polys[i][j].x(), polys[i][j].y(), polys[i][j].z());
        }
        glEnd();
    }

}

void NCOCCCtrlView::UpdateShapeList()
{
    MyMesh &mesh = (static_cast<NCTriaOpenMesh *>(g_pMesh.get()))->GetMesh();
    // 绘制模型
    if(m_modelList > 0)
    {
        glDeleteLists(m_modelList, 1);
    }
    glNewList(m_modelList, GL_COMPILE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.8f, 0.5f, 0.1f);
    for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
    {
        glBegin(GL_TRIANGLES);
        for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
        {
            glNormal3fv(mesh.normal(*fv_it).data());
            glVertex3fv(mesh.point(*fv_it).data());
        }
        glEnd();
    }
    glDisable(GL_COLOR_MATERIAL);
    glEndList();


//    clock_t s = clock();
    polys = g_pMesh->ProcessPolyLine(10);
//    qDebug() << u8"耗时：" << clock() - s;
}

void NCOCCCtrlView::Initialize()
{
    m_modelList = glGenLists(1);
    m_listCylinder = glGenLists(1);
    m_listCube = glGenLists(1);
    AddList(m_modelList);
    AddList(m_listCylinder, false);
    AddList(m_listCube, false);
}

void NCOCCCtrlView::MouseDBClick(int flags, int x, int y)
{
    Q_UNUSED(flags);
    Q_UNUSED(x);
    Q_UNUSED(y);
    update();
}

void NCOCCCtrlView::MouseButtonDown(int flags, int x, int y)
{
    Q_UNUSED(flags);
    Q_UNUSED(x);
    Q_UNUSED(y);
}

void NCOCCCtrlView::timerEvent(QTimerEvent *event)
{
    return KWCtrlView::timerEvent(event);
}
