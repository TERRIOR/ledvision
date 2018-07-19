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
#include <qmessagebox.h>
#include <signaldialog.h>
#include "ledclassify.h"
#include <qglobal.h>
#include <windows.h>

#include <opencv2/opencv.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ledcamera.h"
#include "controlthread.h"
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
    explicit MainWindow(bool model,QWidget *parent = 0);
    ~MainWindow();
    //初始化界面主题
    void initform();

    //更新模板图片列表
    void refreshpiclist();

    ///相机变量

    //在回调函数用到的变量

    Mat m_workmat1;
    Mat m_workmat2;
    int m_icountall=0;//总数
    int m_errorcount[10];
    //图像设置，分为两种不同的信号源
    void imgsetting(int i, int j);
    void showres(bool i,int j);
    void UpdateGUI(QLabel *ql, Mat *imgshow);
    void showparam();
    void setparam();
    void setup();
    void savecam(QTextStream &out);
    void loadcam(QTextStream &in);
private slots:
    void Recupdateui(bool,int);
    void RecClassifyError(int);
    void RecClassifySuccess(int);
    void on_pushButton_min_clicked();
    void on_pushButton_max_clicked();
    void on_pushButton_close_clicked();
    void on_pushButton_signal_clicked();
    void receivesignal(int i,int j,string m,string n);
    void on_pushButton_run_toggled(bool checked);
    void on_pushButton_file_clicked();
    void on_tableWidgetfile_cellDoubleClicked(int row, int column);
    void on_pushButton_setimg_clicked();
    void on_pushButton_saveimg_clicked();
    void on_pushButton_takeimg_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_clicked();
    void on_pushButton_load_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QRect location;
    controlthread *conthread=NULL;

    int m_isignalsource1=0;
    int m_isignalsource2=0;
    string m_scamname1="NULL";
    string m_scamname2="NULL";
    bool max;
    Mat m_fileimg;
    int m_fileimgnode;
    bool m_model=false;
    QString m_filename;
    QStringList m_qstrlist;
    QString m_morenfile="moren.zv";
    void UpdateTextedit(QTextEdit *editdown, QTextEdit *editup, thresholdparam thp);
    void setTextedit(QTextEdit *editdown, QTextEdit *editup, thresholdparam &thp);

};

#endif // MAINWINDOW_H
