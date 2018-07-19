#include "ledclassify.h"
ledclassify ledc;
void ledclassify::initparam()
{
    //初始化参数阈值
    m_width.setdownup(150,170);
    m_hight.setdownup(150,170);
    m_ratio.setdownup(0.9,1.06);
    m_gradmax.setdownup(9,50);
    m_gradmin.setdownup(-4,4);
    m_maxlength.setdownup(-1,11);
    m_minlength.setdownup(-1,3);
    m_offset.setdownup(0,5);
    m_radius.setdownup(57,65);
    m_fmthresh.setdownup(42,255);
    m_fmcount.setdownup(20,500);
    //引脚
    m_emptyratio.setdownup(0,0.5);
    m_footratio.setdownup(0,0.6);
    m_emptysamilar.setdownup(0.2,1);
    m_highsamilar.setdownup(0.25,1);
}

bool ledclassify::ledcalibration(const Mat &mat, int camnode, float scale)
{
    cout<<"calibrate"<<endl;
    mytime timer;
    timer.TimerStart();
    bool res;
    if(mat.empty())
        return false;
    switch (camnode) {
    case 0:
        res=calcalibration(mat,m_bcaminited1,scale,m_vibe1);
        break;
    case 1:
        res=calcalibration(mat,m_bcaminited2,scale,m_vibe2);
        break;
    default:
        break;
    }
    cout<<"time: "<<timer.TimerFinish()<<endl;
    return res;
}

