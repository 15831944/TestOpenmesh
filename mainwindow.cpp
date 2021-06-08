#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSplitter>
#include <QTreeWidget>
#include <QFileDialog>
#include <QTreeWidget>
#include <QMessageBox>
#include <QActionGroup>
#include <QMenu>

#include "Widget/ncoccctrlview.h"
#include "Widget/nctreewidgetitem.h"
#include "Widget/ncpathdialog.h"
#include "Widget/MeshViewer/MeshViewerWidget.h"
#include "Widget/BlankDlg/blankboxdlg.h"
#include "Widget/BlankDlg/blankcylinderdlg.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitForm();
    InitControl();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitForm()
{
    //菜单
    //模型：打开模型，创建模型
    m_menuModel = new QMenu(this);
    m_menuModel->addAction(ui->actionOpenModel);
    m_menuModel->addSection(QString("-----------"));
    QMenu *subCreateModel = new QMenu(QString::fromLocal8Bit("创建模型"), this);
    subCreateModel->addAction(ui->actionCreateBall);
    subCreateModel->addAction(ui->actionCreateCube);
    subCreateModel->addAction(ui->actionCreateCylinder);
    m_menuModel->addMenu(subCreateModel);

    m_menuModelOp = new QMenu(this);
    m_menuModelOp->addAction(ui->actionModel);
    m_menuModelOp->addAction(ui->actionModelWire);
    m_menuModelOp->addSection(QString("-----------"));
    m_menuModelOp->addAction(ui->actionModelAttr);
    m_menuModelOp->addAction(ui->actionExportModel);
    m_menuModelOp->addSection(QString("-----------"));
    m_menuModelOp->addAction(ui->actionDeleteModel);

    m_menuTool = new QMenu(this);
    QMenu *subCreateTool = new QMenu(QString::fromLocal8Bit("创建刀具"), this);
    subCreateTool->addAction(ui->actionCreateBallTool);
    m_menuTool->addMenu(subCreateTool);

    m_menuTool = new QMenu(this);

    m_menuToolPath = new QMenu(this);
    QMenu *subCreatePath = new QMenu(QString::fromLocal8Bit("创建刀具路径"), this);
    subCreatePath->addAction(ui->actionCreatePath);
    m_menuToolPath->addMenu(subCreatePath);

    m_menuToolPathOp = new QMenu(this);
    m_menuToolPathOp->addAction(ui->actionViewToolPath);

    //毛胚
    m_menuBlank = new QMenu(this);
    QMenu *subCreateBlank = new QMenu(QString::fromLocal8Bit("创建毛胚"), this);
    subCreateBlank->addAction(ui->actionCBlankCube);
    subCreateBlank->addAction(ui->actionaCBlankCylinder);
    m_menuBlank->addMenu(subCreateBlank);

    m_splitter = new QSplitter(this);
    setCentralWidget(m_splitter);

    m_pTreeWidget = new QTreeWidget(this);
    m_splitter->addWidget(m_pTreeWidget);
    //TreeWidget
    {
        QStringList strList;
        strList << QString::fromLocal8Bit("模型")
                << QString::fromLocal8Bit("毛胚")
                << QString::fromLocal8Bit("刀具")
                << QString::fromLocal8Bit("路径");

        m_pTreeWidget->addTopLevelItem(new NCTreeWidgetItem(QStringList() << strList[0], m_menuModel));
        m_pTreeWidget->addTopLevelItem(new NCTreeWidgetItem(QStringList() << strList[1], m_menuBlank));
        m_pTreeWidget->addTopLevelItem(new NCTreeWidgetItem(QStringList() << strList[2], m_menuTool));
        m_pTreeWidget->addTopLevelItem(new NCTreeWidgetItem(QStringList() << strList[3], m_menuToolPath));
        m_pTreeWidget->setHeaderHidden(true);
    }

    m_pView = new MeshViewerWidget(this);
    m_splitter->addWidget(m_pView);

    m_splitter->setStretchFactor(0, 2);
    m_splitter->setStretchFactor(1, 4);
    m_splitter->setOpaqueResize();

    m_pTreeWidget->installEventFilter(this);
}

