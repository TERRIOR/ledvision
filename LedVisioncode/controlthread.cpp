#include "controlthread.h"


controlthread::controlthread()
{
//    thread = new QThread();
//    worker = new activecamworker();

//    worker->moveToThread(thread);
//    connect(worker, SIGNAL(imgchange()),this, SLOT(refleshmat()));//与ui结合时用
//    connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));//1,worker请求，触发thread的start
//    connect(thread, SIGNAL(started()), worker, SLOT(doWork()));//2,start后 dowork(实质工作的地方)
//    connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);
}
controlthread::controlthread(string name1,string name2)
{
    //线程一
    thread = new QThread();
    worker = new activecamworker(0,name1);
    worker->moveToThread(thread);
    //线程二
    thread2 = new QThread();
    worker2 = new activecamworker(1,name2);
    worker2->moveToThread(thread2);
    //线程一连接
    connect(worker,SIGNAL(requeststop(int)),this,SLOT(restop(int)));
    connect(worker, SIGNAL(imgchange(int)),this, SLOT(refleshui(int)));//与ui结合时用
    connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));//1,worker请求，触发thread的start
    connect(thread, SIGNAL(started()), worker, SLOT(doWork()));//2,start后 dowork(实质工作的地方)
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);
    //线程二连接
    connect(worker2,SIGNAL(requeststop(int)),this,SLOT(restop(int)));
    connect(worker2, SIGNAL(imgchange(int)),this, SLOT(refleshui(int)));//与ui结合时用
    connect(worker2, SIGNAL(workRequested()), thread2, SLOT(start()));//1,worker请求，触发thread的start
    connect(thread2, SIGNAL(started()), worker2, SLOT(doWork()));//2,start后 dowork(实质工作的地方)
    connect(worker2, SIGNAL(finished()), thread2, SLOT(quit()), Qt::DirectConnection);
}

controlthread::~controlthread()
{
    delete thread;
    delete worker;
    thread=NULL;
    worker=NULL;
    delete thread2;
    delete worker2;
    thread2=NULL;
    worker2=NULL;
    cout<<"release control"<<endl;
}

void controlthread::stopthread2()
{
    if(m_opened2){
        worker2->abort();
        thread2->wait();
        m_opened2=false;
    }
}

void controlthread::startthread2()
{
    if(!m_opened2){
        worker2->abort();
        thread2->wait();
        worker2->requestWork();
        m_opened2=true;
    }
}

void controlthread::stopthread1()
{
    if(m_opened1){
        worker->abort();
        thread->wait();
        m_opened1=false;
    }
}

void controlthread::startthread1()
{
    if(!m_opened1){
        worker->abort();
        thread->wait();
        worker->requestWork();
        m_opened1=true;
    }
}

void controlthread::release()
{
    stopthread1();
    stopthread2();
    delete this;

}

void controlthread::refleshui(int i)
{
    emit sendui(true,i);
}

void controlthread::restop(int i)
{
    switch (i) {
    case 0:
        if(m_opened1){
            thread->wait();
            m_opened1=false;
        }
        break;
    case 1:
        if(m_opened2){
            thread2->wait();
            m_opened2=false;
        }
        break;
    default:
        break;
    }
}
