#include "mainwindow.h"
#include <QApplication>
#include "CameraApi.h"
#include "windows.h"
#pragma comment(lib,"User32.lib")
#ifdef _WIN64
#pragma comment(lib,"MVCAMSDK_X64.lib")
#else
#pragma comment(lib,"MVCAMSDK.lib")
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CameraSdkInit(1);
    //句柄名字
    QString win="LedVision";
    //查找句柄
    HWND hwnd= FindWindow(NULL,win.toStdWString().c_str());
    MainWindow w(hwnd!=NULL);
    //获取显示器的数目
    QDesktopWidget* desktop = QApplication::desktop();
    int N = desktop->screenCount();
    if(hwnd!=NULL){
       //显示器数目大于1时，辅助程序在第二屏幕打开
        if(N>1){
           w.setGeometry(desktop->screenGeometry(1));
        }
        //设置辅助程序的句柄
        w.setWindowTitle("LedVision2");
    }else{
        w.setGeometry(desktop->screenGeometry(0));
        //设置主程序的句柄
        w.setWindowTitle("LedVision");
    }
    w.show();

    return a.exec();
}
