#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    object = 0;
    xRot = yRot = zRot = 0;
    scale = 1.0;

    Molecule mol("molecules/cyanocobalamin.mol");
    setMolecule(mol);
}

void GLWidget::setXRot(int value)
{
    xRot = value;
    update();
}

void GLWidget::setYRot(int value)
{
    yRot = value;
    update();
}

void GLWidget::setZRot(int value)
{
    zRot = value;
    update();
}

void GLWidget::setScale(int value)
{
    scale = value / 100.0;
    update();
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(object, 1);
}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::black);
    object = makeObject();
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslated(0, 0, -10.0);
    glScaled(scale, scale, scale);
    glRotated(xRot, 1.0, 0.0, 0.0);
    glRotated(yRot, 0.0, 1.0, 0.0);
    glRotated(zRot, 0.0, 0.0, 1.0);
    glTranslated(-molecule.massCenterX, -molecule.massCenterY, -molecule.massCenterZ);
    glCallList(object);
}

void GLWidget::setMolecule(const Molecule &molecule)
{
    this->molecule = molecule;
    if (object)
    {
        glDeleteLists(object, 1);
        object = makeObject();
    }
    update();
}

void GLWidget::updateStuff()
{

}

GLuint GLWidget::makeObject()
{
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    glColor3f(1,1,1);
    glBegin(GL_LINES);
    for (QList<Bond>::const_iterator it = molecule.bonds.begin(); it != molecule.bonds.end(); ++it)
    {
        glVertex3f(it->a->x, it->a->y, it->a->z);
        glVertex3f(it->b->x, it->b->y, it->b->z);
    }
    glEnd();

    glEndList();
    return list;
}

void GLWidget::resizeGL(int width, int height)
{
    double ratio = 1.0 * width / height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ratio, 0.01, 1000);
    glMatrixMode(GL_MODELVIEW);
}
