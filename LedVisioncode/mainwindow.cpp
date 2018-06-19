#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initform();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initform()
{
    this->location = this->geometry();
    this->setProperty("form", true);
    this->setProperty("canMove", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    ui->mainToolBar->hide();
    QString qssFile=":/qss/flatwhite.css";
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = file.readAll();
        QString paletteColor = qss.mid(20, 7);
        this->setPalette(QPalette(QColor(paletteColor)));
        this->setStyleSheet(qss);
        file.close();
    }
//    ui->tableWidgettool->setColumnCount(3);
//    QStringList header;
//    header<<tr("Name")<<tr("Index")<<tr("Id");
//    ui->tableWidgettool->setHorizontalHeaderLabels(header);
//    ui->tableWidgettool->horizontalHeader()->setDefaultSectionSize(220/3);
//    ui->tableWidgettool->setFrameShape(QFrame::NoFrame); //设置无边框
//    ui->tableWidgettool->setShowGrid(true); //显示格子线
//    ui->tableWidgettool->verticalHeader()->setVisible(false); //设置垂直头不可见
//    ui->tableWidgettool->setSelectionMode(QAbstractItemView::ExtendedSelection);
//    ui->tableWidgettool->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->tableWidgettool->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    showMaximized();
}

void MainWindow::on_pushButton_min_clicked()
{
    showMinimized();
}
void MainWindow::on_pushButton_max_clicked()
{
    if (max) {
        this->setGeometry(location);
        this->setProperty("canMove", true);
    } else {
        location = this->geometry();
        //ui->centralWidget->showFullScreen();
        this->setGeometry(QApplication::desktop()->availableGeometry());
        this->setProperty("canMove", false);
    }

    max = !max;
}

void MainWindow::on_pushButton_close_clicked()
{
    close();
}

void MainWindow::on_pushButton_signal_clicked()
{
    signalDialog* diasignal=new signalDialog(m_isignalsource1,m_isignalsource2);
    //diasignal->setwhere();
    connect(diasignal,SIGNAL(sendsignalset(int,int,string,string)),this,SLOT(receivesignal(int,int,string,string)));
    diasignal->show();
}

void __stdcall GrabImageCallback0(CameraHandle hCamera, BYTE *pFrameBuffer, tSdkFrameHead* pFrameHead,PVOID pContext)
{
    cout<<"callback0"<<endl;
    CameraSdkStatus status;
    IplImage *g_iplImage1 = NULL;
    MainWindow *pThis = (MainWindow*)pContext;
    //将获得的原始数据转换成RGB格式的数据，同时经过ISP模块，对图像进行降噪，边沿提升，颜色校正等处理。
    //我公司大部分型号的相机，原始数据都是Bayer格式的

    status = CameraImageProcess(hCamera, pFrameBuffer, pThis->m_pFrameBuffer[0],pFrameHead);

    //分辨率改变了，则刷新背景
    if (pThis->m_sFrInfo[0].iWidth != pFrameHead->iWidth || pThis->m_sFrInfo[0].iHeight != pFrameHead->iHeight)
    {
        pThis->m_sFrInfo[0].iWidth = pFrameHead->iWidth;
        pThis->m_sFrInfo[0].iHeight = pFrameHead->iHeight;
    }

    if(status == CAMERA_STATUS_SUCCESS )
    {
        //调用SDK封装好的显示接口来显示图像,您也可以将m_pFrameBuffer中的RGB数据通过其他方式显示，比如directX,OpengGL,等方式。
        CameraImageOverlay(hCamera, pThis->m_pFrameBuffer[0],pFrameHead);
        if (g_iplImage1)
        {
            cvReleaseImageHeader(&g_iplImage1);
        }
        g_iplImage1 = cvCreateImageHeader(cvSize(pFrameHead->iWidth,pFrameHead->iHeight),IPL_DEPTH_8U,pThis->m_sFrInfo[0].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3);
        cvSetData(g_iplImage1,pThis->m_pFrameBuffer[0],pFrameHead->iWidth*(pThis->m_sFrInfo[0].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3));
        Mat matcam1=Mat(g_iplImage1);

        if(!matcam1.empty())
            imshow("cam1",matcam1);
        waitKey(1);
    }

    memcpy(&pThis->m_sFrInfo[0],pFrameHead,sizeof(tSdkFrameHead));

}

