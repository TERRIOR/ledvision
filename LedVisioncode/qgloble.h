#ifndef QGLOBLE_H
#define QGLOBLE_H
#include <QDir>
#include "iostream"
#include <QDebug>
#include <Vector>
#include <string>

using namespace std;

QString fromstdstring(string str);
QString fromnum(int intnum);
QString fromnum(float flnum);
QString fromnum(double dounum);
int qtoint(QString qstr);
float qtofloat(QString qstr);
double qtodouble(QString qstr);
string tostdstring(QString qstr);
wstring tostdwstring(QString qstr);
QStringList getFileNames(const QString &path);
#endif // QGLOBLE_H
