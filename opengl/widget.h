#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGLWidget>
#include <QtOpenGL>
namespace Ui {
class Widget;
}

class Widget : public QGLWidget
{
    Q_OBJECT

public:
    explicit Widget(QGLWidget *parent = 0);
    ~Widget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::Widget *ui;
    GLuint readImage(QString filename);
    GLuint texture;
    int width;
    int height;
    int MouseCurrentX;
    int MouseCurrentY;
    float deviationOriginX;
    float deviationOriginY;
    float zoomratio;
    float lastzoomratio;
    float imageStartX,imageStartY;
    QPointF getridofblack(float PosX , float PosY, float heightz, float widthz);
    QPointF counterpoint[4];
    QPointF DragstartPos;
    int MoveFlag;
    float DragLastPosX;
    float DragLastPosY;
    void changezuobiao(float X,float Y);
};

#endif // WIDGET_H