void __stdcall GrabImageCallback1(CameraHandle hCamera, BYTE *pFrameBuffer, tSdkFrameHead* pFrameHead,PVOID pContext)
{
    cout<<"callback1"<<endl;
    CameraSdkStatus status;
    IplImage *g_iplImage2 = NULL;
    MainWindow *pThis = (MainWindow*)pContext;
    //将获得的原始数据转换成RGB格式的数据，同时经过ISP模块，对图像进行降噪，边沿提升，颜色校正等处理。
    //我公司大部分型号的相机，原始数据都是Bayer格式的

    status = CameraImageProcess(hCamera, pFrameBuffer, pThis->m_pFrameBuffer[1],pFrameHead);

    //分辨率改变了，则刷新背景
    if (pThis->m_sFrInfo[1].iWidth != pFrameHead->iWidth || pThis->m_sFrInfo[1].iHeight != pFrameHead->iHeight)
    {
        pThis->m_sFrInfo[1].iWidth = pFrameHead->iWidth;
        pThis->m_sFrInfo[1].iHeight = pFrameHead->iHeight;
    }

    if(status == CAMERA_STATUS_SUCCESS )
    {
        //调用SDK封装好的显示接口来显示图像,您也可以将m_pFrameBuffer中的RGB数据通过其他方式显示，比如directX,OpengGL,等方式。
        CameraImageOverlay(hCamera, pThis->m_pFrameBuffer[1],pFrameHead);
        if (g_iplImage2)
        {
            cvReleaseImageHeader(&g_iplImage2);
        }
        g_iplImage2 = cvCreateImageHeader(cvSize(pFrameHead->iWidth,pFrameHead->iHeight),IPL_DEPTH_8U,pThis->m_sFrInfo[1].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3);
        cvSetData(g_iplImage2,pThis->m_pFrameBuffer[1],pFrameHead->iWidth*(pThis->m_sFrInfo[1].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3));
        Mat matcam2=Mat(g_iplImage2);
        if(!matcam2.empty())
            imshow("cam2",matcam2);
        waitKey(1);
    }

    memcpy(&pThis->m_sFrInfo[1],pFrameHead,sizeof(tSdkFrameHead));

}
bool MainWindow::caminit(string str,int signalnode){
    int                     iCameraCounts = 4;
    int                     iStatus=-1;
    tSdkCameraDevInfo       tCameraEnumList[2];
    tSdkCameraCapbility     g_tCapability;
    switch(signalnode){
        case 0:
            if(m_bsignal1inited)
                return false;
        break;
        case 1:
            if(m_bsignal2inited)
                return false;
        break;
    }
    CameraEnumerateDevice(tCameraEnumList,&iCameraCounts);
    if(iCameraCounts==0){
        //TODO:add warn
        return false;
    }
    int i;
    for(i=0;i<iCameraCounts;i++){
        string strtemp=tCameraEnumList[i].acFriendlyName;
        if(str==strtemp){
           iStatus = CameraInit(&tCameraEnumList[i],-1,-1,&m_hCamera[i]);
           if(iStatus!=CAMERA_STATUS_SUCCESS){
               cout<< CameraGetErrorString(iStatus)<<endl;
               return false;
           }
           CameraGetCapability(m_hCamera[i],&g_tCapability);

           m_pFrameBuffer[i] = (BYTE *)CameraAlignMalloc(g_tCapability.sResolutionRange.iWidthMax*g_tCapability.sResolutionRange.iHeightMax*3,16);
           CameraSetTriggerMode(m_hCamera[i],1);
           /*让SDK进入工作模式，开始接收来自相机发送的图像
           数据。如果当前相机是触发模式，则需要接收到
           触发帧以后才会更新图像。    */
           CameraPlay(m_hCamera[i]);
           switch (signalnode) {
           case 0:
               //与信号1的回调函数绑定
               m_isignal1camnode=i;
               m_bsignal1inited=true;
               CameraSetCallbackFunction(m_hCamera[i],GrabImageCallback0,(PVOID)this,NULL);//"设置图像抓取的回调函数";
               break;
           case 1:
               //与信号2的回调函数绑定
               m_isignal2camnode=i;
               m_bsignal2inited=true;
               CameraSetCallbackFunction(m_hCamera[i],GrabImageCallback1,(PVOID)this,NULL);//"设置图像抓取的回调函数";
               break;
           default:
               break;
           }
        }
    }
}

