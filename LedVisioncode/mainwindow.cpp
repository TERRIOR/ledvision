#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->pushButton_2->setEnabled(false);
//    ui->pushButton_3->setEnabled(false);
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
    ui->tableWidgetfile->setColumnCount(1);
    QStringList header;
    header<<tr("FileName");
    ui->tableWidgetfile->setHorizontalHeaderLabels(header);
    ui->tableWidgetfile->horizontalHeader()->setDefaultSectionSize(220);
    ui->tableWidgetfile->setFrameShape(QFrame::NoFrame); //设置无边框
    ui->tableWidgetfile->setShowGrid(true); //显示格子线
    ui->tableWidgetfile->verticalHeader()->setVisible(false); //设置垂直头不可见
    ui->tableWidgetfile->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableWidgetfile->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
    cout<<"callback0"<<" first step"<<endl;
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
        pThis->m_workmat1=Mat(g_iplImage1);

        if(!pThis->m_workmat1.empty())

            pThis->showres(pThis->m_ledc.ledfront(pThis->m_workmat1));

            //imshow("cam1",matcam1);
        waitKey(1);
    }

    memcpy(&pThis->m_sFrInfo[0],pFrameHead,sizeof(tSdkFrameHead));

}

void __stdcall GrabImageCallback1(CameraHandle hCamera, BYTE *pFrameBuffer, tSdkFrameHead* pFrameHead,PVOID pContext)
{
    cout<<"callback1"<<" second step"<<endl;
    CameraSdkStatus status;
    IplImage *g_iplImage2 = NULL;
    MainWindow *pThis = (MainWindow*)pContext;


    //TODO:添加判断逻辑代码，一工位检测是坏的led就不需要进行第二工位的检测了，虽然检测了也没关系，再商量
//    if(){
//        return;
//    }
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
        pThis->m_workmat2=Mat(g_iplImage2);
        if(!pThis->m_workmat2.empty())
            pThis->showres(pThis->m_ledc.ledfront(pThis->m_workmat2));
            //imshow("cam2",matcam2);
        waitKey(1);
    }

    memcpy(&pThis->m_sFrInfo[1],pFrameHead,sizeof(tSdkFrameHead));

}
void MainWindow::showres(bool i){
    if(i){
        ui->label_result->setText("OK");
    }else{
        ui->label_result->setText("NG");
    }
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

void MainWindow::refreshpiclist()
{
    ui->tableWidgetfile->clearContents();
    ui->tableWidgetfile->setRowCount(0);
    int size=m_qstrlist.size();
    int i;
    //first save the num of tools
    for(i=0;i<size;i++){
        int row_count = ui->tableWidgetfile->rowCount(); //获取表单行数
        ui->tableWidgetfile->insertRow(row_count); //插入新行
        QTableWidgetItem *item = new QTableWidgetItem();

        //设置对应的图标、文件名称、最后更新时间、对应的类型、文件大小
        item->setText(m_qstrlist.at(row_count));
        ui->tableWidgetfile->setItem(row_count, 0, item);

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


void MainWindow::on_pushButton_run_toggled(bool checked)
{
    if(checked){
        //打开定时器定时更新

        ui->pushButton_run->setText(QString::fromLocal8Bit("运行中"));

    }else{

        ui->pushButton_run->setText(QString::fromLocal8Bit("开始运行"));

    }
}


void MainWindow::on_pushButton_file_clicked()
{
    //TODO:添加获取图片文件名，并刷新列表，显示图片名称，并保存当前地址
    m_filename = QFileDialog::getExistingDirectory(this,
            tr("Load"),""); //选择路径
    cout<<m_filename.toStdString()<<endl;
    m_qstrlist=getFileNames(m_filename);
    cout<<m_qstrlist.size()<<endl;
    refreshpiclist();
    //mat=imread(m_filename.toStdString()+string((const char *)m_qstrlist.at(m_index).toLocal8Bit()));
//    QFile ExpandData(filename);
//    if(ExpandData.open(QIODevice::ReadWrite))
//    {
//        /*文本输出流，用于保存数据*/
//        int i;
//        int size;
//        QTextStream in(&ExpandData);
//        in>>size;
//        for(i=0;i<size;i++){
//            loadtool(in);
//        }
//        ExpandData.close();
//    }
}

void MainWindow::on_tableWidgetfile_cellDoubleClicked(int row, int column)
{
    m_fileimgnode=row;
    cout<<string((const char *)m_qstrlist.at(row).toLocal8Bit())<<endl;
    m_fileimg=imread(m_filename.toStdString()+"/"+string((const char *)m_qstrlist.at(row).toLocal8Bit()));
    //imshow("c",m_fileimg);
    if(ui->radioButton->isChecked()){
        cout<<"工位2"<<endl;
        UpdateGUI(ui->lblOriginal,&m_fileimg);
        showres(m_ledc.ledback(m_fileimg));

    }else if(ui->radioButton_2->isChecked()){
        cout<<"工位1"<<endl;
        UpdateGUI(ui->lblOriginal_2,&m_fileimg);
        showres(m_ledc.ledfront(m_fileimg));
    }
}

void MainWindow::imgsetting(int i,int j)
{
    switch (i) {
    case 0:

        break;
    case 1:
        char str[]="Setting";
        CameraCreateSettingPage(m_hCamera[j],NULL,str,NULL,NULL,0);//"通知SDK内部建该相机的属性页面";
        CameraShowSettingPage(m_hCamera[j],TRUE);//TRUE显示相机配置界面。FALSE则隐藏。
        break;

    }
}

void MainWindow::on_pushButton_setimg_clicked()
{
    if(ui->radioButton->isChecked()){
        cout<<"工位2"<<endl;
        imgsetting(m_isignalsource2,m_isignal2camnode);

    }else if(ui->radioButton_2->isChecked()){
        cout<<"工位1"<<endl;
        imgsetting(m_isignalsource1,m_isignal1camnode);
    }
}

void MainWindow::on_pushButton_saveimg_clicked()
{
    //TODO:创建文件夹，把图片保存
    if(ui->radioButton->isChecked()){
        cout<<"工位2"<<endl;
    }else if(ui->radioButton_2->isChecked()){
        cout<<"工位1"<<endl;
    }
}

void MainWindow::UpdateGUI(QLabel *ql,Mat *imgshow)
{
    //更新显示
    //refreshmat();//test
    Mat temp;
    if(imgshow!=NULL&&!imgshow->empty()){//先判断指针
        imgshow->copyTo(temp);
        if(!temp.empty()){
            float scale,scale1,scale2;
            int matrows=temp.rows;
            int lablerows=ql->height();
            int matcols=temp.cols;
            int lablecols=ql->width();

            scale1=(float)lablerows/matrows;
            scale2=(float)lablecols/matcols;
            if(scale1<scale2){
                scale=scale1;
            }else{
                scale=scale2;
            }
            scalemat(temp,scale);
            QImage qimg=Mat2QImage(temp);
            ql->setPixmap(QPixmap::fromImage(qimg));
        }

    }

}
void MainWindow::on_pushButton_takeimg_clicked()
{
    //TODO:添加单拍代码
    //文件夹形式：使用列表选中的图片运行
    //相机形式：软件触发拍照
    if(ui->radioButton->isChecked()){
        cout<<"工位2"<<endl;
        switch (m_isignal1camnode) {
        case 0:
            //获取列表图像 运行结果
            //工位2检测背面
            UpdateGUI(ui->lblOriginal,&m_fileimg);
            showres(m_ledc.ledback(m_fileimg));
            break;
        case 1:
            //软触发
            CameraSoftTrigger(m_hCamera[m_isignalsource1]);
            break;

        }
    }else if(ui->radioButton_2->isChecked()){
        cout<<"工位1"<<endl;
        switch (m_isignal1camnode) {
        case 0:
            //获取列表图像 运行结果
            //工位1检测前面
            UpdateGUI(ui->lblOriginal_2,&m_fileimg);
            showres(m_ledc.ledfront(m_fileimg));
            break;
        case 1:
            //软触发
            CameraSoftTrigger(m_hCamera[m_isignalsource1]);
            break;

        }
    }
}

void MainWindow::on_pushButton_save_clicked()
{

}
