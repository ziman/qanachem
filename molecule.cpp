#include "molecule.h"
#include <QFile>
#include <iostream>
#include <QStringList>
#include <QDebug>

Molecule::Molecule()
{
    massCenterX = massCenterY = massCenterZ = 0;
}

QStringList readFields(QFile & f)
{
    QStringList result = QString(f.readLine()).split(' ', QString::SkipEmptyParts);

    int r0 = result[0].toInt();
    if (r0 > 1000)
    {
        result[0] = QString::number(r0 / 1000);
        result.insert(1, QString::number(r0 % 1000));
    }

    return result;
}

static BondType int2bt(int type)
{
    switch(type)
    {
    case 1: return btSingle;
    case 2: return btDouble;
    case 3: return btTriple;
    case 4: return btAromatic;
    case 6: return btSingle; // or Aromatic
    case 7: return btDouble; // or Aromatic
    default: return btNone;
    }
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
    qDebug() << atomCnt << " " << bondCnt << endl;

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
        // qDebug() << i << ": " << fields << endl;
        bond.a = &atoms[fields[0].toInt() - 1];
        bond.b = &atoms[fields[1].toInt() - 1];
        bond.type = int2bt(fields[2].toInt());

        bonds.append(bond);
    }
}