bool ledclassify::calcalibration(const Mat &mat, bool &inited, float scale, ViBe_BGS &Vibe_Bgs)
{
    Mat gray;
    cvtColor(mat,gray,CV_RGBA2GRAY);
    scalemat(gray,scale);
    if(!inited){
        Vibe_Bgs.init(gray);
        Vibe_Bgs.processFirstFrame(gray);
        cout << " Training ViBe complete!" << endl;
        inited=true;
        return false;
    }
    Vibe_Bgs.testAndUpdate(gray);
    Mat mask = Vibe_Bgs.getMask();
    int mask_area = countNonZero(mask);
    cout<<"area:"<<mask_area<<endl;
    if(mask_area<2000)
        return false;
    return true;
}
void ledclassify::getroi(Mat &ledmat,Rect *rcV,Mat *underV){
    double centerx=ledmat.cols/2;
    double centery=ledmat.rows/2;
    //纵向
    int widthV = centerx*0.48;         //*↓
    int heightV = centery*0.55;
    vector<Point> vtVert;
    vtVert.push_back(Point(centerx*0.28, 0));
    vtVert.push_back(Point(centerx*1.25, 0));
    vtVert.push_back(Point(centerx*0.28, centery*1.45));
    vtVert.push_back(Point(centerx*1.25, centery*1.45));
    //cout<<vtVert[0].x <<endl;
    rcV[0] =Rect(vtVert[0].x, vtVert[0].y, widthV, heightV);          //左上
    rcV[1] =Rect(vtVert[1].x, vtVert[1].y, widthV, heightV);          //右上
    rcV[2] =Rect(vtVert[2].x, vtVert[2].y, widthV, heightV);        //左下
    rcV[3] =Rect(vtVert[3].x, vtVert[3].y, widthV, heightV);//*↑   //右下
    for (int i = 0; i<4; i++)
    {
        underV[i]=ledmat(rcV[i]);
    }
}
bool ledclassify::calback(const Mat &mat_b,float scale){
    //TODO:添加背面处理代码
    Mat ledmat;
    //mat_b.copyTo(ledmat);
    cvtColor(mat_b, ledmat, CV_RGBA2GRAY);
    scalemat(ledmat, scale);
    getimgroi(ledmat, 0.9);   //*
    Mat image;
    //cvtColor(ledmat, image, CV_RGBA2GRAY);
    blur(ledmat, image, Size(5, 5)); //*
    threshold(image, image, 200, 255, THRESH_BINARY_INV);//*
    Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));//*
    morphologyEx(image, image, MORPH_CLOSE, element);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarcy;

    //CV_RETR_EXTERNAL:只检测多连通区域的外部轮廓
    findContours(image, contours, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    vector<RotatedRect> box(contours.size()); //定义最小外接矩形集合
    Point2f rect[4];
    //std::cout << "contours.size() = " << contours.size()<< endl;
    if (contours.size() == 0)
    {
        emit senderror(0); //others defected1,NG
        return false;
    }
    int i;
    for (i=0; i<contours.size(); i++)
    {
        double area = contourArea(Mat(contours[i]));
        //cout <<"contourArea = " <<area <<endl;
        if (area < 10000*4*scale*scale||area>40000*4*scale*scale)continue;
        box[i] = minAreaRect(Mat(contours[i]));  //计算每个轮廓最小外接矩形
        box[i].points(rect);  //把最小外接矩形四个端点复制给rect数组
        int angle=box[i].angle;
        int line1 = sqrt((rect[1].y-rect[0].y)*(rect[1].y-rect[0].y)+(rect[1].x-rect[0].x)*(rect[1].x-rect[0].x));
        int line2 = sqrt((rect[3].y-rect[0].y)*(rect[3].y-rect[0].y)+(rect[3].x-rect[0].x)*(rect[3].x-rect[0].x));
        int x=line2;
        int y=line1;
        imrotate(ledmat, ledmat, angle+90, box[i].center);//旋转
        //cv::circle(dstImg, Point(box[i].center.x, box[i].center.y), 5, Scalar(0, 255, 0), -1, 8);  //绘制最小外接矩形的中心点
        ledmat=ledmat(Rect(box[i].center.x-x/2, box[i].center.y-y/2, x, y));
        break;
    }
    if(i==contours.size()){
        senderror(1);
        return false;
    }

    //cv::imshow("dst", ledmat);
    //cout<<"led.size："<<ledmat.cols<<"*"<<ledmat.rows<<endl;
    //纵向
    double areaV[4];
    double SquaV[4];
    Rect rcV[4];
    Mat underV[4];
    Mat image1;
    //cvtColor(ledmat, image1, CV_RGBA2GRAY);
    ledmat.copyTo(image1);
    getroi(image1,rcV,underV);
#if 0
    //绘制纵向ROI矩形
    cv::rectangle(ledmat, rcV[0], Scalar(0, 0, 255), 1, 1, 0);
    cv::rectangle(ledmat, rcV[1], Scalar(0, 0, 255), 1, 1, 0);
    cv::rectangle(ledmat, rcV[2], Scalar(0, 0, 255), 1, 1, 0);
    cv::rectangle(ledmat, rcV[3], Scalar(0, 0, 255), 1, 1, 0);
    cv::imshow("l", ledmat);//画出截取矩形
#endif
    Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3)); //*可以不改
    for (int i = 0; i<4; i++)
    {
        threshold(underV[i], underV[i],200,255,THRESH_BINARY);  //*
        morphologyEx(underV[i], underV[i], MORPH_CLOSE, element1);

        areaV[i] = countNonZero(underV[i]); //统计非0的像素值
        SquaV[i] = areaV[i]/(rcV[i].width * rcV[i].height);//各区域矩形度
    }
    //imshow("underV000", underV[0]); imshow("underV111", underV[1]);
    //imshow("underV222", underV[2]); imshow("underV333", underV[3]);
    //方向反，旋转后重新计算
    if ((SquaV[0]<0.8) && (SquaV[1]<0.8) && (SquaV[2]<0.8) && (SquaV[3]<0.8)) //*
    {
        transpose(image1, image1);
        //0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
        flip(image1, image1, 1);
        getroi(image1,rcV,underV);
        for (int i = 0; i<4; i++)
        {

            threshold(underV[i], underV[i],200,255,THRESH_BINARY);  //*
            morphologyEx(underV[i], underV[i], MORPH_CLOSE, element1);
            areaV[i] = countNonZero(underV[i]); //统计非0的像素值
            SquaV[i] = areaV[i]/(rcV[i].width * rcV[i].height);//各区域矩形度
        }
    }

    Rect realrectv[4];
    for(int i=0;i<4;i++){
        //此处获得每个角落的真实正方形轮廓
        if(!calcornor(underV[i],realrectv[i])){
            return false;
        }
        //白色占比小于0.5 真实正方形尺寸小于模板的0.6时 均是NG零件
        m_emptyratio.setNow(SquaV[i]);
        m_footratio.setNow((float)realrectv[i].height/rcV[i].height);
        if(m_emptyratio.inrange()||m_footratio.inrange()){
            emit senderror(5);
            return false;
        }
        //偶数次检测一次 即是同一侧的引脚 进行数据对比
        if(i%2==1){
            m_highsamilar.setNow(abs((float)realrectv[i].height-realrectv[i-1].height)/rcV[i].height);
            m_emptysamilar.setNow(fabs(SquaV[i] - SquaV[i-1]));
            if(m_highsamilar.inrange()||m_emptysamilar.inrange()){
                emit senderror(5);
                return false;
            }
        }
    }
    //背面的检测成功 OK
    sendsuccess(1);
    return true;
}
bool ledclassify::calcornor(Mat &cornormat,Rect &bd){
    vector<vector<Point>> contours;
    vector<Vec4i> hierarcy;
    //CV_RETR_EXTERNAL:只检测多连通区域的外部轮廓

    findContours(cornormat, contours, hierarcy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //vector<Rect> box(contours.size()); //定义最小外接矩形集合
    if (contours.size() == 0||contours.size()>10)
    {

        emit senderror(6);
        return false;
    }
    int i;
    for(i=0; i<contours.size(); i++)
    {
        double area = contourArea(Mat(contours[i]));
        cout<<area<<endl;
        if(area<500)continue;
        bd=boundingRect(Mat(contours[i]));
        break;
    }
    if(i==contours.size()){
        senderror(5);
        return false;
    }

}
bool ledclassify::ledback(Mat &ledmat,float scale)
{
    cout<<"back"<<endl;
    mytime timer;
    timer.TimerStart();
    if(ledmat.empty()){
        return false;
    }
    bool res = calback(ledmat,scale);
    cout<<"time: "<<timer.TimerFinish()<<endl;
    return res;
}
void ledclassify::loadparam(QTextStream &in,thresholdparam &param){
    float a,b;
    in>>a;
    in>>b;
    param.setdownup(a,b);
}
void ledclassify::ledload(QTextStream &in)
{
    loadparam(in,m_width);
    loadparam(in,m_hight);
    loadparam(in,m_ratio);
    loadparam(in,m_gradmax);
    loadparam(in,m_gradmin);
    loadparam(in,m_maxlength);
    loadparam(in,m_minlength);
    loadparam(in,m_fmthresh);
    loadparam(in,m_fmcount);
    loadparam(in,m_emptyratio);
    loadparam(in,m_footratio);
    loadparam(in,m_emptysamilar);
    loadparam(in,m_highsamilar);

}
void ledclassify::saveparam(QTextStream &out,const thresholdparam &param){
    out<<param.down();
    out<<' ';
    out<<param.up();
    out<<' ';
}
void ledclassify::ledsave(QTextStream &out)
{
    saveparam(out,m_width);
    saveparam(out,m_hight);
    saveparam(out,m_ratio);
    saveparam(out,m_gradmax);
    saveparam(out,m_gradmin);
    saveparam(out,m_maxlength);
    saveparam(out,m_minlength);
    saveparam(out,m_fmthresh);
    saveparam(out,m_fmcount);
    saveparam(out,m_emptyratio);
    saveparam(out,m_footratio);
    saveparam(out,m_emptysamilar);
    saveparam(out,m_highsamilar);
}


ledclassify::ledclassify()
{
    initparam();
}
bool ledclassify::calfront(const Mat &mat,float scale){
    Mat ledmat;
    Mat ledmat2;
    vector<Point> vecp;

    Mat image,scale_mat;
    cvtColor(mat , scale_mat , CV_RGBA2GRAY);
    //TODO:修改缩小比例，图像金字塔参数
    scalemat(scale_mat,scale);
    getimgroi(scale_mat,0.6);
    scale_mat.copyTo(image);
    //cvtColor(scale_mat , image , CV_RGBA2GRAY);
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
    if (contours.size() == 0)
    {
        emit senderror(0);
        return false;
    }
    Point2f rect[4];
    int count=0;
    int i;
    for(i=0; i<contours.size(); i++)
    {
        double area = contourArea(Mat(contours[i]));
        cout<<area<<endl;
        if(area<10000*4*scale*scale||area>40000*4*scale*scale)continue;
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
            cout<<"冲烂:OK"<<endl;

        }else{
            cout<<"冲烂:NG"<<endl;
            senderror(2);
            return false;
        }

        imrotate(scale_mat,ledmat,angle,box[i].center);//旋转
        ledmat=ledmat(Rect(box[i].center.x-x/2,box[i].center.y-y/2,x,y));
        count++;
        break;
    }
    if(i==contours.size()){
        senderror(1);
        return false;
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

//    line(ledmat, it[0], it[1], Scalar(255, 0,255), 2, 10);
//    line(ledmat, it[2],it[3], Scalar(255, 0,255), 2, 10);
//    line(ledmat,it[4],it[5], Scalar(255, 0,255), 2, 10);
//    line(ledmat, it[6],it[7], Scalar(255, 0,255), 2, 10);

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
    //cout<<" radius :"<<r<<endl;
    vecp.clear();
    /*同时显示多图像*/
//    vector<Mat> vecmat;
//    vecmat.push_back(*rule1.getGradhisMat());
//    vecmat.push_back(*rule2.getGradhisMat());
//    vecmat.push_back(*rule3.getGradhisMat());
//    vecmat.push_back(*rule4.getGradhisMat());
//    vecmat.push_back(*rule1.getGrayhisMat());
//    vecmat.push_back(*rule2.getGrayhisMat());
//    vecmat.push_back(*rule3.getGrayhisMat());
//    vecmat.push_back(*rule4.getGrayhisMat());
//    imshowMany("his",vecmat);
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
    //cout<<offsetcenter<<endl;
    //根据波峰参数判断
    //if(min>-4&&maxlength<11&&max>=10&&minlength<3&&(r>57&&r<65)&&offsetcenter<5){
    if(m_gradmin.inrange()&&m_minlength.inrange()&&m_gradmax.inrange()&&m_maxlength.inrange()
            &&m_offset.inrange()&&m_radius.inrange())
    {
        cout<<"多少胶：OK"<<endl;
    }else{
        cout<<"多少胶：NG"<<endl;
        emit senderror(3);
        return false;
    }
    //第一步正面成功
    //TODO:可能需要固定大小，即是限定图像的尺寸大小，调用resize
    //	Mat mask;
    Mat circleRegion;
    equalizeHist(ledmat2, ledmat2);
    ledmat2.copyTo(circleRegion);
    Mat circleRegion_med;
    //medianBlur(circleRegion, circleRegion_med, 3);
    blur(circleRegion,circleRegion_med,Size(3,3));
    //sobel
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Sobel(circleRegion_med, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);  //*CV_16S根据图像depth改，详查sobel算子。
    Sobel(circleRegion_med, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x); //转回CV_8U
    convertScaleAbs(grad_y, abs_grad_y);
    Mat sobel;
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, sobel);
    //imshow("sobel", sobel);
    Mat elementS = getStructuringElement(MORPH_RECT, Size(2, 2));  //*
    morphologyEx(sobel, sobel, MORPH_DILATE, elementS);
    //imshow("sobel1", sobel);
    blur(sobel,sobel,Size(6,6));
    //消除非缺陷
    Mat maskS1;
    Mat removeS1;
    sobel.copyTo(maskS1);
    maskS1.setTo(cv::Scalar::all(0));
    circle(maskS1, centerpoint, r-5, Scalar(255,255,255), -1, 1);  //*
    //imshow("maskS1", maskS1);
    sobel.copyTo(removeS1, maskS1);

    double thresh = getThreshVal_Otsu_8u_mask(removeS1, maskS1);//掩膜部分Otsu分割
    threshold(removeS1, removeS1, thresh+45, 255, CV_THRESH_BINARY); //*
    //std::cout << "thresh = " << thresh << endl;

    vector<vector<Point>> contr1;
    vector<Vec4i> hierr1;
    vector<int>area;
    vector<int>::iterator ar;
    findContours(removeS1, contr1, hierr1, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    for (int i=0; i<contr1.size(); i++)
        area.push_back(contourArea(Mat(contr1[i])));
    int defectN = 0;
    for (ar = area.begin(); ar != area.end(); ar++)
    {
        std::cout << *ar << endl;
        m_fmcount.setNow(*ar);
        if (m_fmcount.inrange())  //*
            defectN++;
    }
    //m_fmcount.setNow(thresh);
    m_fmthresh=thresh;
    if (m_fmthresh.now()>m_fmthresh.down() && defectN>0)   //*
    {
        senderror(4);
        cout<<"异物:NG"<<endl;
        return false;
    }
    cout<<"异物:OK"<<endl;
    sendsuccess(0);
    return true;
}

bool ledclassify::ledfront(Mat &mat,float scale)
{
    cout<<"front"<<endl;
    mytime timer;
    timer.TimerStart();
    if(mat.empty()){
        return false;
    }
    bool res=calfront(mat,scale);
    cout<<"time: "<<timer.TimerFinish()<<endl;
    return res;
}
