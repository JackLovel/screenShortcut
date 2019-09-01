#include "canvas.h"

#include <QFileInfo>

#include <QDebug>
#include <QDateTime>
#include "operateset.h"
#include <QTranslator>

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    okFlag = 1;
    const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
    screen_width = screenGeometry.width();
    screen_height = screenGeometry.height();

    this->raise();
    this->showFullScreen();

    clipboard = QApplication::clipboard();

    setMouseTracking(true);

    canvasInit();

    initToolBar();

    drawEditFlag = 0;
}

void Canvas::canvasInit()
{
    pointS.rx() = 0;
    pointS.ry() = 0;
    pointE.rx() = 0;
    pointE.ry() = 0;
    pointDrag.rx() = 0;
    pointDrag.ry() = 0;
    rectFlag = DrawStatus::waitDraw;

    lineList.clear();
    rectList.clear();
    ellipseList.clear();
    textList.clear();

    drawPen.setBrush(Qt::red);
    drawPen.setWidthF(2);
    drawPen.setStyle(Qt::SolidLine);
}

void Canvas::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (rectFlag == DrawStatus::waitDraw) {
            pointS.rx() = e->x();
            pointS.ry() = e->y();
            pointE.rx() = e->x();
            pointE.ry() = e->y();
            shotArea = getRectF(pointS, pointE);
            rectFlag = DrawStatus::drawing;
        }
        else if(rectFlag == DrawStatus::drawed)
        {
            if (drawEditFlag == 0) {
                pointDrag.setX(e->x());
                pointDrag.setY(e->y());
            } else if(drawEditFlag == 1) {  // draw line
                pointS.rx() = e->x();
                pointS.ry() = e->y();
                pointE.rx() = e->x();
                pointE.ry() = e->y();

                LinePaint tempLine(pointS.toPoint(), pointE.toPoint());
                tempLine.setPen(drawPen);
                lineList.append(tempLine);
            } else if (drawEditFlag == 2) { // draw rect
                pointS.rx() = e->x();
                pointS.ry() = e->y();
                pointE.rx() = e->x();
                pointE.ry() = e->y();

                RectPaint tempRect = getRectF(pointS, pointE);
                tempRect.setPen(drawPen);
                rectList.append(tempRect);
            } else if (drawEditFlag == 3) { // 画椭圆
                pointS.rx() = e->x();
                pointS.ry() = e->y();
                pointE.rx() = e->x();
                pointE.ry() = e->y();

                RectPaint tempRect = getRectF(pointS, pointE);
                tempRect.setPen(drawPen);
                ellipseList.append(tempRect);
            } else if (drawEditFlag == 4) { // add text
                int x = e->x();
                int y = e->y();
                TextPaint *curText = new TextPaint(this);
                int cnt = textList.length();

                for (int i = 0;i < cnt; i++) {
                    textList.at(i)->clearFocus();
                }

                QFontMetrics fm(textfont);

                curText->setGeometry(x, y, 10, fm.height() + 2);
                curText->setFont(textfont);

                QPalette palette;
                palette.setColor(QPalette::Text, textColor);
                curText->setPalette(palette);

                curText->setVisible(true);
                textList.append(curText);
                curText->setFocus();
                QWidget::mousePressEvent(e);
                update();
            }
        }
    } else if (e->button() == Qt::RightButton) {
        setCursor(Qt::ArrowCursor);

        if (rectFlag == DrawStatus::waitDraw) {
            slt_cancel();
        } else {
            canvasInit();
            hideToolBar();
            update();
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *e)
{
    if (e->button() & Qt::LeftButton)
    {
        if (rectFlag == DrawStatus::drawing) {
            pointE.setX(e->x());
            pointE.setY(e->y());
            shotArea = getRectF(pointS, pointE);
        } else if (rectFlag == DrawStatus::drawed) {
            QPointF tempTl, tempBR;

            tempTl = shotArea.topLeft();
            tempBR = shotArea.bottomRight();

            switch(cursorCaptureFlag)
            {
            case 0:
                setCursor(Qt::ArrowCursor);
                break;
            case 1:
                setCursor(Qt::SizeFDiagCursor);
                tempTl.setX(e->x());
                tempTl.setY(e->y());
                break;
            case 2:
                setCursor(Qt::SizeBDiagCursor);
                tempTl.setX(e->x());
                tempBR.setY(e->y());
                break;
            case 3:
                setCursor(Qt::SizeHorCursor);
                tempTl.setX(e->x());
                break;
            case 4:
                setCursor(Qt::SizeBDiagCursor);
                tempBR.setX(e->x());
                tempTl.setY(e->y());
                break;
            case 5:
                setCursor(Qt::SizeFDiagCursor);
                tempBR.setX(e->x());
                tempBR.setY(e->y());
                break;
            case 6:
                setCursor(Qt::SizeHorCursor);
                tempBR.setX(e->x());
                shotArea = getRectF(pointS, pointE);
                break;
            case 7:
                setCursor(Qt::SizeVerCursor);
                tempTl.setY(e->y());
                break;
            case 8:
                setCursor(Qt::SizeVerCursor);
                tempBR.setY(e->y());
                break;
            case 9:
                if (drawEditFlag == 0) {
                    setCursor(Qt::SizeAllCursor);
                    qreal dx = e->x() - pointDrag.x();
                    qreal dy = e->y() - pointDrag.y();

                    pointDrag.setX(e->x());
                    pointDrag.setY(e->y());

                    if ((tempTl.x()+dx)>0 && (tempBR.x()+dx) < screen_width){
                        tempTl.rx() += dx;
                        tempBR.rx() += dx;
                    }

                    if ((tempTl.y() + dy) > 0 && (tempBR.y() + dy) < screen_height)
                    {
                        tempTl.ry() += dy;
                        tempBR.ry() += dy;
                    }
                } else if (drawEditFlag == 1) {
                    pointE.setX(e->x());
                    pointE.setY(e->y());

                    lineList.last().setPoints(pointS.toPoint(), pointE.toPoint());
                } else if (drawEditFlag == 2) {
                    pointE.setX(e->x());
                    pointE.setY(e->y());

                    rectList.last().setTopLeft(pointS);
                    rectList.last().setBottomRight(pointE);
                } else if (drawEditFlag == 3) {
                    pointE.setX(e->x());
                    pointE.setY(e->y());

                    ellipseList.last().setTopLeft(pointS);
                    ellipseList.last().setBottomRight(pointE);
                }
                break;
            }

            showArea.setTopLeft(tempTl);
            showArea.setBottomRight(tempBR);

            showToolBar();
        }
        update();
    } else if (e->buttons() == Qt::NoButton) { // no press button
        if (rectFlag == drawed) {
            cursorCaptureFlag = caputerRect(shotArea, e->x(), e->y());

            switch (cursorCaptureFlag)
            {
                case 0:
                    setCursor(Qt::ArrowCursor);
                    break;
                case 1:
                    setCursor(Qt::SizeFDiagCursor);
                    break;
                case 2:
                    setCursor(Qt::SizeBDiagCursor);
                    break;
                case 3:
                    setCursor(Qt::SizeHorCursor);
                    break;
                case 4:
                    setCursor(Qt::SizeBDiagCursor);
                    break;
                case 5:
                    setCursor(Qt::SizeFDiagCursor);
                    break;
                case 6:
                    setCursor(Qt::SizeHorCursor);
                    break;
                case 7:
                    setCursor(Qt::SizeVerCursor);
                    break;
                case 8:
                    setCursor(Qt::SizeVerCursor);
                    break;
                case 9:
                    if (drawEditFlag == 0) {
                        setCursor(Qt::SizeAllCursor);
                    } else {
                        setCursor(Qt::ArrowCursor);
                    }
                    break;
            }
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (rectFlag == DrawStatus::drawing) {
            pointE.setX(e->x());
            pointE.setY(e->y());
            shotArea = getRectF(pointS, pointE);
            rectFlag = DrawStatus::drawed;
            showToolBar();
        }
    } else if (rectFlag == DrawStatus::drawed) {
        if (drawEditFlag == 0) {
            showToolBar();
        } else if (drawEditFlag == 1) {
            if ((pointS.x() == pointE.x()) && (pointE.y() == pointS.y())) {
                lineList.removeLast();
            }
        } else if (drawEditFlag == 2) {
            if ((pointS.x() == pointE.x()) && (pointS.y() == pointE.y())) {
                rectList.removeLast();
            }
        } else if (drawEditFlag == 3) {
            if ((pointS.x() == pointE.x()) && (pointS.y() == pointE.y())) {
                ellipseList.removeLast();
            }
        }
    }
    update();

}

void Canvas::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QPixmap tempmask(screen_width, screen_height);
    tempmask.fill(QColor(0, 0, 0, 160));

    painter.drawPixmap(0, 0, fullPixmap);
    painter.drawPixmap(0, 0, tempmask);

    switch(rectFlag) {
    case DrawStatus::waitDraw:
        break;
    case DrawStatus::drawing:
    {
        painter.setPen(QPen(Qt::green, 2, Qt::DashLine));
        painter.drawRect(shotArea);
        painter.drawPixmap(shotArea, fullPixmap, shotArea);
        break;
    }
    case DrawStatus::drawed:
    {
        painter.setPen(QPen(Qt::green, 2, Qt::DashLine));
        painter.drawRect(shotArea);
        painter.drawPixmap(shotArea, fullPixmap, shotArea);
        break;
    }
    default:
    {
        break;
    }

    }

    quint16 len = lineList.length();
    if (len) {
        for (quint16 i = 0; i < len; i++) {
            painter.setPen(lineList[i].getPen());
            painter.drawLine(lineList[i]);
        }
    }

    len = rectList.length();
    if (len) {
        for (quint16 i = 0; i < len; i++) {
            painter.setPen(rectList[i].getPen());
            painter.drawRect(rectList[i]);
        }
    }

    len = ellipseList.length();
    if (len) {
        for (quint16 i = 0; i < len; i++) {
            painter.setPen(ellipseList[i].getPen());
            painter.drawEllipse(ellipseList[i]);
        }
    }

    QWidget::paintEvent(e);
}

void Canvas::setBgPixmap(QPixmap &px)
{
    fullPixmap = px;
}

void Canvas::shootScreen(QRectF &rect)
{
    QScreen *screen = QGuiApplication::primaryScreen();

    originalPixmap = screen->grabWindow(0, rect.x(), rect.y(), rect.width(), rect.height());
}

void Canvas::initToolBar()
{
    toolbar = new QWidget(this);

    QWidget *MainToolBar = new QWidget();
    QHBoxLayout *mainToolLayout = new QHBoxLayout();

    btn_cancel = new QPushButton(tr("quit"));
    btn_saveClipboard = new QPushButton(tr("copy"));
    btn_saveFile = new QPushButton(tr("Save"));

    btn_drawLine = new QPushButton(tr("Line"));
    btn_drawRect = new QPushButton(tr("Rect"));
    btn_drawEllipse = new QPushButton(tr("Ellipse"));
    btn_drawText = new QPushButton(tr("Text"));

    btn_cancel->setStyleSheet("background-color: rgb(255, 255, 255);");
    btn_saveClipboard->setStyleSheet("background-color: rgb(255, 255, 255);");
    btn_saveFile->setStyleSheet("background-color: rgb(255, 255, 255);");

    mainToolLayout->addWidget(btn_drawLine);
    mainToolLayout->addWidget(btn_drawRect);
    mainToolLayout->addWidget(btn_drawEllipse);
    mainToolLayout->addWidget(btn_drawText);
    mainToolLayout->addWidget(btn_cancel);

    mainToolLayout->addWidget(btn_saveClipboard);
    mainToolLayout->addWidget(btn_saveFile);
    mainToolLayout->setContentsMargins(0, 0, 0, 0);
    mainToolLayout->setSpacing(0);
    MainToolBar->setLayout(mainToolLayout);

    shapeToolBar = new QWidget();
    QHBoxLayout *shapeToolLayout = new QHBoxLayout();

    textToolBar = new  QWidget();
    QHBoxLayout *textToolLayout = new QHBoxLayout();

    cbx_lineSize = new QComboBox();
    btn_colorSelect = new QPushButton();
    cbx_lineStyle = new QComboBox();

    QStringList sizeItems;
    sizeItems <<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"<<"13"<<"14"<<"15"<<"16"<<"17"<<"18"<<"19"<<"20";
    cbx_lineSize->addItems(sizeItems);
    btn_colorSelect->setStyleSheet("background-color: rgb(255, 0, 0);");

    cbx_lineStyle->addItem(QIcon(":/pic/1.ico"), "");
    cbx_lineStyle->addItem(QIcon(":/pic/2.ico"), "");
    cbx_lineStyle->addItem(QIcon(":/pic/3.ico"), "");
    cbx_lineStyle->addItem(QIcon(":/pic/4.ico"), "");
    cbx_lineStyle->addItem(QIcon(":/pic/5.ico"), "");
    cbx_lineSize->setIconSize(QSize(80, 20));
    shapeToolLayout->addWidget(cbx_lineSize);
    shapeToolLayout->addWidget(btn_colorSelect);
    shapeToolLayout->addWidget(cbx_lineStyle);
    shapeToolLayout->addStretch();
    shapeToolLayout->setContentsMargins(0, 0, 0, 0);
    shapeToolLayout->setSpacing(0);
    shapeToolBar->setLayout(shapeToolLayout);
    shapeToolBar->setVisible(false);

    cbx_lineSize->setCurrentText(QString::number(drawPen::width()));
    QColor color = drawPen.brush().color();
    QString colorStyle = QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue());
    btn_colorSelect->setStyleSheet(colorStyle);

    switch (drawPen.style()) {
    case Qt::SolidLine:
        cbx_lineStyle->setCurrentIndex(0);
        break;
    case Qt::DashLine:
        cbx_lineStyle->setCurrentIndex(1);
        break;

    case Qt::DotLine:
        cbx_lineStyle->setCurrentIndex(2);
        break;
    case Qt::DashDotLine:
        cbx_lineStyle->setCurrentIndex(3);
        break;
    case Qt::DashDotDotLine:
        cbx_lineStyle->setCurrentIndex(4);
        break;
    default:
        cbx_lineStyle->setCurrentIndex(0);
        break;
    }

    btn_colorText = new QPushButton();
    btn_FontText = new QPushButton(tr("Font:10"));

    textColor = QColor(Qt::red);
    textfont = QFont("Times", 10, QFont::Bold);

    colorStyle = QString("background-color: rgb(%1, %2, %3);").arg(textColor.red()).arg(textColor.green()).arg(textColor.blue());
    btn_colorText->setStyleSheet(colorStyle);
    btn_FontText->setFont(textfont);

    textToolLayout->addWidget(btn_colorText);
    textToolLayout->addWidget(btn_FontText);
    textToolLayout->addStretch();
    textToolLayout->setContentsMargins(0,0,0,0);
    textToolLayout->setSpacing(0);

    textToolBar = new QWidget(this);
    textToolBar->setLayout(textToolLayout);
    textToolBar->setVisible(false);

    QVBoxLayout *toolLayout = new QVBoxLayout();
    toolLayout->addWidget(MainToolBar);
    toolLayout->addWidget(shapeToolBar);
    toolLayout->addWidget(textToolBar);
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(0);
    toolbar->setLayout(toolLayout);
    toolbar->setVisible(true);
    toolBarWidth = toolbar->width();
    toolBarHeight = toolbar->height();
    toolbar->setVisible(false);

    btn_drawLine->setStyleSheet("background-color: rgb(255, 255, 255);");
    btn_drawRect->setStyleSheet("background-color: rgb(255, 255, 255);");
    btn_drawEllipse->setStyleSheet("background-color: rgb(255, 255, 255);");
    btn_drawText->setStyleSheet("background-color: rgb(255, 255, 255);");

    hideToolBar();

    connect(btn_drawLine, &QPushButton::clicked, this, &Canvas::slt_drawLine);
    connect(btn_drawRect, &QPushButton::clicked, this, &Canvas::slt_drawRect);
    connect(btn_drawEllipse, &QPushButton::clicked, this, &Canvas::slt_drawEllipse);
    connect(btn_drawText, &QPushButton::clicked, this, &Canvas::slt_drawText);
    connect(btn_cancel, &QPushButton::clicked, this, &Canvas::slt_cancel);
    connect(btn_saveClipboard, &QPushButton::clicked, this, &Canvas::slt_saveClipboard);
    connect(btn_saveFile, &QPushButton::clicked, this, &Canvas::slt_saveFile);

    //
    connect(cbx_lineSize, SIGNAL(currentTextChanged(QString)), this, SLOT(slt_changePenWidth(QString)));
    connect(btn_colorSelect, SIGNAL(clicked(bool)), this, SLOT(slt_changePenColor()));
    connect(cbx_lineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slt_changePenStyle(int)));
    connect(btn_colorText, SIGNAL(clicked(bool)), this, SLOT(slt_changeTextColor()));
    connect(btn_FontText,  SIGNAL(clicked(bool)), this, SLOT(slt_changeTextFont()));
}

