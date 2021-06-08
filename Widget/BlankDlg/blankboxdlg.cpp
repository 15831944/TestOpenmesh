#include "blankboxdlg.h"
#include "ui_blankboxdlg.h"


BlankBoxDlg::BlankBoxDlg(NCTriaMeshSPtr node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlankBoxDlg),
    m_mesh(node)
{

    ui->setupUi(this);
    InitConnect();
}

BlankBoxDlg::~BlankBoxDlg()
{
    delete ui;
}

int BlankBoxDlg::exec()
{
    InitFrom();

    Update();
    return QDialog::exec();
}

void BlankBoxDlg::InitFrom()
{
    if(!m_mesh)
        return;

    KWBox box = m_mesh->GetBouningBox();
    SetBox(box);
}

void BlankBoxDlg::InitConnect()
{
    connect(ui->btnAutoComput, &QPushButton::clicked, this, &BlankBoxDlg::OnAutoComputer);
    connect(ui->btnOk, &QPushButton::clicked, this, &BlankBoxDlg::OnOk);
    connect(ui->btnCancle, &QPushButton::clicked, this, &BlankBoxDlg::OnCancle);
}

void BlankBoxDlg::Update()
{
    ui->edXCenter->setText(QString::number(dCenter[0], 'g', 3));
    ui->edYCenter->setText(QString::number(dCenter[1], 'g', 3));
    ui->edZCenter->setText(QString::number(dCenter[2], 'g', 3));

    ui->edXMin->setText(QString::number(dMin[0], 'g', 3));
    ui->edYMin->setText(QString::number(dMin[1], 'g', 3));
    ui->edZMin->setText(QString::number(dMin[2], 'g', 3));

    ui->edXMax->setText(QString::number(dMax[0], 'g', 3));
    ui->edYMax->setText(QString::number(dMax[1], 'g', 3));
    ui->edZMax->setText(QString::number(dMax[2], 'g', 3));

    ui->edXEx->setText(QString::number(dEx[0], 'g', 3));
    ui->edYEx->setText(QString::number(dEx[1], 'g', 3));
    ui->edZEx->setText(QString::number(dEx[2], 'g', 3));

    ui->edXLen->setText(QString::number(dLen[0], 'g', 3));
    ui->edYLen->setText(QString::number(dLen[1], 'g', 3));
    ui->edZLen->setText(QString::number(dLen[2], 'g', 3));
}

void BlankBoxDlg::OnAutoComputer()
{
    if(!m_mesh)
        return;

    KWBox box = m_mesh->GetBouningBox();

    //更新数据
    dEx[0] = ui->edXEx->text().toDouble();
    dEx[1] = ui->edYEx->text().toDouble();
    dEx[2] = ui->edZEx->text().toDouble();

    box.xmin -= dEx[0];
    box.ymax -= dEx[1];
    box.zmin -= dEx[2];
    box.xmax += dEx[0];
    box.ymax += dEx[1];
    box.zmax += dEx[2];

    SetBox(box);

    Update();
}

void BlankBoxDlg::OnOk()
{
    //更新数据
    dMax[0] = ui->edXMax->text().toDouble();
    dMax[1] = ui->edYMax->text().toDouble();
    dMax[2] = ui->edZMax->text().toDouble();
    dMin[0] = ui->edXMin->text().toDouble();
    dMin[1] = ui->edYMin->text().toDouble();
    dMin[2] = ui->edZMin->text().toDouble();

    m_data.reset(new BlankBox());
    m_data->SetValue(BlankBox::BOX_XMAX, QString::number(dMax[0]));
    m_data->SetValue(BlankBox::BOX_YMAX, QString::number(dMax[1]));
    m_data->SetValue(BlankBox::BOX_ZMAX, QString::number(dMax[2]));
    m_data->SetValue(BlankBox::BOX_XMIN, QString::number(dMin[0]));
    m_data->SetValue(BlankBox::BOX_YMIN, QString::number(dMin[1]));
    m_data->SetValue(BlankBox::BOX_ZMIN, QString::number(dMin[2]));
    QDialog::accept();
}

void BlankBoxDlg::OnCancle()
{
    QDialog::reject();
}

ToolDataSPtr BlankBoxDlg::GetResult()
{
    return m_data;
}

void BlankBoxDlg::SetBox(const KWBox &box)
{
    dMin[0] = box.xmin;
    dMin[1] = box.ymin;
    dMin[2] = box.zmin;
    dMax[0] = box.xmax;
    dMax[1] = box.ymax;
    dMax[2] = box.zmax;

    dCenter[0] = box.center()[0];
    dCenter[1] = box.center()[1];
    dCenter[2] = box.center()[2];

    dLen[0] = box.xmax - box.xmin;
    dLen[1] = box.ymax - box.ymin;
    dLen[2] = box.zmax - box.zmin;
}
