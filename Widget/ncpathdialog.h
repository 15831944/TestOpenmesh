#ifndef NCPATHDIALOG_H
#define NCPATHDIALOG_H

#include <QDialog>

namespace Ui {
class NCPathDialog;
}

class NCPathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NCPathDialog(QWidget *parent = nullptr);
    ~NCPathDialog();

    double GetOffset();
private:
    Ui::NCPathDialog *ui;
};

#endif // NCPATHDIALOG_H
