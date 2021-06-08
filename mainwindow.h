#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QSplitter;
class QTreeWidget;
class NCOCCCtrlView;
class QActionGroup;
class MeshViewerWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum
    {
        MODEL,
        BLANK,
        TOOLS,
        PATH,
    };


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitForm();
    void InitControl();

public Q_SLOTS:
    void OpenModel();
    void OnSection();
    void OnAbout();
    void OnModelView();
    void OnPathView();
    void OnModelWire();
    void AddTool();
    void OnCreatePath();
    void AddBlank();
    void OnXYGrid();
    void OnXZGrid();
    void OnYZGrid();
    void OnXYPlane();
    void OnXZPlane();
    void OnYZPlane();
    void OnYXPlane();
    void OnZXPlane();
    void OnZYPlane();
    void OnEnterPathSimu();
    void OnExitPathSimu();
    void OnCreateBlankCylinder();
    void OnCreateBlankCube();

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    QSplitter *m_splitter;

    MeshViewerWidget *m_pView;
    QTreeWidget *m_pTreeWidget;
    QMenu *m_menuModel;
    QMenu *m_menuModelOp;
    QMenu *m_menuBlank;
    QMenu *m_menuBlankOp;
    QMenu *m_menuTool;
    QMenu *m_menuToolOp;
    QMenu *m_menuToolPath;
    QMenu *m_menuToolPathOp;
};
#endif // MAINWINDOW_H
