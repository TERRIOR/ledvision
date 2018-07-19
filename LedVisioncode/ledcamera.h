#ifndef LEDCAMERA_H
#define LEDCAMERA_H

#include <windows.h>
#include <opencv2/opencv.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <qobject.h>
#include "CameraApi.h"
#include "ledclassify.h"

#pragma comment(lib,"User32.lib")
using namespace std;
using namespace cv;

class ledcamera:public QObject
{
    Q_OBJECT
public:
    int		m_hCamera[2];
    BYTE*	m_pRawBuffer[2];
    BYTE*	m_pFrameBuffer[2];
    UINT    m_threadID[2];//图像抓取线程的ID
    HANDLE  m_hDispThread[2];//图像抓取线程的句柄
    BOOL    m_bExit[2];//用来通知图像抓取线程结束
    tSdkFrameHead   m_sFrInfo[2];//用于保存当前图像帧的帧头信息
    ledcamera();
    //初始化相机
    bool caminit(string str,int signalnode,int model=2);
    //释放相机
    bool camuninit(int i);
    Mat m_workmat1;
    Mat m_workmat2;
    void imgsetting(int i, int j);
    int isignal1camnode() const;
    void getimg(int i);
    int isignal2camnode() const;
    void softtrigger(int );
    int getIsignal1camnode() const;

    int getIsignal2camnode() const;
    void starttime(int node);
    void stoptime(int node);

    void recordtime(int node);
signals:
    void sendupdateui(bool b,int i);
private slots:

private:
    bool m_bsignal1inited=false;
    bool m_bsignal2inited=false;
    int m_isignal1camnode=-1;
    int m_isignal2camnode=-1;
    mytime m_time1;
    mytime m_time2;
    int m_firsttime1=0;
    int m_firsttime2=0;
    int m_secondtime1=0;
    int m_secondtime2=0;
    int m_delaytime1=0;
    int m_delaytime2=0;
};
extern ledcamera ledcam;
#endif // LEDCAMERA_H
