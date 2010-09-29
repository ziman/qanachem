#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    object = 0;
    xRot = yRot = zRot = 0;
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
    glTranslated(0.0, 0.0, -10.0);
    glRotated(xRot, 1.0, 0.0, 0.0);
    glRotated(yRot, 0.0, 1.0, 0.0);
    glRotated(zRot, 0.0, 0.0, 1.0);
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
    int side = qMin(width, height);
    double ratio = 1.0 * width / height;
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
    // glOrtho(-100, +100, +100, -100, 0.0, 1000.0);
    gluPerspective(45, ratio, 0.01, 1000);
    glMatrixMode(GL_MODELVIEW);
}
