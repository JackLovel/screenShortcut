#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QRect>
#include <QSize>
#include <QDesktopWidget>
#include <QDebug>
#include <QScreen>
#include <QPixmap>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QHotkey>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QPixmap orginalPixmap;
};

#endif // MAINWINDOW_H