void Canvas::showToolBar()
{
    qreal x, y;

    int bar_width = toolBarWidth; // toolbar->width;
    int bar_height = toolBarHeight; // toolbar->height;
    int offset = 0;

    if (shotArea.bottomLeft().x() + bar_width + offset < screen_width) {
        x = shotArea.bottomLeft().x() + offset;
    } else {
        x = screen_width - (bar_width + offset);
    }

    if (screen_height-shotArea.bottomLeft().y > (bar_height + offset) ) {
        y = shotArea.bottomLeft().x() + offset;
    } else if(shotArea.topLeft().y() > (bar_height + offset)){
        x = shotArea.topLeft().x() - (bar_height + offset);
    } else if(shotArea.topLeft().y() < 0) {
        y = offset;
    } else {
        y = shotArea.topLeft().y() + offset;
    }

    toolbar->move(x, y);
    toolbar->setVisible(true);
}

void Canvas::hideToolBar()
{
    toolbar->setVisible(false);
}

void Canvas::refrashToolBar()
{
    btn_drawLine->setText(tr("Line"));
    btn_drawRect->setText(tr("Rect"));
    btn_drawEllipse->setText(tr("Ellipse"));
    btn_drawText->setText(tr("Text"));
    btn_cancel->setText(tr("Quit"));
    btn_saveClipboard->setText(tr("Copy"));
    btn_saveFile->setText(tr("Save"));
}

