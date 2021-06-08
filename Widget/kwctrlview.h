#ifndef KWCTRLVIEW_H
#define KWCTRLVIEW_H

#include <QWidget>
#include <QPainterPath>

#include <QGLWidget>
#include <QOpenGLExtraFunctions>

#include "ArcBall.h"

class KWCtrlView
        : public QGLWidget
{
    Q_OBJECT
public:
    explicit KWCtrlView(QWidget *parent = nullptr);
    ~KWCtrlView() override;

    virtual void ResetView();

    bool AddList(GLuint theList, bool isVisiable = true);

    void SetListVisiable(GLuint theList, bool isVisiable);

    void SetDrawMode(GLenum mode = GL_LINE);

    void SetViweBound(const CVector3d &minB, const CVector3d& maxB);

    virtual void DrawGLSence() {};

public Q_SLOTS:
    virtual void UpdateShapeList() = 0;

protected:
    ///初始化Opengl
    void initializeGL() override;
    ///窗口大小改变
    void resizeGL(int width,int height) override;
    void paintGL() override;//重绘
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void enterEvent(QEvent *evt) override;
    void leaveEvent(QEvent *evt) override;

protected:
    virtual void Initialize() = 0;

    enum ButtonFlag{
        NO_BUTTON = 0,
        LEFT_BUTTON = 1 << 0,
        RIGHT_BUTTON = 1 << 1,
        MID_BUTTON = 1 << 2,
    };

    virtual void MouseButtonDown(int flags, int x, int y);
    virtual void MouseButtonUp(int flags, int x, int y);
    virtual void MouseMove(int flags, int xpre, int ypre, int xcur, int ycur);
    virtual void MouseDBClick(int flags, int x, int y);

private:
    void handleMouseButton(double x, double y);
    void ViewAll(bool check_first = true);
    void InitCamera();
    void InitLight0();
    /**
     * @brief FindList 查找list
     * @param list
     * @return -1--未找到 否则返回index
     */
    size_t FindList(GLuint list);
    const size_t nopos{static_cast<size_t>(-1)};

signals:

private:
    struct ShapeList
    {
        GLuint list;
        bool isVisiable;
    };
    vector<ShapeList>  m_shapeList;

    // arcball
    CCamera   camera;
    CViewport viewport;
    CArcball  arcball;

    GLenum m_glMode = GL_FILL;
    CVector3d minBound, maxBound;
    bool firstView;
    int oldX, oldY;
    int m_buttonflags;
};

#endif // KWCTRLVIEW_H
