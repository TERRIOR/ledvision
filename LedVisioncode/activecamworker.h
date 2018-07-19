#ifndef ACTIVECAMWORKER_H
#define ACTIVECAMWORKER_H
#include "ledcamera.h"
#include <iostream>
#include <QObject>
#include <QMutex>
#include <QThread>
#include "ledclassify.h"
#include <windows.h>
#include <qdebug.h>
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib,"user32.lib")
using namespace std;
class activecamworker:public QObject
{

    Q_OBJECT
public:
    activecamworker();
    activecamworker(int,string);
    void requestWork();//请求工作
    void abort();//结束工作
signals:
    void starttimerecord();
    void workRequested();//已经请求
    void finished();//已经结束工作
    void requeststop(int);
    void imgchange(int);//通知ui主线程更新显示
public slots:
    void doWork();//do something
private:
    string m_camname;
    int m_node;
    bool m_bcam1=false;
    bool m_bcam2=false;
    bool _abort;//abort:停止 用于停止程序的运行
    bool _working;//是否工作的判断的变量
    QMutex inmutex;//类里变量的锁
};

#endif // ACTIVECAMWORKER_H
