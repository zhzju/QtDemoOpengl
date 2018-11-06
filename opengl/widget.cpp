#include "widget.h"
#include "ui_widget.h"
#include <GL/glu.h>
#include <QDebug>
#define zoomRatioMax 50
#define zoomRationMin 1
Widget::Widget(QGLWidget *parent):
    QGLWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initializeGL();
    width = 0;
    height = 0;
    zoomratio = 1.0;
    lastzoomratio = 1.0;
    deviationOriginX = 0;
    deviationOriginY = 0;
    imageStartX = 0;
    imageStartY = 0;
    DragstartPos = QPointF(0.0f,0.0f);
    counterpoint[0]=QPointF(-1.0f,1.0f);
    counterpoint[1]=QPointF(1.0f,1.0f);
    counterpoint[2]=QPointF(1.0f,-1.0f);
    counterpoint[3]=QPointF(-1.0f,-1.0f);
    DragLastPosX = 0;
    DragLastPosY = 0;
    MoveFlag = 0;
    updateGL();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initializeGL()
{
    texture = readImage("1.jpg");
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}
void Widget::wheelEvent(QWheelEvent *e){
    MouseCurrentX = e->pos().x();
    MouseCurrentY = e->pos().y();
    if(MouseCurrentX <0 || MouseCurrentX > width || MouseCurrentY < 0 || MouseCurrentY > height)
        return;
    float deltaDegree = e->delta()*1.0/720;
    if (zoomratio+deltaDegree > zoomRatioMax ||
            zoomratio+deltaDegree < zoomRationMin-0.1)
        return;
    zoomratio += deltaDegree;
    float MousecalX,MousecalY;
    MousecalX = (-deviationOriginX + MouseCurrentX);
    MousecalY = (-deviationOriginY + MouseCurrentY);
    imageStartX = MouseCurrentX-MousecalX*zoomratio/lastzoomratio;
    imageStartY = MouseCurrentY-MousecalY*zoomratio/lastzoomratio;
    QPointF afterpoint;
    afterpoint = getridofblack(imageStartX,imageStartY,height* zoomratio,width* zoomratio);
    imageStartX = afterpoint.x();
    imageStartY = afterpoint.y();
    if(imageStartX<1&&imageStartX>-1) imageStartX = 0;
    if(imageStartY<1&&imageStartY>-1) imageStartY = 0;
//    qDebug()<<imageStartX<<imageStartY;
    counterpoint[0].setX(imageStartX);
    counterpoint[0].setY(imageStartY);
    counterpoint[1].setX(counterpoint[0].x()+width* zoomratio);
    counterpoint[1].setY(counterpoint[0].y());
    counterpoint[2].setX(counterpoint[1].x());
    counterpoint[2].setY(counterpoint[1].y()+height* zoomratio);
    counterpoint[3].setX(counterpoint[2].x()-width* zoomratio);
    counterpoint[3].setY(counterpoint[2].y());
    deviationOriginX = imageStartX;
    deviationOriginY = imageStartY;
    DragLastPosX = imageStartX;
    DragLastPosY = imageStartY;
    lastzoomratio = zoomratio;
    QPointF topleft = QPoint(width/2.0,height/2.0);
    for(int i = 0 ; i < 4 ; i++){
    counterpoint[i].setX(counterpoint[i].x() -topleft.x());
    counterpoint[i].setY(-counterpoint[i].y() +topleft.y());
    counterpoint[i].setX(counterpoint[i].x()/(width/2.0));
    counterpoint[i].setY(counterpoint[i].y()/(height/2.0));
    }//坐标系变换，原点变换至屏幕中央
    updateGL();
}
void Widget::paintGL()
{
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glBindTexture(GL_TEXTURE_2D, texture);
     glEnable(GL_TEXTURE_2D);    //启用2D纹理映射,将纹理图片贴到四边形上，x，y，z轴坐标
     glLoadIdentity();
     glBegin(GL_QUADS);
     glTexCoord2f(0.0f, 0.0f);   //纹理坐标配置，左下角
     glVertex3f(counterpoint[3].x(), counterpoint[3].y(), 0.0f);
     glTexCoord2f(1.0f, 0.0f);   //纹理坐标配置，右下角
     glVertex3f(counterpoint[2].x(), counterpoint[2].y(), 0.0f);
     glTexCoord2f(1.0f, 1.0f);   //纹理坐标配置，右上角
     glVertex3f(counterpoint[1].x(), counterpoint[1].y(), 0.0f);
     glTexCoord2f(0.0f, 1.0f);   //纹理坐标配置，左上角
     glVertex3f(counterpoint[0].x(), counterpoint[0].y(), 0.0f);
     glEnd();
}
void Widget::resizeGL(int w, int h){
    if(w>0 && h>0)
    {glViewport(0,0,(GLint)w,(GLint)h);
    width = w;
    height = h;}
}

