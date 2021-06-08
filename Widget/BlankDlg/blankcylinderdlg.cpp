#include "blankcylinderdlg.h"
#include "ui_blankcylinderdlg.h"

BlankCylinderDlg::BlankCylinderDlg(NCTriaMeshSPtr node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlankCylinderDlg),
    m_mesh(node)
{
    ui->setupUi(this);
    InitConnect();
}


BlankCylinderDlg::~BlankCylinderDlg()
{
    delete ui;
}

int BlankCylinderDlg::exec()
{
    InitFrom();
    Update();
    return QDialog::exec();
}

void BlankCylinderDlg::Update()
{
    ui->edCenter->setText(QString("X:%1 Y:%2 Z:%3").arg(m_center.x(), 0,  'g', 3)
                          .arg(m_center.y(), 0,  'g', 3)
                          .arg(m_center.z(), 0,  'g', 3));
    ui->edDia->setText(QString::number(m_dDia, 'g', 3));
    ui->edHeight->setText(QString::number(m_dHeight, 'g', 3));
    ui->edREx->setText(QString::number(m_dREx, 'g', 3));
    ui->edHEx->setText(QString::number(m_dHEx, 'g', 3));
    ui->edXMin->setText(QString::number(m_dXMin, 'g', 3));
    ui->edXMax->setText(QString::number(m_dXMax, 'g', 3));
}

void BlankCylinderDlg::OnAutoComputer()
{
    KWBox box;
    if(!m_mesh)
        return;
    box = m_mesh->GetBouningBox();

    //更新参数
    m_dHEx = ui->edHEx->text().toDouble();
    m_dREx = ui->edREx->text().toDouble();
    m_center = box.center();

    m_dXMin = box.xmin - m_dHEx;
    m_dXMax = box.xmax + m_dHEx;
    m_dHeight = m_dXMax - m_dXMin;
    m_dDia = std::max(box.zmax - box.zmin, box.ymax - box.ymin) + 2 ;
    Update();
}

void BlankCylinderDlg::OnOk()
{
    m_data.reset(new BlankCylinder);
    //更新参数
    m_dHeight = ui->edHeight->text().toDouble();
    m_dDia = ui->edDia->text().toDouble();

    m_data->SetValue(BlankCylinder::CYLINDER_DIA, QString::number(m_dDia));
    m_data->SetValue(BlankCylinder::CYLINDER_HEIGHT, QString::number(m_dHeight));
    m_data->SetValue(BlankCylinder::CYLINDER_XCENTER, QString::number(m_center.x()));
    m_data->SetValue(BlankCylinder::CYLINDER_YCENTER, QString::number(m_center.y()));
    m_data->SetValue(BlankCylinder::CYLINDER_ZCENTER, QString::number(m_center.z()));

    QDialog::accept();
}

void BlankCylinderDlg::OnCancle()
{
    QDialog::reject();
}

void BlankCylinderDlg::OnEditXMax()
{
    m_dXMax = ui->edXMax->text().toDouble();
    m_dHeight = m_dXMax - m_dXMin;
    Update();
}

void BlankCylinderDlg::OnEditXMin()
{
    m_dXMin = ui->edXMin->text().toDouble();
    m_dHeight = m_dXMax - m_dXMin;
    Update();
}

ToolDataSPtr BlankCylinderDlg::GetResult()
{
    return m_data;
}

void BlankCylinderDlg::InitFrom()
{
    if(!m_mesh)
        return;

    KWBox box = m_mesh->GetBouningBox();
    SetBox(box);
}

void BlankCylinderDlg::InitConnect()
{
    connect(ui->btnAutoComput, &QPushButton::clicked, this, &BlankCylinderDlg::OnAutoComputer);
    connect(ui->btnOk, &QPushButton::clicked, this, &BlankCylinderDlg::OnOk);
    connect(ui->btnCancle, &QPushButton::clicked, this, &BlankCylinderDlg::OnCancle);
    connect(ui->edXMax, &QLineEdit::textChanged, this, &BlankCylinderDlg::OnEditXMax);
    connect(ui->edXMin, &QLineEdit::textChanged, this, &BlankCylinderDlg::OnEditXMin);
}

void BlankCylinderDlg::SetBox(const KWBox &box)
{
    m_dXMin = box.ymin;
    m_dXMax = box.ymax;
    m_dHeight = m_dXMax - m_dXMin;
    double a = box.zmax - box.zmin;
    double b = box.xmax - box.xmin;
    m_dDia = sqrt(a*a + b*b);
    m_center = box.center();
    m_center[1] = box.ymin;
}
