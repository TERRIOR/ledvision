#include "activecamworker.h"

activecamworker::activecamworker()
{
    _working =false;
    _abort = false;
}

activecamworker::activecamworker(int i,string name):m_node(i),m_camname(name)
{
    _working =false;
    _abort = false;
}

void activecamworker::requestWork()
{
    inmutex.lock();
    _working = true;
    _abort = false;
    qDebug()<<"Request worker start in Thread "<<thread()->currentThreadId();
    inmutex.unlock();
    emit workRequested();
}

void activecamworker::abort()
{
    inmutex.lock();
    if (_working) {
        _abort = true;
        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
    }
    inmutex.unlock();
}

void activecamworker::doWork()
{
    //设置为连续采集模式
//    switch (m_node) {
//    case 0:
//        ledcam.getimg(ledcam.isignal1camnode());
//        break;
//    case 1:
//        ledcam.getimg(ledcam.isignal2camnode());
//        break;
//    default:
//        break;
//    }
    if(!ledcam.caminit(m_camname,m_node,0)){
        cout<<"初始化失败"<<endl;
        return ;
    }
    ledcam.starttime(m_node);
    while(1){
        inmutex.lock();
        bool abort = _abort;
        inmutex.unlock();
        if (abort) {
            qDebug()<<"Aborting worker process in Thread "<<thread()->currentThreadId();
            emit requeststop(m_node);
            Sleep(0);
            break;
        }
        //获取当前相机的图像
        //处理图像，判断
        //TODO:添加运动检测
        cout<<"run" <<m_node<<endl;
        bool res;
        switch (m_node) {
        case 0:
            ledcam.getimg(ledcam.isignal1camnode());
            res=ledc.ledcalibration(ledcam.m_workmat1,0);
            if(res!=m_bcam1){
                if(res){
                    ledcam.recordtime(m_node);
                }else{
                    ledcam.stoptime(m_node);
                    //关闭线程
                    _abort=true;

                }
                m_bcam1=res;
            }
            break;
        case 1:
            ledcam.getimg(ledcam.isignal2camnode());
            res=ledc.ledcalibration(ledcam.m_workmat2,1);
            if(res!=m_bcam2){
                if(res){
                    ledcam.recordtime(m_node);
                }else{
                    ledcam.stoptime(m_node);
                    //关闭线程
                    _abort=true;

                }
                m_bcam2=res;
            }
            break;
        default:
            break;
        }
        emit imgchange(m_node);
        Sleep(0);
    }
    //释放
    ledcam.camuninit(m_node);
    emit finished();
}
