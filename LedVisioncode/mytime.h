/**
*@projectName   ledvision
*@documentname  mytime.cpp
*@author        zzJun
*@date          20180619
*@brief         测量时间，精确到ms
**/
#ifndef MYTIME_H
#define MYTIME_H
#include "Windows.h"
#include <qstring>
#include <qfile>
class mytime
{
public:
    mytime();

    void TimerStart();

    double TimerFinish();

    //当然这个可以不要，根据测试需要添加
    void OutputToFile(QString fileName);
private:
    LARGE_INTEGER large_integer;
    __int64 IntStart;
    __int64 IntEnd;
    double DobDff;
    double DobMillseconds;

};


#endif // MYTIME_H
