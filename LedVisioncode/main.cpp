#include "mainwindow.h"
#include <QApplication>
#include "CameraApi.h"
#ifdef _WIN64
#pragma comment(lib,"MVCAMSDK_X64.lib")
#else
#pragma comment(lib,"MVCAMSDK.lib")
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CameraSdkInit(1);
    MainWindow w;
    w.show();

    return a.exec();
}