void MainWindow::InitControl()
{
    connect(ui->actionOpenModel, &QAction::triggered, this, &MainWindow::OpenModel);
    connect(ui->actionSection, &QAction::triggered, this, &MainWindow::OnSection);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::OnAbout);
    connect(ui->actionModel, &QAction::triggered, this, &MainWindow::OnModelView);
    connect(ui->actionModelWire, &QAction::triggered, this, &MainWindow::OnModelWire);
    connect(ui->actionCreatePath, &QAction::triggered, this, &MainWindow::OnCreatePath);
    connect(ui->actionViewToolPath, &QAction::triggered, this, &MainWindow::OnPathView);
    connect(ui->actionXYGrid, &QAction::triggered, this, &MainWindow::OnXYGrid);
    connect(ui->actionXZGrid, &QAction::triggered, this, &MainWindow::OnXZGrid);
    connect(ui->actionYZGrid, &QAction::triggered, this, &MainWindow::OnYZGrid);
    connect(ui->actionXY, &QAction::triggered, this,  &MainWindow::OnXYPlane);
    connect(ui->actionXZ, &QAction::triggered, this,  &MainWindow::OnXZPlane);
    connect(ui->actionYZ, &QAction::triggered, this,  &MainWindow::OnYZPlane);
    connect(ui->actionYX, &QAction::triggered, this,  &MainWindow::OnYXPlane);
    connect(ui->actionZX, &QAction::triggered, this,  &MainWindow::OnZXPlane);
    connect(ui->actionZY, &QAction::triggered, this,  &MainWindow::OnZYPlane);
    connect(ui->actionEnterPathPlay, &QAction::triggered, this,  &MainWindow::OnEnterPathSimu);
    connect(ui->actionExitPathShow, &QAction::triggered, this,  &MainWindow::OnExitPathSimu);
    connect(ui->actionaCBlankCylinder, &QAction::triggered, this,  &MainWindow::OnCreateBlankCylinder);
    connect(ui->actionCBlankCube, &QAction::triggered, this,  &MainWindow::OnCreateBlankCube);

}

void MainWindow::OpenModel()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open mesh file"),
        tr(""),
        tr("Model Files (*.obj *.stl *.off *.stp *.step);;"
        "OBJ Files (*.obj);;"
        "OFF Files (*.off);;"
        "STL Files (*.stl);;"
        "STEP Files (*.stp *.step)"
        "All Files (*)"));
    if(!fileName.isEmpty() && g_pMesh)
    {
        QByteArray ba = fileName.toLocal8Bit();
        string stdFileName = ba.data();
        bool res = g_pMesh->LoadFile(stdFileName);
        if(res)
        {
            g_pMesh->ComputerBox();
//            m_pView->open_mesh_gui(fileName);
            m_pView->set_mesh_gui(static_cast<NCTriaOpenMesh *>(g_pMesh.get())->GetMesh());
//            m_pView->UpdateList();
//            m_pView->UpdateShapeList();
//            m_pView->ResetView();
            if(m_pTreeWidget->topLevelItem(MODEL)->child(0))
            {
                m_pTreeWidget->topLevelItem(MODEL)->child(0)->setText(0, g_pMesh->GetMeshName().c_str());
            }
            else
            {
                m_pTreeWidget->topLevelItem(MODEL)->addChild(
                            new NCTreeWidgetItem(QStringList()
                                                 << g_pMesh->GetMeshName().c_str()
                                                 , m_menuModelOp));
                m_pTreeWidget->topLevelItem(MODEL)->setExpanded(true);
            }
            cout << "read file:[" << stdFileName << "] SUCCESSD" << endl;
            g_pMesh->Output();
        }
        else
        {
            cout << "read file:[" << stdFileName << "] FAILD" << endl;
        }
    }
}

void MainWindow::OnSection()
{
}

void MainWindow::OnAbout()
{
//    float Version = 0.00;
//    char VerString[20]{0};
//    unsigned int VersionData = 0;

//    if(g_pBoardBroker  && g_pBoardBroker->GetBoard())
//        g_pBoardBroker->GetBoard()->GetSoftVersion(&Version, VerString, &VersionData);

    QString info = QString::fromLocal8Bit("名称：CNC\n"
                                          "版本：%1\n"
//                                          "控制器型号：%2-%3\n"
//                                          "固件版本：%4\n"
                                          "公司：杭州开维科技有限公司")
            .arg(qApp->applicationVersion())
//            .arg(static_cast<double>(Version))
//            .arg(VersionData)
//            .arg(VerString)
            ;

    QMessageBox::information(nullptr, u8"关于", info);
}