void Canvas::slt_drawLine()
{
    if (drawEditFlag != 1) {
        drawEditFlag = 1;
        btn_drawLine->setStyleSheet("background-color: rgb(146, 189, 108)");
        btn_drawRect->setStyleSheet("background-color: rgb(255, 255, 255)");
        btn_drawEllipse->setStyleSheet("background-color: rgb(255, 255, 255)");
        btn_drawText->setStyleSheet("background-color: rgb(255, 255, 255)");
        shapeToolBar->setVisible(true);
        textToolBar->setVisible(false);
        toolbar->adjustSize();
        showToolBar();
    } else {
        drawEditFlag = 0;
        btn_drawLine->setStyleSheet("background-color: rgb(255, 255, 255)");
        btn_drawRect->setStyleSheet("background-color: rgb(255, 255, 255)");
        btn_drawEllipse->setStyleSheet("background-color: rgb(255, 255, 255)");
        btn_drawText->setStyleSheet("background-color: rgb(255, 255, 255)");
        shapeToolBar->setVisible(false);
        textToolBar->setVisible(false);
        toolbar->adjustSize();
        showToolBar();
    }
}

void Canvas::slt_saveFile()
{
    shootScreen(shotArea);

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyyMMdd_hhmmsszzz");
    QString savefile = "ScreenShot_" + current_date + ".jpg";

    QString savepath = OperateSet::readSetting("Setting", "QuickSaveDir", ".").toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("save file"), savepath + "/" + savefile, tr("JPEG File (*jpg);;BMP File (*.bmp);;"
                                                                                                         "PNG File (*.png);;PPM File (*.ppm);;"
                                                                                                         "XBM File (*xbm);;XPM File (*.xpm)"));
    originalPixmap.save(fileName);
    QFileInfo fi = QFileInfo(fileName);
    QString file_path = fi.absolutePath();

    if (file_path != savepath) {
        OperateSet::writeSetting("Setting", "QuickSaveDir", file_path);
    }

    slt_cancel();
}