GLuint Widget::readImage(QString filename){
    GLuint tex_id;
    GLint alignment;
    QImage tex,buf;
    buf.load(filename);
    tex=QGLWidget::convertToGLFormat(buf);
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D,tex_id);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);   //配置纹理信息

    glGetIntegerv (GL_UNPACK_ALIGNMENT, &alignment);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );
    glPixelStorei (GL_UNPACK_ALIGNMENT, alignment);

    return tex_id;
}
QPointF Widget::getridofblack(float PosX , float PosY, float heightz, float widthz)
{
    int imagex1 = 0;
    int imagey1 = 0;
    int imagex2 = width;
    int imagey2 = height;
    int X,Y;

    if(PosX > imagex1)
    {
        if(PosY > imagey1)
        Y = imagey1;
        else if(PosY + heightz < imagey2)
        Y = imagey2 - heightz;
        else return QPointF(imagex1,PosY);
        return QPointF(imagex1,Y);
    }
    if(PosX + widthz < imagex2)
    {
        if(PosY > imagey1)
        Y = imagey1;
        else if(PosY + heightz < imagey2)
        Y = imagey2 - heightz;
        else return QPointF(imagex2 - widthz,PosY);
        return QPointF(imagex2 - widthz,Y);
    }
    if(PosY > imagey1)
    {
        if(PosX > imagex1)
        X = imagex1;
        else if(PosX + widthz < imagex2)
        X = imagex2 - widthz;
        else return QPointF(PosX,imagey1);
        return QPointF(X,imagey1);
    }
    if(PosY + heightz < imagey2)
    {
        if(PosX > imagex1)
        X = imagex1;
        else if(PosX + widthz < imagex2)
        X = imagex2 - widthz;
        else return QPointF(PosX,imagey2 - heightz);
        return QPointF(X,imagey2 - heightz);
    }
    return QPointF(PosX,PosY);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    QPoint invalidpos = event->pos();
    if(invalidpos.x()<0 || invalidpos.y()<0 || invalidpos.x()>width || invalidpos.y()>height)
    {
        return;
    }
    if(event->button() == Qt::LeftButton)
        DragstartPos = event->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint invalidpos = event->pos();
    if(invalidpos.x()<0 || invalidpos.y()<0 || invalidpos.x()>width || invalidpos.y()>height)
    {
        MoveFlag = 0;
        return;
    }
    if ((event->pos() - DragstartPos).manhattanLength() < QApplication::startDragDistance())
    {MoveFlag = 0;return;}
    if(zoomratio <= 1) {MoveFlag = 0;return;}
    int imagex1 = 0;
    int imagey1 = 0;
    int imagex2 = width;
    int imagey2 = height;
    QPointF offset;
    offset =  event->pos() - DragstartPos;
    if(DragLastPosX+offset.x() > imagex1)
    {
       if(DragLastPosY+offset.y() > imagey1)
           DragLastPosY = imagey1;
       else if(DragLastPosY+offset.y() + height* zoomratio < imagey2)
           DragLastPosY = imagey2 - height* zoomratio;
       else
       {
           changezuobiao(imagex1,DragLastPosY+offset.y());
           updateGL();
           MoveFlag = 1;
           return;
       }
       DragLastPosX = imagex1;
       MoveFlag = 0;
       deviationOriginX = DragLastPosX;
       deviationOriginY = DragLastPosY;
       return;
    }
    if(DragLastPosX+offset.x()+width* zoomratio < imagex2)
    {
       if(DragLastPosY+offset.y() > imagey1)
           DragLastPosY = imagey1;
       else if(DragLastPosY+offset.y() + height* zoomratio < imagey2)
           DragLastPosY = imagey2 - height* zoomratio;
       else
       {
           changezuobiao(imagex2-width* zoomratio,DragLastPosY+offset.y());
           updateGL();
           MoveFlag = 2;
           return;
       }
       DragLastPosX = imagex2-width* zoomratio;
       MoveFlag = 0;
       deviationOriginX = DragLastPosX;
       deviationOriginY = DragLastPosY;
       return;
    }
    if(DragLastPosY+offset.y() > imagey1)
    {
        if(DragLastPosX+offset.x() > imagex1)
          DragLastPosX = imagex1;
        else if(DragLastPosX+offset.x() + width* zoomratio <imagex2)
          DragLastPosX = imagex2 - width* zoomratio;
        else
        {
           changezuobiao(DragLastPosX+offset.x(),imagey1);
           updateGL();
           MoveFlag = 3;
           return;
        }
        DragLastPosY = imagey1;
        MoveFlag = 0;
        deviationOriginX = DragLastPosX;
        deviationOriginY = DragLastPosY;
        return;
    }
    if(DragLastPosY+offset.y() + height* zoomratio < imagey2)
    {
        if(DragLastPosX+offset.x() > imagex1)
          DragLastPosX = imagex1;
        else if(DragLastPosX+offset.x() + width* zoomratio < imagex2)
          DragLastPosX = imagex2 - width* zoomratio;
        else
        {
           changezuobiao(DragLastPosX+offset.x(),imagey2 - height* zoomratio);
           updateGL();
           MoveFlag = 4;
           return;
        }
        DragLastPosY = imagey2 - height* zoomratio;
        MoveFlag = 0;
        deviationOriginX = DragLastPosX;
        deviationOriginY = DragLastPosY;
        return;
    }
    changezuobiao(DragLastPosX+offset.x(),DragLastPosY+offset.y());
    updateGL();
    MoveFlag = 5;
    return;
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint invalidpos = event->pos();
    if(invalidpos.x()<0 || invalidpos.y()<0 || invalidpos.x()>width || invalidpos.y()>height)
    {
        MoveFlag = 0;
        return;
    }
    if(MoveFlag == 0) return;
    int imagex1 = 0;
    int imagey1 = 0;
    int imagex2 = width;
    int imagey2 = height;
    QPointF offset;
    offset = event->pos() - DragstartPos ;
    if(MoveFlag == 1){
        DragLastPosX = imagex1;
        DragLastPosY = DragLastPosY+offset.y();}

    if(MoveFlag == 2){
        DragLastPosX = imagex2 - width* zoomratio;
        DragLastPosY = DragLastPosY+offset.y();}

    if(MoveFlag == 3){
        DragLastPosX = DragLastPosX+offset.x();
        DragLastPosY = imagey1;}

    if(MoveFlag == 4){
        DragLastPosX = DragLastPosX+offset.x();
        DragLastPosY = imagey2 - height* zoomratio;}

    if(MoveFlag == 5){
        DragLastPosX = DragLastPosX+offset.x();
        DragLastPosY = DragLastPosY+offset.y();}

    deviationOriginX = DragLastPosX;
    deviationOriginY = DragLastPosY;
}


void Widget::changezuobiao(float X,float Y){
    counterpoint[0].setX(X);
    counterpoint[0].setY(Y);
    counterpoint[1].setX(counterpoint[0].x()+width* zoomratio);
    counterpoint[1].setY(counterpoint[0].y());
    counterpoint[2].setX(counterpoint[1].x());
    counterpoint[2].setY(counterpoint[1].y()+height* zoomratio);
    counterpoint[3].setX(counterpoint[2].x()-width* zoomratio);
    counterpoint[3].setY(counterpoint[2].y());
    QPointF topleft = QPoint(width/2.0,height/2.0);
    for(int i = 0 ; i < 4 ; i++){
    counterpoint[i].setX(counterpoint[i].x() -topleft.x());
    counterpoint[i].setY(-counterpoint[i].y() +topleft.y());
    counterpoint[i].setX(counterpoint[i].x()/(width/2.0));
    counterpoint[i].setY(counterpoint[i].y()/(height/2.0));
    }//坐标系变换，原点变换至屏幕中央
}

