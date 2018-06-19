
#include "mytime.h"

mytime::mytime()
{

}

void mytime::TimerStart(){
    QueryPerformanceFrequency(&large_integer);
    DobDff = large_integer.QuadPart;

    QueryPerformanceCounter(&large_integer);
    IntStart = large_integer.QuadPart;
}

double mytime::TimerFinish(){
    QueryPerformanceCounter(&large_integer);
    IntEnd = large_integer.QuadPart;
    DobMillseconds = (IntEnd - IntStart)*1000 / DobDff; //转为ms
    return DobMillseconds;
}

void mytime::OutputToFile(QString fileName){
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
    QByteArray data = QByteArray("The Timer of millseconds is: ")
            + QByteArray::number(DobMillseconds) + QByteArray("ms\n");
    file.write(data);
    file.close();
}
