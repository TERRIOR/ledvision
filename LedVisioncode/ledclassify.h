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
enum error_type{
    errornoled,
    errorothersth,
    errorbroken,
    errorglue,
    errorfmater,
    errorfoot,
    errorotherfront,
    errorotherback,
    returndeal
};
class ledclassify: public QObject
{
    Q_OBJECT

public:
    ledclassify();
    bool ledfront(Mat &ledmat,float scale=0.5);
    bool ledback(Mat &ledmat,float scale=0.5);
    void ledload(QTextStream &in);
    void ledsave(QTextStream &out);
    void initparam();
    thresholdparam m_width;
    thresholdparam m_hight;
    thresholdparam m_ratio;
    thresholdparam m_gradmax;
    thresholdparam m_gradmin;
    thresholdparam m_maxlength;
    thresholdparam m_minlength;
    thresholdparam m_offset;
    thresholdparam m_radius;
    thresholdparam m_footratio;
    thresholdparam m_emptyratio;
    thresholdparam m_highsamilar;
    thresholdparam m_emptysamilar;
    thresholdparam m_fmthresh;
    thresholdparam m_fmcount;
    bool calfront(Mat &mat, float scale);
    bool calback(Mat ledmat, float scale);
    void getroi(Mat &ledmat, Rect *rcV,Mat *underV);
    bool calcornor(Mat &cornormat, Rect &bd);
    void saveparam(QTextStream &out, const thresholdparam &param);
    void loadparam(QTextStream &in,thresholdparam &param);
signals:
    /**
     * @brief sendsuccess
     * 发送正确信号
     * 0：工位1
     * 1：工位2（最终结果）
     */
    void sendsuccess(int);
    /**
     * @brief senderror
     * 发送错误信号，给主程序进行记录
     * param int：
     * 0：空白 没有零件
     * 1：其他异物
     * 2：压烂
     * 3：多少胶
     * 4：异物
     * 5：引脚
     * 6：其他问题正面
     * 7：其他问题反面
     * 8：放反，回炉
     */
    void senderror(int);
private:
    bool m_front_jiao;
    bool m_front_ya;

};

#endif // LEDCLASSIFY_H
