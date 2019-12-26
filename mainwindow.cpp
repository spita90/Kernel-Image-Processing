#include <QPixmap>
#include <QImage>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageprocessor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget::setWindowTitle("Kernel Image Processing with C++11 MultiThreading");

    unsigned threadsSupported = std::thread::hardware_concurrency();
    if(threadsSupported == 0)
    {
        threadsSupported = 2;
        QMessageBox alert;
        alert.setText("Impossibile determinare # di Thread possibili. Imposto a 2.");
        alert.exec();
    }
    ui->thr_sb->setValue(int(threadsSupported));

    ui->effect_cb->addItems({"Box Blur", "Gaussian Blur"});

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_load_image_btn_clicked()
{
    imagePath = QFileDialog::getOpenFileName(this,"Apri immagine", QDir::homePath(),"Immagine (*.bmp *.jpg *.jpeg *.tiff *.png);;Tutti i files (*)");
    if((imagePath.length() == 0) || (!img.load(imagePath)))
    {
        QMessageBox alert;
        alert.setText("Nessuna immagine caricata");
        alert.exec();
        return;
    }
    ui->image_ref->setPixmap(QPixmap::fromImage(img.scaled(ui->image_ref->width(),ui->image_ref->height(),Qt::KeepAspectRatioByExpanding)));
    ui->image_st->clear();
    ui->image_mt->clear();
    ui->start_btn->setEnabled(true);

}

void MainWindow::on_start_btn_clicked()
{
    ui->start_btn->setEnabled(false);
    ui->load_image_btn->setEnabled(false);
    QString effect = ui->effect_cb->currentText();
    uint threadNumber = static_cast<uint>(ui->thr_sb->value()) ;
    int passes = ui->pass_sb->value();
    QImage processed = img;
    // processo l'immagine mt
    qint64* times_mt = new qint64[passes];
    for(int i = 0; i < passes; i++)
    {
        processed = ImageProcessor::applyEffect(effect, processed, threadNumber, times_mt[i]);
    }
    qint64 total_mt_time=0;
    for(int i=0;i<passes;i++)
        total_mt_time+=times_mt[i];
    ui->time_mt_lbl->setText(QString::number(total_mt_time) + " ms");
    ui->image_mt->setPixmap(QPixmap::fromImage(processed.scaled(ui->image_mt->width(),ui->image_mt->height(),Qt::KeepAspectRatioByExpanding)));

    //processo l'immagine st
    processed = img;
    qint64* times_st = new qint64[passes];
    for(int i = 0; i < passes; i++)
    {
        processed = ImageProcessor::applyEffect(effect, processed, 1, times_st[i]);
    }
    qint64 total_st_time=0;
    for(int i=0;i<passes;i++)
        total_st_time+=times_st[i];
    ui->time_st_lbl->setText(QString::number(total_st_time) + " ms");
    ui->image_st->setPixmap(QPixmap::fromImage(processed.scaled(ui->image_st->width(),ui->image_st->height(),Qt::KeepAspectRatioByExpanding)));

    ui->load_image_btn->setEnabled(true);
    ui->start_btn->setEnabled(true);
}
