#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QMap>

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
    bool anaglyph;
    bool paintAtoms;

    QMap<QString, QColor> elements;

    void renderImage();
public:
    explicit GLWidget(QWidget *parent = 0);
    virtual ~GLWidget();

    void setMolecule(const Molecule & molecule);

protected:
     virtual void initializeGL();
     virtual void paintGL();
     virtual void resizeGL(int width, int height);

signals:

public slots:
     virtual void updateStuff();

     void setXRot(int value);
     void setYRot(int value);
     void setZRot(int value);

     // per cent
     void setScale(int value);
     void setAnaglyph(bool anaglyph);
     void setPaintAtoms(bool paintAtoms);
};

#endif // GLWIDGET_H
