#include "qgloble.h"

QString fromstdstring(string str)
{
    return QString::fromStdString(str);
}


int qtoint(QString qstr)
{
    return qstr.toInt();
}

float qtofloat(QString qstr)
{
    return qstr.toFloat();
}

double qtodouble(QString qstr)
{
    return qstr.toDouble();
}

QString fromnum(int intnum)
{
    return QString::number(intnum);
}

QString fromnum(float flnum)
{
    return QString::number(flnum);
}

QString fromnum(double dounum)
{
    return QString::number(dounum);
}
QStringList getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    //nameFilters << "*.tif";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    return files;
}

string tostdstring(QString qstr)
{
    return qstr.toStdString();
}

wstring tostdwstring(QString qstr)
{
    return qstr.toStdWString();
}

