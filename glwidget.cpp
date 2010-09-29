#include "glwidget.h"
#include "GL/glut.h"
#include <QRgb>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    object = 0;
    xRot = yRot = zRot = 0;
    scale = 1;
    anaglyph = true;

    Molecule mol("molecules/thujone.mol");
    setMolecule(mol);

    elements.insert("C", Qt::black);
    elements.insert("H", Qt::white);
    elements.insert("O", Qt::red);
    elements.insert("N", Qt::green);
    elements.insert("P", Qt::blue);
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

void GLWidget::setAnaglyph(bool anaglyph)
{
    this->anaglyph = anaglyph;
    update();
}

void GLWidget::setPaintAtoms(bool paintAtoms)
{
    this->paintAtoms = paintAtoms;
    update();
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(object, 1);
}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::gray);

    object = makeObject();
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    const float light_ambient[4]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    const float light_diffuse[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const float light_specular[4] = { 0.4f, 0.4f, 0.4f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_LIGHTING);    /* enable lighting */
    glEnable(GL_LIGHT0);        /* enable light 0 */
}

void GLWidget::renderImage(double xShift)
{
    glLoadIdentity();
    glTranslated(xShift, 0, -10.0);
    glScaled(scale, scale, scale);
    glRotated(zRot, 0.0, 0.0, 1.0);
    glRotated(yRot, 0.0, 1.0, 0.0);
    glRotated(xRot, 1.0, 0.0, 0.0);
    glTranslated(-molecule.massCenterX, -molecule.massCenterY, -molecule.massCenterZ);
    glCallList(object);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (anaglyph)
    {
        const double xShift = 0.05;

        // red image
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        renderImage(xShift);

        glClear(GL_DEPTH_BUFFER_BIT);

        // cyan image
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        renderImage(-xShift);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    else
    {
        renderImage(0.0);
    }
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

    // draw bonds
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    foreach (Bond bond, molecule.bonds)
    {
        glVertex3f(bond.a->x, bond.a->y, bond.a->z);
        glVertex3f(bond.b->x, bond.b->y, bond.b->z);
    }
    glEnd();

    // draw atoms
    glColor3f(1,1,1);
    foreach (Atom atom, molecule.atoms)
    {
        glPushMatrix();

        QMap<QString,QColor>::const_iterator it = elements.constFind(atom.element);
        if (it != elements.end())
        {
            float mat[4] = {it->redF(), it->greenF(), it->blueF(), 1.0};
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
            glTranslated(atom.x, atom.y, atom.z);
            glutSolidSphere(0.3, 12, 12);
        }
        else
        {
            const double fontScale = 0.005;
            const double shift = 0.2;

            glTranslated(atom.x - shift, atom.y - shift, atom.z);
            glScaled(fontScale, fontScale, fontScale);
            for (const char *p = atom.element.toLocal8Bit().data(); *p; p++)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
        }
        glPopMatrix();
    }

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
