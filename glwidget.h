#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>

#include "molecule.h"

class GLWidget : public QGLWidget
{
Q_OBJECT

    GLuint object;
    GLuint makeObject();

    int xRot, yRot, zRot;
    double scale;
    double massCenterX, massCenterY;
    Molecule molecule;

    void renderImage(bool isLeft);
public:
    explicit GLWidget(QWidget *parent = 0);
    virtual ~GLWidget();

    void setMolecule(const Molecule & molecule);

protected:
     virtual void initializeGL();
     virtual void paintGL();
     virtual void resizeGL(int width, int height);
     /*
     virtual void mousePressEvent(QMouseEvent *event);
     virtual void mouseMoveEvent(QMouseEvent *event);
     */

signals:

public slots:
     virtual void updateStuff();

     void setXRot(int value);
     void setYRot(int value);
     void setZRot(int value);

     // per cent
     void setScale(int value);
};

#endif // GLWIDGET_H