void MainWindow::camuninit(int i){

    switch (i) {
    case 0:
        CameraUnInit(m_hCamera[m_isignal1camnode]);
        break;
    case 1:
        CameraUnInit(m_hCamera[m_isignal2camnode]);
        break;
    default:
        break;
    }
}
void MainWindow::receivesignal(int i,int j,string m,string n)
{

    m_isignalsource1=i;
    m_isignalsource2=j;
    m_scamname1=m;
    m_scamname2=n;
    string str1="signal1 is ";

    switch (m_isignalsource1) {
    case 0:
        str1+="file";
        break;
    case 1:
        str1+="cam";

        break;
    default:
        break;
    }
    str1+="\nsignal2 is ";
    switch (m_isignalsource2) {
    case 0:
        str1+="file";
        break;
    case 1:
        str1+="cam";
        break;
    default:
        break;
    }
    QMessageBox::information(this,tr("succeed"),QString::fromStdString(str1));
}

//bool MainWindow::InitCamera()
//{
//    int                     iCameraCounts = 2;
//    int                     iStatus=-1;
//    tSdkCameraDevInfo       tCameraEnumList[2];
//    tSdkCameraCapbility     g_tCapability;
//    //sdk初始化  0 English 1中文
//    CameraSdkInit(1);

//    //枚举设备，并建立设备列表
//    CameraEnumerateDevice(tCameraEnumList,&iCameraCounts);

//    //没有连接设备
//    if(iCameraCounts==0){
//        return false;
//    }
//    cout<<iCameraCounts<<endl;
//    int i;
//    for(i=0;i<iCameraCounts;i++){
//        //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
//        iStatus = CameraInit(&tCameraEnumList[i],-1,-1,&m_hCamera[i]);

//        //初始化失败
//        if(iStatus!=CAMERA_STATUS_SUCCESS){
//            cout<< CameraGetErrorString(iStatus)<<endl;
//            return false;
//        }
//        //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
//        CameraGetCapability(m_hCamera[i],&g_tCapability);

//        m_pFrameBuffer[i] = (BYTE *)CameraAlignMalloc(g_tCapability.sResolutionRange.iWidthMax*g_tCapability.sResolutionRange.iHeightMax*3,16);
//        CameraSetTriggerMode(m_hCamera[i],1);
//        /*让SDK进入工作模式，开始接收来自相机发送的图像
//        数据。如果当前相机是触发模式，则需要接收到
//        触发帧以后才会更新图像。    */
//        CameraPlay(m_hCamera[i]);

//        switch (i) {
//        case 0:
//            CameraSetCallbackFunction(m_hCamera[i],GrabImageCallback0,(PVOID)this,NULL);//"设置图像抓取的回调函数";
//            break;
//        case 1:
//            CameraSetCallbackFunction(m_hCamera[i],GrabImageCallback1,(PVOID)this,NULL);//"设置图像抓取的回调函数";
//            break;
//        default:
//            break;
//        }
//    }

//    return true;
//}

