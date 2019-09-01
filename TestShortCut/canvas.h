#ifndef CANVAS_H
#define CANVAS_H

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QDesktopWidget>
#include <QScreen>
#include <QPixmap>
#include <QGuiApplication>
#include <QPen>
#include <QApplication>

#include <QPushButton>

#include <QFileDialog>

#include <QHBoxLayout>
#include <QClipboard>

#include <QComboBox>

#include <QColorDialog>
#include <QFontDialog>
#include <QRectF>

#include "linepaint.h"
#include "rectpaint.h"
#include "textpaint.h"

enum DrawStatus {
  waitDraw = 0,
  drawing,
  drawed,
};

class Canvas : public QWidget
{
public:
    Canvas(QWidget *parent = 0);
    ~Canvas();

    void setBgPixmap(QPixmap &px);
    void shootScreen(QRectF &rect);

    void canvasInit();
    void initToolBar();
    void showToolBar();
    void hideToolBar();
    void refrashToolBar();

    quint8 caputerRect(QRectF t_rect, qreal t_x, qreal t_y);
    RectPaint getRectF(QPointF p1, QPointF p2); // 通过两个坐标点生成矩形
    void changeLanguage(QString lan);

signals:

public slots:
    void slt_drawLine();
    void slt_drawRect();
    void slt_drawEllipse();
    void slt_drawText();
    void slt_saveFile(); // 保存到文件
    void slt_saveClipboard(); // 保存到剪切板
    void slt_cancel();

    void slt_changePenWidth(QString s);
    void slt_changePenColor();
    void slt_changePenStyle(int index);
    void slt_changeTextColor();
    void slt_changeTextFont();

private:
    int screen_width = 0, screen_height = 0;
    quint8 okFlag = 0;
    QPointF pointS;
    QPointF pointE;
    RectPaint shotArea; // 截图区域

    QList<LinePaint> lineList;
    QList<RectPaint> rectList;
    QList<RectPaint> ellipseList;
    QList<TextPaint *> textList;

    QPointF pointDrag;

    DrawStatus rectFlag = DrawStatus::waitDraw;

    quint8 drawEditFlag = 0;
    QPixmap fullPixmap;
    QPixmap originalPixmap;

    QWidget *toolbar;
    QPushButton *btn_saveFile;
    QPushButton *btn_saveClipboard;
    QPushButton *btn_cancel;
    QPushButton *btn_drawLine;
    QPushButton *btn_drawRect;
    QPushButton *btn_drawEllipse;
    QPushButton *btn_drawText;

    QPen drawPen;
    QWidget *shapeToolBar;
    QWidget *textToolBar;

    int toolBarWidth;
    int toolBarHeight;

    QComboBox *cbx_lineSize;
    QPushButton *btn_colorSelect;
    QComboBox *cbx_lineStyle;

    QPushButton *btn_colorText;
    QPushButton *btn_FontText;
    QColor textColor;
    QFont textfont;

    QClipboard *clipboard;

    quint8 cursorCaptureFlag = 0;

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
};

#endif // CANVAS_H
