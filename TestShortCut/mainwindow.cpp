#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QWidget *m_parent = new QWidget;
    setParent(m_parent);

    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::WindowStaysOnTopHint);
    this->setWindowTitle(tr("Setting"));

    QDesktopWidget* desktopWidget = QApplication::desktop();
    // 获取可用桌面大小
    QRect deskRect = desktopWidget->screenGeometry();

    int screenWidth = deskRect.width();
    int screenHeight = deskRect.height();

//    int screenHeight = deskRect.height();

    qDebug() << screenWidth << screenHeight;

// 截图 api
//    QScreen* screen = QGuiApplication::primaryScreen();

//    int x = 0;
//    int y = 0;
//    int width = screenWidth;
//    int height = screenHeight;

//    QString path = "/home/gog/Desktop/1.png";
//    orginalPixmap = screen->grabWindow(0, x, y, width, height);
//    orginalPixmap.save(path, "png");

}

MainWindow::~MainWindow()
{

}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton) {
        QPoint cursor = QCursor::pos();
        int x = cursor.x();
        int y = cursor.y();

        qDebug() << x << y;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    Q_UNUSED(e)
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    Q_UNUSED(e)
}
