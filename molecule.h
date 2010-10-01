#ifndef MOLECULE_H
#define MOLECULE_H

#include <QString>
#include <QList>

enum BondType
{
    btNone,
    btSingle,
    btDouble,
    btTriple,
    btAromatic
};

struct Atom
{
    double x, y, z;
    QString element;
};

struct Bond
{
    Atom *a, *b;
    BondType type;
};

struct Molecule
{
    QString name;
    QString comment;
    QList<Atom> atoms;
    QList<Bond> bonds;

    double massCenterX, massCenterY, massCenterZ;

    Molecule();
    Molecule(const QString & fname);
};

#endif // MOLECULE_H