void Canvas::slt_saveClipboard()
{
    shootScreen(shotArea);
    clipboard->setPixmap(originalPixmap);
    slt_cancel();
}

void Canvas::slt_cancel()
{
    rectFlag = DrawStatus::waitDraw;
    this->close();
}

void Canvas::slt_changePenWidth(QString s)
{
    drawPen.setWidth(s.toInt());
}

void Canvas::slt_changePenColor()
{
    QColor color = QColorDialog::getColor(Qt::blue);
    if (color.isValid()) {
        // if color is valid, set select color
        QString colorStyle = QString("background-color: rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
        btn_colorSelect->setStyleSheet(colorStyle);
    }

    drawPen.setColor(color);
}

void Canvas::slt_changePenStyle(int index)
{
    switch (index) {
    case 0:
        drawPen.setStyle(Qt::SolidLine);
        break;
    case 1:
        drawPen.setStyle(Qt::DashLine);
        break;
    case 2:
        drawPen.setStyle(Qt::DotLine);
        break;
    case 3:
        drawPen.setStyle(Qt::DashDotLine);
        break;
    case 4:
        drawPen.setStyle(Qt::DashDotDotLine);
        break;
    default:
        drawPen.setStyle(Qt::SolidLine);
        break;
    }
}

void Canvas::slt_changeTextColor()
{
    QColor color = QColorDialog::getColor(Qt::blue);
    if (color.isValid()) {
        textColor = color;
        QString colorStyle = QString("background-color: rgb(%1, %2, %3)").arg(textColor.red()).arg(textColor.green()).arg(textColor.blue());
        btn_colorText->setStyleSheet(colorStyle);
    }
}

void Canvas::slt_changeTextFont()
{
    bool bRet = true;
    QFont font = QFontDialog::getFont(&bRet);
    if (bRet) {
        textfont = font;
        font.setPointSize(10);
        btn_FontText->setFont(font);
        btn_FontText->setText("Font" + QString::number(textfont.pointSize()));
    }
}

RectPaint Canvas::getRectF(QPointF p1, QPointF p2)
{
    float x1, y1, x2, y2;
    if (p1.x() < p2.x()) {
        x1 = p1.x();
        x2 = p2.x();
    } else {
        x1 = p2.x();
        x2 = p1.x();
    }

    if (p1.y() < p2.y()) {
        y1 = p1.y();
        y2 = p2.y();
    } else {
        y1 = p2.y();
        y2 = p1.y();
    }

    QPointF ps(x1, y1);
    QPointF pe(x2, y2);

    RectPaint rect(ps, pe);
    return rect;
}

void Canvas::changeLanguage(QString lan)
{
    QTransform translator;
    bool  sta = false;

    if (lan == "zh_cn") {

    }
}










