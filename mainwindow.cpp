#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
    timer->start(50);
}

void advance(QScrollBar * sb, int step)
{
    if (sb->value() == sb->maximum())
        sb->setValue(sb->minimum());

    sb->setValue(sb->value() + step);
}

void MainWindow::tick()
{
    int step = 1;

    if (ui->cbX->isChecked()) advance(ui->sbX, step);
    if (ui->cbY->isChecked()) advance(ui->sbY, step);
    if (ui->cbZ->isChecked()) advance(ui->sbZ, step);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFile()
{
    QString fname = QFileDialog::getOpenFileName(this, "Load molecule", "molecules/", "MOL files (*.mol);;All files (*)");
    if (fname.isNull()) return;

    ui->display->setMolecule(Molecule(fname));
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
