/**
*@projectName   ledvision
*@documentname  mainwindow.h
*@author        zzJun
*@date          20180619
*@brief         led芯片主页面
**/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qfile.h>
#include <QDesktopWidget>
#include <qwidget.h>
#include <signaldialog.h>
#include <qglobal.h>
#include <windows.h>
#include "CameraApi.h"
#include <opencv2/opencv.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //初始化界面主题
    void initform();
    //初始化相机
    bool caminit(string str,int signalnode);
    //释放相机
    void camuninit(int i);
    ///相机变量
    int		m_hCamera[2];
    BYTE*	m_pFrameBuffer[2];
    UINT    m_threadID[2];//图像抓取线程的ID
    HANDLE  m_hDispThread[2];//图像抓取线程的句柄
    BOOL    m_bExit[2];//用来通知图像抓取线程结束
    tSdkFrameHead   m_sFrInfo[2];//用于保存当前图像帧的帧头信息

private slots:
    void on_pushButton_min_clicked();

    void on_pushButton_max_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_signal_clicked();

    void receivesignal(int i,int j,string m,string n);

private:
    Ui::MainWindow *ui;
    QRect location;
    bool m_bsignal1inited=false;
    bool m_bsignal2inited=false;
    int m_isignal1camnode=-1;
    int m_isignal2camnode=-1;
    int m_isignalsource1=0;
    int m_isignalsource2=0;
    string m_scamname1="";
    string m_scamname2="";
    bool max;

};

#endif // MAINWINDOW_H
