#ifndef RULE_H
#define RULE_H
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "math.h"
#include <iostream>

#define PI 3.1415926
#define FILTER_N 3
using namespace std;
using namespace cv;
class rule{
public:
    rule();
    ~rule();
    rule(Point p1,Point p2,const Mat &img,int width);
    //优先使用下面的构造函数,注意p1是圆外点,p2是圆内点;
    rule(Point p1, Point p2, const Mat &img, int width, int up, int down);
    void calrule(Point p1, Point p2, const Mat &img, int width);
    void caledealmat(Point p1,Point p2,const Mat &img,int width);//计算出待处理区域的mat
    void calgrayhis();//根据mat算出沿位置分布的灰度xy函数
    void calgradhis();//根据mat算出沿位置分布的灰度梯度xy函数
    void calgray();//计算灰度数组
    void calhis();//同时计算数组，没滤波
    void calgrad();//计算梯度数组
    Point calpoint(int x, int length);//根据分布图横坐标返回原图中的坐标
    void calminmax(int zero_up, int zero_down);
    void filter(int *func,int size,int num);
    void imrotate(const Mat& img, Mat& newIm, double angle,Point2f pt);//旋转
    Point pointResult() const;
    void setPointResult(const Point &pointResult);
    Point PointEnd() const;
    void setPointEnd(const Point &PointEnd);
    Point pointStart() const;
    void setPointStart(const Point &pointStart);
    void showHistogram();
    int *getGrayHistogram() const;
    int *getGradientHis() const;
    Mat getDealmat() const;
    void setDealmat(const Mat &value);
    double getAngle() const;
    Mat *getGrayhisMat() const;
    Mat *getGradhisMat() const;
    int getMax() const;
    int getMin() const;
    int getMinxa() const;
    int getMinxb() const;
    int getMaxxc() const;
    int getMaxxd() const;
    int getGrayhislength() const;
    int getGradhislength() const;
    int getMaxindex() const;
    int getMinindex() const;
    void setzeroband(int up,int down);

private:
    bool binver=false;
    Mat dealmat;
    int m_width;
    double angle;
    int max=0,min=0;//最大值最小值
    int maxindex=0,minindex=0;//最值索引
    Point m_pointStart;//开始的坐标
    Point m_PointEnd;//结束的坐标
    Point m_pointResult;//边界结果的坐标
    int *GrayHistogram=NULL;//灰度的柱状图
    Mat *GrayhisMat=NULL;//灰度的显示mat
    int *GradientHis=NULL;//梯度柱状图
    Mat *GradhisMat=NULL;//梯度的显示mat
    int zeroup=3,zerodown=-3;//默认数据
    int minxa=0,minxb=0,maxxc=0,maxxd=0;//梯度图的零点
    int grayhislength;//灰度长度
    int gradhislength;//梯度长度
};
//最小二乘法 拟合圆
void circleLeastFit(const std::vector<cv::Point> points, cv::Point &center, double &radius);
#endif // RULE_H
