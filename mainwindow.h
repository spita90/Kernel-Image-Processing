#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_load_image_btn_clicked();

    void on_start_btn_clicked();

private:
    QString imagePath;
    QImage img;
    Ui::MainWindow *ui;
    void createActions();
};
#endif // MAINWINDOW_H