void MainWindow::OnModelView()
{
    g_pMesh->SetVisiable(ui->actionModel->isChecked());
    m_pView->updateGL();
//    m_pView->SetModelVisiable(ui->actionModel->isChecked());

}

void MainWindow::OnPathView()
{
    auto index = m_pTreeWidget->currentIndex();

    if(index.isValid())
    {
        auto path = g_paths[index.column()];
        m_pView->set_path(path);
    }

    m_pView->updateGL();
}

void MainWindow::OnModelWire()
{
    m_pView->slotDrawMode( ui->actionModelWire->isChecked() ? ACT_HIDING_LINE : ACT_SMOOTH);// Smooth Flat
    m_pView->updateGL();
//    m_pView->SetDrawMode(ui->actionModelWire->isChecked() ? GL_LINE : GL_FILL);
}

void MainWindow::AddTool()
{

}

void MainWindow::OnCreatePath()
{
    NCPathDialog dlg;
    int res = dlg.exec();

    if(res == QDialog::Accepted)
    {
        double offset = dlg.GetOffset();

        auto vecpoly = g_pMesh->ProcessPolyLine(offset);
        NCPath poly;
        for(size_t i = 0; i < vecpoly.size(); ++i)
        {
            poly.insert(poly.end(), vecpoly[i].begin(), vecpoly[i].end());
        }

        if(poly.size() > 2)
        {
            poly.SetOffset(offset);
            g_paths.push_back(poly);
            m_pView->set_path(poly);
            m_pTreeWidget->topLevelItem(PATH)->addChild(
                        new NCTreeWidgetItem(QStringList()
                                             << poly.GetFileName().c_str()
                                             , m_menuToolPathOp));
            m_pTreeWidget->topLevelItem(PATH)->setExpanded(true);
        }
        m_pView->updateGL();
    }
}

void MainWindow::AddBlank()
{

}

void MainWindow::OnXYGrid()
{
    m_pView->setGridxyVisiable(ui->actionXYGrid->isChecked());
}

void MainWindow::OnXZGrid()
{
    m_pView->setGridxzVisiable(ui->actionXZGrid->isChecked());
}

void MainWindow::OnYZGrid()
{
    m_pView->setGridyzVisiable(ui->actionYZGrid->isChecked());
}

void MainWindow::OnXYPlane()
{
    m_pView->showXYPlane();
}

void MainWindow::OnXZPlane()
{
    m_pView->showXZPlane();
}

void MainWindow::OnYZPlane()
{
    m_pView->showYZPlane();
}

void MainWindow::OnYXPlane()
{
    m_pView->showYXPlane();
}

void MainWindow::OnZXPlane()
{
    m_pView->showZXPlane();
}

void MainWindow::OnZYPlane()
{
    m_pView->showZYPlane();
}

void MainWindow::OnEnterPathSimu()
{
    m_pView->start_simu();
}

void MainWindow::OnExitPathSimu()
{
    m_pView->stop_simu();
}

void MainWindow::OnCreateBlankCylinder()
{
    BlankCylinderDlg dlg(g_pMesh);
    dlg.exec();

    g_pBlank = dlg.GetResult();
    g_pBlank->SetVisiable(true);
}

void MainWindow::OnCreateBlankCube()
{
    BlankBoxDlg dlg(g_pMesh);
    dlg.exec();


    g_pBlank = dlg.GetResult();
    g_pBlank->SetVisiable(true);

}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_pTreeWidget)
    {
        switch(event->type() )
        {
        case QEvent::ContextMenu:
        {
            auto item = dynamic_cast<NCTreeWidgetItem *>(m_pTreeWidget->currentItem());
            if(item)
            {
                if(item->Menu())
                    item->Menu()->popup(QCursor::pos());
                return true;
            }

            break;
        }
        default:
            break;
        }

    }
    return QMainWindow::eventFilter(watched, event);
}

