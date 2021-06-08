#include "kwctrlview.h"
#include <QMouseEvent>
#include <QDebug>
#include <ctime>
#include <QPainter>

KWCtrlView::KWCtrlView(QWidget *parent)
    : QGLWidget(parent)
    , minBound(-1000.0, -1000.0, -1000.0)
    , maxBound(1000, 1000, 1000)
    , firstView(true)
    , oldX(0)
    , oldY(0)
    , m_buttonflags(NO_BUTTON)
{
    int winWidth = 640;
    int winHeight = 480;
    setGeometry( 0, 0, winWidth, winHeight );
    setAttribute(Qt::WA_DeleteOnClose);

}

KWCtrlView::~KWCtrlView()
{
}

void KWCtrlView::ResetView()
{
    if(!isValid())
        return;
    makeCurrent();
    arcball.Clear();
    arcball.SetCenter(-0.5*(minBound.x() + maxBound.x()),
                      -0.5*(minBound.y() + maxBound.y()),
                      -maxBound.z());
//    arcball.SetRadius(2000);
    ViewAll(true);
//    firstView = true;
    doneCurrent();
//    repaint();
    update();
}

bool KWCtrlView::AddList(GLuint theList, bool isVisiable)
{
    if(FindList(theList) == nopos)
    {
        ShapeList slist {theList, isVisiable};
        m_shapeList.push_back(slist);
        return m_shapeList.size();
    }
    return false;
}

void KWCtrlView::SetListVisiable(GLuint theList, bool isVisiable)
{
    size_t index = nopos;
    if((index = FindList(theList)) != nopos)
    {
        m_shapeList[index].isVisiable = isVisiable;
    }
}

void KWCtrlView::SetDrawMode(GLenum mode)
{
    m_glMode = mode;
    update();
}

void KWCtrlView::SetViweBound(const CVector3d &minB, const CVector3d &maxB)
{
    minBound = minB;
    maxBound = maxB;
}


//void KWCtrlView::UpdateShape()
//{
//    UpdateList();
//}

void KWCtrlView::initializeGL()
{

    InitCamera();


    glShadeModel( GL_SMOOTH );

    glClearColor( 0.0, 0.0, 0.0, 0.0 );

    glClearDepth( 1.0 );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);//源像素因子采用alpha通道值，目标像素因子采用1.0
    glEnable(GL_BLEND);
//    ResetView();

    InitLight0();

    Initialize();
//    ResetView();
}

