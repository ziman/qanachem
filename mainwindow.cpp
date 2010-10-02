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

void MainWindow::saveView()
{
    QString fname = QFileDialog::getSaveFileName(this, "Save view as image", "snapshots/", "PNG files (*.png);;All files (*)");
    if (fname.isNull()) return;

    // add the PNG suffix if needed
    if (!fname.endsWith(".png", Qt::CaseInsensitive)) fname.append(".png");

    QImage img = ui->display->grabFrameBuffer(false);
    img.save(fname, "PNG", 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFile()
{
    QString fname = QFileDialog::getOpenFileName(this, "Load molecule", "molecules/", "MOL/SDF files (*.mol *.sdf);;All files (*)");
    if (fname.isNull()) return;

    try {
        Molecule mol(fname);
        ui->display->setMolecule(mol);

        // auto-set render mode
        int mode = 0;
        int count = mol.atoms.count();
        if (count < 100) mode = 0;
        if (count > 100) mode = 1;
        if (count > 1000) mode = 2;

        ui->comboBox->setCurrentIndex(mode);
    } catch (...) {
        QMessageBox::critical(this, "Load molecule", "Unable to load " + fname + ".");
    }
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
