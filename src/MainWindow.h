#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Canvas;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void clearCanvas();
    void pickColor();
    void saveImage();

private:
    Canvas *canvas;
};

#endif