void KWCtrlView::resizeGL(int w, int h)
{
    if ( h == 0 )
    {
        h = 1;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    viewport.SetSize(w, h);
    viewport.glDraw();
    camera.glDraw(viewport);
    glDrawBuffer(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void KWCtrlView::paintGL()
{
    //要清除之前的深度缓存
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    ViewAll(firstView);
    arcball.glDraw();

    glPolygonMode(GL_FRONT_AND_BACK , m_glMode );

    if(m_glMode == GL_FILL)
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    //与显示相关的函数
    for(size_t i = 0; i < m_shapeList.size(); ++i)
    {
        if(m_shapeList[i].isVisiable)
        {
            glCallList(m_shapeList[i].list);
        }
    }

    DrawGLSence();

    glPopMatrix();
    glFlush();
    glFinish();
}

void KWCtrlView::mouseMoveEvent(QMouseEvent *event)
{
     MouseMove(m_buttonflags, oldX, oldY , event->x(), event->y());
     oldX = event->x();
     oldY = event->y();
     update();
}

void KWCtrlView::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons().testFlag(Qt::LeftButton))
    {
        m_buttonflags |= LEFT_BUTTON;
    }
    if(event->buttons().testFlag(Qt::RightButton))
    {
        m_buttonflags |= RIGHT_BUTTON;

    }

//    if(event->buttons().testFlag(Qt::MiddleButton))
//    {
//        m_buttonflags = MID_BUTTON;
//        handleMouseButton(event->x(), event->y());
//    }
    handleMouseButton(event->x(), event->y());
    MouseButtonDown(m_buttonflags, event->x(), event->y());

    oldX = event->x();
    oldY = event->y();
}

void KWCtrlView::mouseReleaseEvent(QMouseEvent *event)
{
    CVector3d vec = arcball.Intersect(event->x(), viewport.yRes()-event->y(),
        camera, viewport);
    arcball.EndDrag(vec);
    MouseButtonUp(m_buttonflags, event->x(), event->y());

    //抬起之后置为 0
    m_buttonflags = 0;
    oldX = event->x();
    oldY = event->y();
}

void KWCtrlView::wheelEvent(QWheelEvent *event)
{
    double delta = event->delta() / 20;   //delta()一般返回 +-120

    CVector3d vec = CVector3d(0.0, 0.0, 0.0);
    arcball.SetMode(ARCBALL_TRANSLATE_Z);
    arcball.BeginDrag(vec);
    vec.y(vec.y() + delta);
    arcball.Motion(vec);
    arcball.EndDrag(vec);
    repaint();
}

void KWCtrlView::mouseDoubleClickEvent(QMouseEvent *event)
{
    MouseDBClick(m_buttonflags, event->x(), event->y());
}

void KWCtrlView::enterEvent(QEvent *evt)
{
    return QGLWidget::enterEvent(evt);
}

void KWCtrlView::leaveEvent(QEvent *evt)
{
    return QGLWidget::leaveEvent(evt);
}

void KWCtrlView::MouseButtonDown(int flags, int x, int y)
{
    Q_UNUSED(flags);
    Q_UNUSED(x);
    Q_UNUSED(y);
}

void KWCtrlView::MouseButtonUp(int flags, int x, int y)
{
    Q_UNUSED(flags);
    Q_UNUSED(x);
    Q_UNUSED(y);
}

void KWCtrlView::MouseMove(int flags, int xpre, int ypre, int xcur, int ycur)
{
    Q_UNUSED(xpre);
    Q_UNUSED(ypre);
    if(flags)
    {
        CVector3d vec = arcball.Intersect(xcur, viewport.yRes() - ycur, camera, viewport);
        arcball.Motion(vec);
    }

    repaint();
}

void KWCtrlView::MouseDBClick(int flags, int x, int y)
{
    Q_UNUSED(flags);
    Q_UNUSED(x);
    Q_UNUSED(y);
}

void KWCtrlView::handleMouseButton(double x, double y)
{
    CVector3d vec = arcball.Intersect(x, viewport.yRes()-y,
        camera, viewport);
    arcball.EndDrag(vec);
    arcball.SetMode(m_buttonflags);
    vec = arcball.Intersect(x, viewport.yRes()-y, camera,viewport);
    arcball.BeginDrag(vec);
}

void KWCtrlView::ViewAll(bool check_first)
{
    // make it only once
    if(!check_first)
        return;

    firstView = false;

    // set up the camera to visualize the whole object
    //	TRACE("setup camera.");

    CMatrix44 ArcballMatrix = arcball.GetMatrix();

    minBound = ArcballMatrix * minBound;
    maxBound = ArcballMatrix * maxBound;
    camera.ViewAll(minBound[0],
        maxBound[0],
        minBound[1],
        maxBound[1],
        minBound[2],
        maxBound[2],
        viewport);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.glDraw(viewport);
    glMatrixMode(GL_MODELVIEW);
}

void KWCtrlView::InitCamera()
{
    // set viewport and camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    QRect rect = this->geometry();
    viewport.SetOrigin(0,0);
    viewport.SetSize(rect.width(), rect.height());
    camera.SetHeightAngle(30);
    camera.SetPosition(0., 0., 10.);
    camera.SetOrientation(0., 0., 1., 0.0);
    camera.SetNearDistance(.1);
    camera.SetFarDistance(100.);
    viewport.glDraw();
    camera.glDraw(viewport);
    glDrawBuffer(GL_BACK);
}

void KWCtrlView::InitLight0()
{
    GLfloat posLight[] = {1.0, 1.0, 1.0, 0.0};
    GLfloat ambientLight[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat diffuseLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, posLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}
//void KWCtrlView::UpdateList()
//{

//}

size_t KWCtrlView::FindList(GLuint list)
{
    for(size_t i = 0; i < m_shapeList.size(); ++i)
    {
        if(m_shapeList[i].list == list)
        {
            return i;
        }
    }
    return nopos;
}
