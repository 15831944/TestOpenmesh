#include "ncpathdialog.h"
#include "ui_ncpathdialog.h"

NCPathDialog::NCPathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NCPathDialog)
{
    ui->setupUi(this);
}

NCPathDialog::~NCPathDialog()
{
    delete ui;
}

double NCPathDialog::GetOffset()
{
    return ui->edOffset->text().toDouble();
}
