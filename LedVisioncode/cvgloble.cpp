#include "cvgloble.h"

void scalemat(Mat &mat,float scale){
    resize(mat,mat,Size(mat.cols*scale,mat.rows*scale),0,0,INTER_LINEAR);

}
void getimgroi(Mat& mat,const float scale){
    int cols=mat.cols;
    int rows=mat.rows;
    int offsety=(rows-scale*rows)/2;
    int offsetx=(cols-scale*cols)/2;
    mat=mat(Rect(offsetx,offsety,cols*scale,rows*scale));
}
void imrotate(Mat& img, Mat& newIm, double angle,Point2f pt,Size size){
    Mat r = getRotationMatrix2D(pt,angle,1.0);
    warpAffine(img,newIm,r,Size(img.cols,img.rows));
}
double Entropy(Mat img)
{
    // 将输入的矩阵为图像
    double temp[256];
    // 清零
    for(int i=0;i<256;i++)
    {
        temp[i] = 0.0;
    }

    // 计算每个像素的累积值
    for(int m=0;m<img.rows;m++)
    {// 有效访问行列的方式
        const uchar* t = img.ptr<uchar>(m);
        for(int n=0;n<img.cols;n++)
        {
            int i = t[n];
            temp[i] = temp[i]+1;
        }
    }

    // 计算每个像素的概率
    for(int i=0;i<256;i++)
    {
    temp[i] = temp[i]/(img.rows*img.cols);
    }

    double result = 0;
    // 根据定义计算图像熵
    for(int i =0;i<256;i++)
    {
    if(temp[i]==0.0)
        result = result;
    else
        result = result-temp[i]*(log(temp[i])/log(2.0));
    }
    return result;
}
double getfuza(Mat &img){

    int height = img.rows;
    int width = img.cols;
    double mean = 0.0;
    double variance = 0.0;
    int temp = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            mean += img.at<cv::Vec3b>(y, x)[0];
        }
    }
    mean /= double(height*width);

    //variance(texture complexity纹理复杂度)
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            temp = img.at<cv::Vec3b>(y, x)[0] - mean;
            variance += (temp*temp);
        }
    }
    variance /= double(height*width);
    variance = int(variance / 100);
    std::cout << "variance:" << variance;
    return variance;
}
void imshowMany(const std::string& _winName, const vector<Mat>& _imgs)
{
    int nImg = (int)_imgs.size();

    Mat dispImg;

    int size;
    int x, y;

    // w - Maximum number of images in a row
    // h - Maximum number of images in a column
    int w, h;
    // scale - How much we have to resize the image
    float scale;
    int max;

    if (nImg <= 0)
    {
        printf("Number of arguments too small....\n");
        return;
    }
    else if (nImg > 12)
    {
        printf("Number of arguments too large....\n");
        return;
    }

    else if (nImg == 1)
    {
        w = h = 1;
        size = 300;
    }
    else if (nImg == 2)
    {
        w = 2; h = 1;
        size = 300;
    }
    else if (nImg == 3 || nImg == 4)
    {
        w = 2; h = 2;
        size = 300;
    }
    else if (nImg == 5 || nImg == 6)
    {
        w = 3; h = 2;
        size = 200;
    }
    else if (nImg == 7 || nImg == 8)
    {
        w = 4; h = 2;
        size = 200;
    }
    else
    {
        w = 4; h = 3;
        size = 150;
    }

    dispImg.create(Size(100 + size*w, 60 + size*h), 0);

    for (int i= 0, m=20, n=20; i<nImg; i++, m+=(20+size))
    {
        x = _imgs[i].cols;
        y = _imgs[i].rows;

        max = (x > y)? x: y;
        scale = (float) ( (float) max / size );

        if (i%w==0 && m!=20)
        {
            m = 20;
            n += 20+size;
        }

        Mat imgROI = dispImg(Rect(m, n, (int)(x/scale), (int)(y/scale)));
        resize(_imgs[i], imgROI, Size((int)(x/scale), (int)(y/scale)));
    }

    namedWindow(_winName);
    imshow(_winName, dispImg);
}

QImage Mat2QImage(Mat image1)
{
    QImage img;
    //image1=image;
    if (image1.channels()==3) {
        cvtColor(image1, image1, CV_BGR2RGB);
        img = QImage((const unsigned char *)(image1.data), image1.cols, image1.rows,
                image1.cols*image1.channels(), QImage::Format_RGB888);
    } else if (image1.channels()==1) {
        img = QImage((const unsigned char *)(image1.data), image1.cols, image1.rows,
                image1.cols*image1.channels(), QImage::Format_Indexed8);//灰度图
    } else {
        img = QImage((const unsigned char *)(image1.data), image1.cols, image1.rows,
                image1.cols*image1.channels(), QImage::Format_RGB888);
    }

    return img;
}
