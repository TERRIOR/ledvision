#ifndef CONTROLTHREAD_H
#define CONTROLTHREAD_H

#include <QObject>
#include "activecamworker.h"
#include<iostream>
using namespace std;
class controlthread: public QObject
{
    Q_OBJECT
public:
    controlthread();
    controlthread(string ,string);
    ~controlthread();
    void stopthread1();
    void stopthread2();
    void startthread1();
    void startthread2();
    void release();
private slots:
    void refleshui(int);
    void restop(int);
signals:
    void sendui(bool,int);
private:
    QThread *thread;
    activecamworker *worker;
    QThread *thread2;
    activecamworker *worker2;
    bool m_opened1=false;
    bool m_opened2=false;
    bool m_uicall=false;//true:是dialog调用
};

#endif // CONTROLTHREAD_H
