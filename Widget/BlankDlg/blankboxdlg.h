#ifndef BLANKBOXDLG_H
#define BLANKBOXDLG_H

#include <QDialog>

#include "KnifeData/blankbox.h"

namespace Ui {
class BlankBoxDlg;
}

class BlankBoxDlg : public QDialog
{
    Q_OBJECT

public:
    explicit BlankBoxDlg(NCTriaMeshSPtr mesh = nullptr, QWidget *parent = nullptr);
    ~BlankBoxDlg();

    int exec() override;

    void InitFrom();
    void InitConnect();

    void Update();


public Q_SLOTS:
    void OnAutoComputer();
    void OnOk();
    void OnCancle();
    ToolDataSPtr GetResult();

private:
    void SetBox(const KWBox &box);

private:
    Ui::BlankBoxDlg *ui;
    ToolDataSPtr m_data;
    NCTriaMeshSPtr m_mesh;

    double dEx[3]{0}, dMin[3]{0}, dMax[3]{0};
    double dLen[3]{0}, dCenter[3]{0};

};

#endif // BLANKBOXDLG_H
