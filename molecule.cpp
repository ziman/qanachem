#include "molecule.h"
#include <QFile>
#include <iostream>
#include <QStringList>

Molecule::Molecule()
{
    massCenterX = massCenterY = massCenterZ = 0;
}

QStringList readFields(QFile & f)
{
    return QString(f.readLine()).split(' ', QString::SkipEmptyParts);
}

Molecule::Molecule(const QString &fname)
{
    QFile f(fname);
    f.open(QIODevice::ReadOnly);
    name = f.readLine();
    comment = f.readLine();
    f.readLine();

    QStringList info = readFields(f);
    int atomCnt = info[0].toInt();
    int bondCnt = info[1].toInt();

    massCenterX = massCenterY = massCenterZ = 0;
    for (int i = 0; i < atomCnt; ++i)
    {
        QStringList fields = readFields(f);

        Atom atom;
        atom.x = fields[0].toDouble();
        atom.y = fields[1].toDouble();
        atom.z = fields[2].toDouble();
        atom.element = fields[3];

        atoms.append(atom);

        massCenterX += atom.x;
        massCenterY += atom.y;
        massCenterZ += atom.z;
    }
    massCenterX /= atomCnt;
    massCenterY /= atomCnt;
    massCenterZ /= atomCnt;

    for (int i = 0; i < bondCnt; ++i)
    {
        QStringList fields = readFields(f);

        Bond bond;
        bond.a = &atoms[fields[0].toInt() - 1];
        bond.b = &atoms[fields[1].toInt() - 1];
        bond.type = fields[2].toInt();

        bonds.append(bond);
    }
}
