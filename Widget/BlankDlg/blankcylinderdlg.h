
#ifndef BLANKCYLINDERDLG_H
#define BLANKCYLINDERDLG_H

#include <QDialog>

#include "KnifeData/blankcylinder.h"

namespace Ui {
class BlankCylinderDlg;
}

class BlankCylinderDlg : public QDialog
{
    Q_OBJECT

public:
    explicit BlankCylinderDlg(NCTriaMeshSPtr mesh = nullptr, QWidget *parent = nullptr);
    ~BlankCylinderDlg();

    int exec() override;

    void Update();

public Q_SLOTS:
    void OnAutoComputer();
    void OnOk();
    void OnCancle();
    void OnEditXMax();
    void OnEditXMin();
    ToolDataSPtr GetResult();

private:
    void InitFrom();
    void InitConnect();
    void SetBox(const KWBox &box);

private:
    Ui::BlankCylinderDlg *ui;
    ToolDataSPtr m_data;

    NCTriaMeshSPtr m_mesh;
    CVector3d m_center;
    double m_dDia = 0, m_dHeight = 0;
    double m_dHEx = 0, m_dREx = 0;
    double m_dXMin = 0, m_dXMax = 0;

};

#endif // BLANKCYLINDERDLG_H
