#include "glwidget.h"
#include "GL/glut.h"
#include "GL/glu.h"
#include <QRgb>

static const double PI = 3.1415926536;

struct ElmRec { QString name; Element elm; };

ElmRec elemRec[] = {
    { "C",  Element(1.70, Qt::black) },
    { "H",  Element(1.20, Qt::white) },
    { "O",  Element(1.52, Qt::red, Qt::darkCyan) },
    { "N",  Element(1.55, Qt::blue) },
    { "P",  Element(1.80, Qt::magenta) },
    { "Co", Element(2.52, Qt::yellow) },
    { "S",  Element(1.80, Qt::yellow) },
    { "F",  Element(1.47, Qt::darkGreen) },
    { "Cl", Element(1.75, Qt::darkGreen) },
    { "Br", Element(1.85, Qt::red) },
    { "I",  Element(1.98, Qt::magenta) },
    { QString::null, Element(0, Qt::black) },
};

Element::Element(double radius, QColor color, QColor anaColor)
{
    this->radius = radius;
    this->color = color;
    this->anaColor = anaColor.isValid() ? anaColor : color;
}

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    object = 0;
    xRot = yRot = zRot = 0;
    panX = panY = panZ = 0;
    scale = 1;
    atomSizeScale = 0.25;
    anaglyph = true;
    eyeDistance = 100;
    renderMode = rmSmall;

    Molecule mol("molecules/thujone.mol");
    setMolecule(mol);

    for (ElmRec * p = elemRec; !p->name.isNull(); ++p)
        elements.insert(p->name, p->elm);
}

void GLWidget::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
        panMousePos = e->globalPos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
        panMousePos = QPoint();
}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{
    if (!panMousePos.isNull())
    {
        double scale = 0.01;
        panX -= scale * (e->globalPos().x() - panMousePos.x());
        panY += scale * (e->globalPos().y() - panMousePos.y());

        panMousePos = e->globalPos();
        update();
    }
}

void GLWidget::wheelEvent(QWheelEvent * e)
{
    scale *= (1 + 0.0005 * e->delta());
    update();
}

const QMap<QString, Element> & GLWidget::elementMap()
{
    return elements;
}

const Molecule & GLWidget::getMolecule()
{
    return molecule;
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

void GLWidget::setAtomSizeScale(int value)
{
    atomSizeScale = value / 100.0;
    recacheObject();
    update();
}

void GLWidget::setAnaglyph(bool anaglyph)
{
    this->anaglyph = anaglyph;
    recacheObject();
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
        glTranslated(-panX, -panY, -panZ);
        glRotated(convRot, 0.0, 1.0, 0.0);
        glTranslated(xShift, 0, -zShift);
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        renderImage();

        glClear(GL_DEPTH_BUFFER_BIT);

        // cyan image
        glLoadIdentity();
        glTranslated(-panX, -panY, -panZ);
        glRotated(-convRot, 0.0, 1.0, 0.0);
        glTranslated(-xShift, 0, -7.0);
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        renderImage();

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    else
    {
        glLoadIdentity();
        glTranslated(-panX, -panY, -panZ);
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

static void drawLineBond(BondType type)
{
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    switch (type)
    {
    case btSingle:
        glVertex3f(0,0,0); glVertex3f(1,0,0);
        break;

    case btDouble:
        glVertex3f(0,-0.0875,0); glVertex3f(1,-0.0875,0);
        glVertex3f(0,0.0875,0); glVertex3f(1,0.0875,0);
        break;

    case btTriple:
        glVertex3f(0,-0.105,0); glVertex3f(1,-0.105,0);
        glVertex3f(0,0,0); glVertex3f(1,0,0);
        glVertex3f(0,0.105,0); glVertex3f(1,0.105,0);
        break;

    default:
        glVertex3f(0,0,0); glVertex3f(1,0,0);
        break;
    }
    glEnd();
}

static void drawBond(BondType type, RenderMode renderMode)
{
    static GLUquadric *quad = gluNewQuadric();
    double baseRadius = 0.01;
    double bondDistanceC = 1.75;

    int slices = 12;
    int stacks = 4;
    switch (renderMode)
    {
    case rmSmall:
        slices = 12; stacks = 4; break;
    case rmLarge:
        slices = 8; stacks = 2;
        baseRadius *= 0.5;
        bondDistanceC *= 2;
        break;
    case rmGiant:
        drawLineBond(type);
        return;
    }

    double singleBondRadius = 7 * baseRadius;
    double doubleBondRadius = 5 * baseRadius;
    double tripleBondRadius = 4 * baseRadius;

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

void GLWidget::smallObject(RenderMode renderMode)
{
    // draw bonds
    foreach (Bond bond, molecule.bonds)
    {
        glPushMatrix();
        stretchBond(*bond.a, *bond.b);
        drawBond(bond.type, renderMode);
        glPopMatrix();
    }

    // draw atoms
    foreach (Atom atom, molecule.atoms)
    {
        glPushMatrix();

        QMap<QString,Element>::const_iterator it = elements.constFind(atom.element);
        if (it != elements.end())
        {
            float normalMat[4] = {it->color.redF(), it->color.greenF(), it->color.blueF(), 1.0};
            float anaglyphMat[4] = {it->anaColor.redF(), it->anaColor.greenF(), it->anaColor.blueF(), 1.0};

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, anaglyph ? anaglyphMat : normalMat);
            glTranslated(atom.x, atom.y, atom.z);
            switch (renderMode)
            {
            case rmSmall:
                glutSolidSphere(it->radius * atomSizeScale, 24, 12);
                break;
            case rmLarge:
                glutSolidSphere(it->radius * atomSizeScale * 0.5, 8, 8);
                break;
            case rmGiant:
                glutSolidSphere(it->radius * atomSizeScale * 0.5, 4, 4);
                break;
            }
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
    smallObject(renderMode);
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
