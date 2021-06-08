#ifndef NCOCCCTRLVIEW_H
#define NCOCCCTRLVIEW_H

#include <memory>

#include <QBasicTimer>

#include "kwctrlview.h"
#include "Geometry/nctriaopenmesh.h"

class NCOCCCtrlView : public KWCtrlView
{
public:
    NCOCCCtrlView(QWidget *parent = nullptr);
    ~NCOCCCtrlView() override;

    void ResetView() override;

    void DrawCircle(double radius,
                    const CVector3d& center = CVector3d(0, 0, 0),
                    const CVector3d& dir = CVector3d(0, 0, 1));

    void DrawCylinder(double radius,
                      double height,
                      const CVector3d& center = CVector3d(0, 0, 0),
                      const CVector3d& dir = CVector3d(0, 0, 1)
                      );

    void SetModelVisiable(bool isVisiable);
    void SetCylinderVisiable(bool bShow);

    void DrawGLSence() override;

public Q_SLOTS:
    void UpdateShapeList() override;

protected:
    void Initialize() override;
    void MouseDBClick(int flags, int x, int y) override;
    void MouseButtonDown(int flags, int x, int y) override;

    virtual void timerEvent(QTimerEvent *event) override;

private:
    GLuint m_modelList;
    GLuint m_listCylinder, m_listCube;

    vector<KWPolyline> polys;
    QBasicTimer m_tmShow;
};

#endif // NCOCCCTRLVIEW_H
