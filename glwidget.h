#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QMap>

#include "molecule.h"

enum RenderMode
{
    rmSmall,
    rmLarge,
    rmGiant
};

struct Element
{
    double radius; // in angstroms
    QColor color;

    Element(double radius, QColor color);
};

class GLWidget : public QGLWidget
{
Q_OBJECT

    GLuint object;
    int xRot, yRot, zRot;
    int eyeDistance;
    double atomSizeScale;
    double scale;
    double massCenterX, massCenterY;
    Molecule molecule;
    bool anaglyph;
    bool paintAtoms;
    QMap<QString, Element> elements;
    RenderMode renderMode;

    void renderImage();

    void smallObject(RenderMode renderMode);
    void largeObject();
    void giantObject();
    void recacheObject();
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
     void setXRot(int value);
     void setYRot(int value);
     void setZRot(int value);

     // per cent
     void setScale(int value);
     void setAtomSizeScale(int value);
     void setAnaglyph(bool anaglyph);
     void setMoleculeSize(int size);

     // default = 100
     void setEyeDistance(int value);
};

#endif // GLWIDGET_H
