#include "ledclassify.h"


bool ledclassify::ledback(Mat &ledmat,float scale)
{
    return true;
}

void ledclassify::initparam()
{
    //初始化参数阈值
    m_width.setdownup(150,170);
    m_hight.setdownup(150,170);
    m_ratio.setdownup(1,1.06);
    m_gradmax.setdownup(10,50);
    m_gradmin.setdownup(-4,4);
    m_maxlength.setdownup(0,11);
    m_minlength.setdownup(0,3);
    m_offset.setdownup(0,5);
    m_radius.setdownup(57,65);
}

ledclassify::ledclassify(QObject *parent)
{

}

bool ledclassify::ledfront(Mat &mat,float scale)
{
    Mat ledmat;
    Mat ledmat2;
    vector<Point> vecp;
    mytime timer;
    timer.TimerStart();
    Mat image,scale_mat;
    mat.copyTo(scale_mat);
    //TODO:修改缩小比例，图像金字塔参数
    scalemat(scale_mat,scale);
    cvtColor(scale_mat , scale_mat , CV_RGBA2RGB);
    getimgroi(scale_mat,0.6);

    cvtColor(scale_mat , image , CV_RGBA2GRAY);
    blur(image,image,Size(10*scale,10*scale));
    //TODO:注意阈值，可考虑使用OTSU
    threshold(image,image,200,255,THRESH_BINARY_INV);
    Mat element = getStructuringElement(MORPH_RECT, Size(30*scale, 30*scale));
    morphologyEx(image, image, MORPH_CLOSE, element);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarcy;
    //CV_RETR_EXTERNAL:只检测多连通区域的外部轮廓
    findContours(image, contours, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    vector<RotatedRect> box(contours.size()); //定义最小外接矩形集合
    Point2f rect[4];
    int count=0;
    for(int i=0; i<contours.size(); i++)
    {
        double area = contourArea(Mat(contours[i]));
        if(area<1000)break;
        box[i] = minAreaRect(Mat(contours[i]));  //计算每个轮廓最小外接矩形
        box[i].points(rect);  //把最小外接矩形四个端点复制给rect数组
        int angle=box[i].angle;
        int line1 = sqrt((rect[1].y-rect[0].y)*(rect[1].y-rect[0].y)+(rect[1].x-rect[0].x)*(rect[1].x-rect[0].x));
        int line2 = sqrt((rect[3].y-rect[0].y)*(rect[3].y-rect[0].y)+(rect[3].x-rect[0].x)*(rect[3].x-rect[0].x));
        int x=line2;
        int y=line1;
        if (line1 > line2)
        {
            angle = 90 + angle;
            x=line1;
            y=line2;
        }

        m_width.setNow(x);
        m_hight.setNow(y);
        m_ratio.setNow(float(x)/y);
        //判断冲烂
        if(m_width.inrange()&&m_hight.inrange()&&m_ratio.inrange()){
            sendfirstres(true);
        }else{
            sendfirstres(false);
            return false;
        }

        imrotate(scale_mat,ledmat,angle,box[i].center,Size(line1,line2));//旋转
        ledmat=ledmat(Rect(box[i].center.x-x/2,box[i].center.y-y/2,x,y));
        count++;
    }

    ledmat.copyTo(ledmat2);
    int centerx=ledmat.cols/2;
    int centery=ledmat.rows/2;

    int radioin=70*scale;
    int radioout=170*scale;
    //加入45度的卡尺
    vecp.push_back(Point(centerx-radioout*0.704,centery-radioout*0.704));
    vecp.push_back(Point(centerx-radioin*0.704,centery-radioin*0.704));
    vecp.push_back(Point(centerx-radioout*0.704,centery+radioout*0.704));
    vecp.push_back(Point(centerx-radioin*0.704,centery+radioin*0.704));
    vecp.push_back(Point(centerx+radioout*0.704,centery-radioout*0.704));
    vecp.push_back(Point(centerx+radioin*0.704,centery-radioin*0.704));
    vecp.push_back(Point(centerx+radioout*0.704,centery+radioout*0.704));
    vecp.push_back(Point(centerx+radioin*0.704,centery+radioin*0.704));

    vector<Point>::iterator it=vecp.begin();

    rule rule1(it[0],it[1],ledmat,40*scale,3,-3);
    rule rule2(it[2],it[3],ledmat,40*scale,3,-3);
    rule rule3(it[4],it[5],ledmat,40*scale,3,-3);
    rule rule4(it[6],it[7],ledmat,40*scale,3,-3);

    line(ledmat, it[0], it[1], Scalar(255, 0,255), 2, 10);
    line(ledmat, it[2],it[3], Scalar(255, 0,255), 2, 10);
    line(ledmat,it[4],it[5], Scalar(255, 0,255), 2, 10);
    line(ledmat, it[6],it[7], Scalar(255, 0,255), 2, 10);

    vector<Point> veccircle;
    Point p1=rule1.calpoint(rule1.getMaxxd(),rule1.getGradhislength());
    Point p2=rule2.calpoint(rule2.getMaxxd(),rule2.getGradhislength());
    Point p3=rule3.calpoint(rule3.getMaxxd(),rule3.getGradhislength());
    Point p4=rule4.calpoint(rule4.getMaxxd(),rule4.getGradhislength());

    veccircle.push_back(p1);
    veccircle.push_back(p2);
    veccircle.push_back(p3);
    veccircle.push_back(p4);

    Point centerpoint;
    double r;
    //拟合圆
    circleLeastFit(veccircle,centerpoint,r);
    cout<<" radius :"<<r<<endl;
    vecp.clear();
    /*同时显示多图像*/
    vector<Mat> vecmat;
    vecmat.push_back(*rule1.getGradhisMat());
    vecmat.push_back(*rule2.getGradhisMat());
    vecmat.push_back(*rule3.getGradhisMat());
    vecmat.push_back(*rule4.getGradhisMat());
    vecmat.push_back(*rule1.getGrayhisMat());
    vecmat.push_back(*rule2.getGrayhisMat());
    vecmat.push_back(*rule3.getGrayhisMat());
    vecmat.push_back(*rule4.getGrayhisMat());
    //imshowMany("his",vecmat);
    //梯度图最大值
    int max=(rule1.getMax()+rule2.getMax()+rule3.getMax()+rule4.getMax())/4;
    m_gradmax.setNow(max);
    //梯度图最小值
    int min=(rule1.getMin()+rule2.getMin()+rule3.getMin()+rule4.getMin())/4;
    m_gradmin.setNow(min);
    //梯度图最小值波长度
    int minlength=(rule1.getMinxb()-rule1.getMinxa()+rule2.getMinxb()-rule2.getMinxa()
                 +rule3.getMinxb()-rule3.getMinxa()+rule4.getMinxb()-rule4.getMinxa())/4;
    m_minlength.setNow(minlength);
    //梯度图最大值波长度
    int maxlength=(rule1.getMaxxd()-rule1.getMaxxc()+rule2.getMaxxd()-rule2.getMaxxc()
                   +rule3.getMaxxd()-rule3.getMaxxc()+rule4.getMaxxd()-rule4.getMaxxc())/4;
    m_maxlength.setNow(maxlength);
    int offsetcenter;//圆心的偏差
    offsetcenter=abs(centerpoint.x-centerx)+abs(centerpoint.y-centery);
    m_offset.setNow(offsetcenter);
    m_radius.setNow(r);
    cout<<offsetcenter<<endl;
    //根据波峰参数判断
    //if(min>-4&&maxlength<11&&max>=10&&minlength<3&&(r>57&&r<65)&&offsetcenter<5){
    if(m_gradmin.inrange()&&m_minlength.inrange()&&m_gradmax.inrange()&&m_maxlength.inrange()
            &&m_offset.inrange()&&m_radius.inrange())
    {
        emit sendsecondres(true);

    }else{
        emit sendsecondres(false);
        return false;
    }
    cout<<"time: "<<timer.TimerFinish()<<endl;
    return true;
}
