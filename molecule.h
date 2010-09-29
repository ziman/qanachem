#ifndef MOLECULE_H
#define MOLECULE_H

#include <QString>
#include <QList>

struct Atom
{
    double x, y, z;
};

struct Bond
{
    Atom *a, *b;
    int type;
};

struct Molecule
{
    QString name;
    QString comment;
    QList<Atom> atoms;
    QList<Bond> bonds;

    Molecule();
    Molecule(const QString & fname);
};

#endif // MOLECULE_H
