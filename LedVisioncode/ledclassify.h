/**
*@projectName   ledvision
*@documentname  ledclassify.h
*@author        zzJun
*@date          20180619
*@brief         led的判断方法都放这个类
**/

#ifndef LEDCLASSIFY_H
#define LEDCLASSIFY_H
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "thresholdparam.h"
#include "rule.h"
#include "cvgloble.h"
#include "mytime.h"
#include "qgloble.h"
#include <QObject>
using namespace std;
using namespace cv;

class ledclassify: public QObject
{
    Q_OBJECT
public:
    explicit ledclassify(QObject *parent = 0);
    bool ledfront(Mat &ledmat,float scale=0.5);
    bool ledback(Mat &ledmat,float scale=0.5);
    void initparam();
signals:
    void sendsecondres(bool);
    void sendfirstres(bool);
    void sendthirdres(bool);
private:
    bool m_front_jiao;
    bool m_front_ya;

    thresholdparam m_width;
    thresholdparam m_hight;
    thresholdparam m_ratio;
    thresholdparam m_gradmax;
    thresholdparam m_gradmin;
    thresholdparam m_maxlength;
    thresholdparam m_minlength;
    thresholdparam m_offset;
    thresholdparam m_radius;

};

#endif // LEDCLASSIFY_H
