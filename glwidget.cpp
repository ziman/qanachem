#include "glwidget.h"
#include "GL/glut.h"
#include "GL/glu.h"
#include <QRgb>

static const double PI = 3.1415926536;

Element::Element(double radius, QColor color)
{
    this->radius = radius;
    this->color = color;
}

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    object = 0;
    xRot = yRot = zRot = 0;
    scale = 1;
    anaglyph = true;
    eyeDistance = 100;
    renderMode = rmSmall;

    Molecule mol("molecules/thujone.mol");
    setMolecule(mol);

    elements.insert("C", Element(1.70, Qt::black));
    elements.insert("H", Element(1.20, Qt::white));
    elements.insert("O", Element(1.52, Qt::blue));
    elements.insert("N", Element(1.55, Qt::darkGreen));
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

void GLWidget::setMoleculeSize(int size)
{
    switch (size)
    {
    case 0:
        renderMode = rmSmall; break;
    case 1:
        renderMode = rmLarge; break;
    case 2: default:
        renderMode = rmGiant; break;
    }

    recacheObject();
    update();
}

void GLWidget::setEyeDistance(int eyeDistance)
{
    this->eyeDistance = eyeDistance;
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
    object = 0;

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

    recacheObject();
}

void GLWidget::renderImage()
{
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

    const double zShift = 7;
    if (anaglyph)
    {
        const double xShift = 0.001 * eyeDistance;
        const double convRot = 180 * atan(xShift/zShift) / 3.1415926536;

        // red image
        glLoadIdentity();
        glRotated(convRot, 0.0, 1.0, 0.0);
        glTranslated(xShift, 0, -zShift);
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        renderImage();

        glClear(GL_DEPTH_BUFFER_BIT);

        // cyan image
        glLoadIdentity();
        glRotated(-convRot, 0.0, 1.0, 0.0);
        glTranslated(-xShift, 0, -7.0);
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        renderImage();

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    else
    {
        glLoadIdentity();
        glTranslated(0, 0, -zShift);
        renderImage();
    }
}

void GLWidget::setMolecule(const Molecule &molecule)
{
    this->molecule = molecule;
    recacheObject();
    update();
}

static void drawBond(BondType type)
{
    static GLUquadric *quad = gluNewQuadric();
    static const double singleBondRadius = 0.07;
    static const double doubleBondRadius = 0.05;
    static const double tripleBondRadius = 0.04;
    static const double bondDistanceC = 1.75;

    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 1, 0);

    switch (type)
    {
    case btSingle:
        gluCylinder(quad, singleBondRadius, singleBondRadius, 1, 12, 4);
        break;

    case btDouble:
        glTranslatef(-bondDistanceC*doubleBondRadius,0,0);
        gluCylinder(quad, doubleBondRadius, doubleBondRadius, 1, 12, 4);
        glTranslatef(2*bondDistanceC*doubleBondRadius,0,0);
        gluCylinder(quad, doubleBondRadius, doubleBondRadius, 1, 12, 4);
        break;

    case btTriple:
        gluCylinder(quad, tripleBondRadius, tripleBondRadius, 1, 12, 4);
        glTranslatef(-1.5*bondDistanceC*tripleBondRadius,0,0);
        gluCylinder(quad, tripleBondRadius, tripleBondRadius, 1, 12, 4);
        glTranslatef(3*bondDistanceC*tripleBondRadius,0,0);
        gluCylinder(quad, tripleBondRadius, tripleBondRadius, 1, 12, 4);
        break;

    default:
        gluCylinder(quad, singleBondRadius, singleBondRadius, 1, 12, 4);
        break;
    }
}

static inline double sqr(double x)
{
    return x*x;
}

static void stretchBond(const Atom &a, const Atom &b)
{
    double dXYZ = sqrt(sqr(a.x - b.x) + sqr(a.y - b.y) + sqr(a.z - b.z));
    double dXZ = sqrt(sqr(a.x - b.x) + sqr(a.z - b.z));

    if (dXYZ < 1.0e-8)
    {
        glScalef(0,0,0);
        return;
    }

    double phi = 0;
    double alpha = (b.y > a.y) ? PI/2 : -PI/2;

    if (dXZ > 1.0e-8)
    {
        double dX = b.x - a.x;
        double dY = b.y - a.y;
        double dZ = b.z - a.z;

        phi = asin(dZ/dXZ);
        if (dX < 0) phi = PI - phi;

        alpha = asin(dY/dXYZ);
    }

    glTranslatef(a.x, a.y, a.z);
    glRotatef(phi*180/PI, 0, -1, 0);
    glRotatef(alpha*180/PI, 0, 0, 1);
    glScalef(dXYZ, 1, 1);
}

void GLWidget::smallObject()
{
    // draw bonds
    foreach (Bond bond, molecule.bonds)
    {
        glPushMatrix();
        stretchBond(*bond.a, *bond.b);
        drawBond(bond.type);
        glPopMatrix();
    }

    // draw atoms
    foreach (Atom atom, molecule.atoms)
    {
        glPushMatrix();

        QMap<QString,Element>::const_iterator it = elements.constFind(atom.element);
        if (it != elements.end())
        {
            float mat[4] = {it->color.redF(), it->color.greenF(), it->color.blueF(), 1.0};
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
            glTranslated(atom.x, atom.y, atom.z);
            // glScaled(0.3, 0.3, 0.3);
            glutSolidSphere(it->radius, 24, 12);
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
}

void GLWidget::largeObject()
{
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

        QMap<QString,Element>::const_iterator it = elements.constFind(atom.element);
        if (it != elements.end())
        {
            float mat[4] = {it->color.redF(), it->color.greenF(), it->color.blueF(), 1.0};
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
            glTranslated(atom.x, atom.y, atom.z);
            glScaled(0.3, 0.3, 0.3);
            glutSolidSphere(it->radius, 24, 12);
        }
        else
        {
            const double fontScale = 0.005;
            const double shift = 0.2;

            this->renderText(atom.x, atom.y, atom.z, atom.element, QFont("Sans", 1));

            glTranslated(atom.x - shift, atom.y - shift, atom.z);
            glScaled(fontScale, fontScale, fontScale);
            for (const char *p = atom.element.toLocal8Bit().data(); *p; p++)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
        }
        glPopMatrix();
    }
}

void GLWidget::giantObject()
{
    // draw bonds
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    foreach (Bond bond, molecule.bonds)
    {
        glVertex3f(bond.a->x, bond.a->y, bond.a->z);
        glVertex3f(bond.b->x, bond.b->y, bond.b->z);
    }
    glEnd();
}

void GLWidget::recacheObject()
{
    if (object)
        glDeleteLists(object, 1);

    object = glGenLists(1);
    glNewList(object, GL_COMPILE);

    switch (renderMode)
    {
    case rmSmall:
        smallObject(); break;
    case rmLarge:
        largeObject(); break;
    case rmGiant:
        giantObject(); break;
    }

    glEndList();
}

void GLWidget::resizeGL(int width, int height)
{
    double ratio = 1.0 * width / height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, ratio, 0.01, 1000);
    glMatrixMode(GL_MODELVIEW);
}
