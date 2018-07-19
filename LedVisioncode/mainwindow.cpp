#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setup();
}
void MainWindow::setup(){
    ui->setupUi(this);
    if(m_model){
        ui->label->setText(QString::fromLocal8Bit("程序二"));
    }
    //ui->pushButton_run->setDown(true);
//    ui->pushButton_2->setEnabled(false);
//    ui->pushButton_3->setEnabled(false);
    memset(m_errorcount,0,10*sizeof(int));
    QFile ExpandData(m_morenfile);
    if(ExpandData.exists()&&ExpandData.open(QIODevice::ReadWrite))
    {
        QTextStream in(&ExpandData);
        ledc.ledload(in);
        ExpandData.close();
    }
    initform();
    connect(&ledc,SIGNAL(senderror(int)),this,SLOT(RecClassifyError(int)));
    connect(&ledc,SIGNAL(sendsuccess(int)),this,SLOT(RecClassifySuccess(int)));
    connect(&ledcam,SIGNAL(sendupdateui(bool,int)),this,SLOT(Recupdateui(bool,int)));
}

void MainWindow::savecam(QTextStream &out)
{
    out<<m_isignalsource1;
    out<<' ';
    out<<QString::fromStdString(m_scamname1);
    out<<' ';
    out<<m_isignalsource2;
    out<<' ';
    out<<QString::fromStdString(m_scamname2);
    out<<' ';

}

void MainWindow::loadcam(QTextStream &in)
{
    QString str1,str2;
    in>>m_isignalsource1;
    in>>str1;
    in>>m_isignalsource2;
    in>>str2;
    m_scamname1=str1.toStdString();
    m_scamname2=str2.toStdString();
}

void MainWindow::Recupdateui(bool i, int j)
{
    showres(i,j);
    switch (j) {
    case 0:
        UpdateGUI(ui->lblOriginal,&ledcam.m_workmat1);
        break;
    case 1:
        UpdateGUI(ui->lblOriginal_2,&ledcam.m_workmat2);
        break;
    default:
        break;
    }
}

MainWindow::MainWindow( bool model,QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_model(model)
{
    //句柄不为NULL时意味着已经打开了一个LedVision软件，运用辅助模式
    if(m_model){
       m_morenfile="moren2.zv";
       //
       cout<<m_morenfile.toStdString()<<endl;
    }
    setup();
}


MainWindow::~MainWindow()
{
    ledcam.camuninit(0);
    ledcam.camuninit(1);
    if(conthread!=NULL){
        delete conthread;
    }
    delete ui;
}
void MainWindow::initform()
{
    this->location = this->geometry();
    this->setProperty("form", true);
    this->setProperty("canMove", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    ui->mainToolBar->hide();
    QString qssFile=":/qss/silvery.css";
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
    showparam();
}
void MainWindow::showparam(){
    //TODO:把ledclassify的参数更新到界面
    UpdateTextedit(ui->textEdit_width_down,ui->textEdit_width_up,ledc.m_width);
    UpdateTextedit(ui->textEdit_high_down,ui->textEdit_high_up,ledc.m_hight);
    UpdateTextedit(ui->textEdit_ratio_down,ui->textEdit_ratio_up,ledc.m_ratio);
    UpdateTextedit(ui->textEdit_maxvalue_down,ui->textEdit_maxvalue_up,ledc.m_gradmax);
    UpdateTextedit(ui->textEdit_minvalue_down,ui->textEdit_minvalue_up,ledc.m_gradmin);
    UpdateTextedit(ui->textEdit_maxwidth_down,ui->textEdit_maxwidth_up,ledc.m_maxlength);
    UpdateTextedit(ui->textEdit_minwidth_down,ui->textEdit_minwidth_up,ledc.m_minlength);
    UpdateTextedit(ui->textEdit_offset_down,ui->textEdit_offset_up,ledc.m_offset);
    UpdateTextedit(ui->textEdit_radius_down,ui->textEdit_radius_up,ledc.m_radius);
    UpdateTextedit(ui->textEdit_farea_down,ui->textEdit_farea_up,ledc.m_emptyratio);
    UpdateTextedit(ui->textEdit_fhigh_down,ui->textEdit_fhigh_up,ledc.m_footratio);
    UpdateTextedit(ui->textEdit_fsa_down,ui->textEdit_fsa_up,ledc.m_emptysamilar);
    UpdateTextedit(ui->textEdit_fsh_down,ui->textEdit_fsh_up,ledc.m_highsamilar);
    UpdateTextedit(ui->textEdit_fmgray_down,ui->textEdit_fmgray_up,ledc.m_fmthresh);
    UpdateTextedit(ui->textEdit_fmsize_down,ui->textEdit_fmsize_up,ledc.m_fmcount);
}
void MainWindow::UpdateTextedit(QTextEdit* editdown,QTextEdit *editup,thresholdparam thp){
    editdown->setText(QString::number(thp.down()));
    editup->setText(QString::number(thp.up()));

}
void MainWindow::setparam(){
    //TODO:把界面参数更新到ledclassify
    setTextedit(ui->textEdit_width_down,ui->textEdit_width_up,ledc.m_width);
    setTextedit(ui->textEdit_high_down,ui->textEdit_high_up,ledc.m_hight);
    setTextedit(ui->textEdit_ratio_down,ui->textEdit_ratio_up,ledc.m_ratio);
    setTextedit(ui->textEdit_maxvalue_down,ui->textEdit_maxvalue_up,ledc.m_gradmax);
    setTextedit(ui->textEdit_minvalue_down,ui->textEdit_minvalue_up,ledc.m_gradmin);
    setTextedit(ui->textEdit_maxwidth_down,ui->textEdit_maxwidth_up,ledc.m_maxlength);
    setTextedit(ui->textEdit_minwidth_down,ui->textEdit_minwidth_up,ledc.m_minlength);
    setTextedit(ui->textEdit_offset_down,ui->textEdit_offset_up,ledc.m_offset);
    setTextedit(ui->textEdit_radius_down,ui->textEdit_radius_up,ledc.m_radius);
    setTextedit(ui->textEdit_farea_down,ui->textEdit_farea_up,ledc.m_emptyratio);
    setTextedit(ui->textEdit_fhigh_down,ui->textEdit_fhigh_up,ledc.m_footratio);
    setTextedit(ui->textEdit_fsa_down,ui->textEdit_fsa_up,ledc.m_emptysamilar);
    setTextedit(ui->textEdit_fsh_down,ui->textEdit_fsh_up,ledc.m_highsamilar);
    setTextedit(ui->textEdit_fmgray_down,ui->textEdit_fmgray_up,ledc.m_fmthresh);
    setTextedit(ui->textEdit_fmsize_down,ui->textEdit_fmsize_up,ledc.m_fmcount);
}
void MainWindow::setTextedit(QTextEdit* editdown,QTextEdit *editup,thresholdparam &thp){
    thp.setdownup(editdown->toPlainText().toFloat(),editup->toPlainText().toFloat());
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


void MainWindow::showres(bool i,int j){
    switch (j) {
    case 0:
        if(i){
            ui->label_result->setText("OK");
        }else{
            ui->label_result->setText("NG");
        }
        break;
    case 1:
        if(i){
            ui->label_result_2->setText("OK");
        }else{
            ui->label_result_2->setText("NG");
        }
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
        if(ledcam.caminit(m_scamname1,0,3)&&ledcam.caminit(m_scamname2,1,3)){
            ui->pushButton_run->setText(QString::fromLocal8Bit("运行中"));
        }else{
            ui->pushButton_run->setChecked(false);
        }
    }else{
        if(ledcam.camuninit(0)&&ledcam.camuninit(1)){
            ui->pushButton_run->setText(QString::fromLocal8Bit("开始运行"));
        }else{
            //ui->pushButton_run->setChecked(true);
        }
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
        showres(ledc.ledback(m_fileimg),1);

    }else if(ui->radioButton_2->isChecked()){
        cout<<"工位1"<<endl;
        UpdateGUI(ui->lblOriginal_2,&m_fileimg);
        showres(ledc.ledfront(m_fileimg),0);
    }
}



void MainWindow::on_pushButton_setimg_clicked()
{
    if(ui->radioButton->isChecked()){
        cout<<"工位2"<<endl;
        ledcam.imgsetting(m_isignalsource2,ledcam.isignal1camnode());

    }else if(ui->radioButton_2->isChecked()){
        cout<<"工位1"<<endl;
        ledcam.imgsetting(m_isignalsource1,ledcam.isignal2camnode());
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

void MainWindow::RecClassifyError(int errortype)
{
    switch (errortype) {
    case errornoled:
        ui->label_error->setText(QString::fromLocal8Bit("图像中没有led"));
        break;
    case errorothersth:
        ui->label_error->setText(QString::fromLocal8Bit("有其他异物"));
        break;
    case errorbroken:
        ui->label_error->setText(QString::fromLocal8Bit("led压烂"));
        break;
    case errorglue:
        ui->label_error->setText(QString::fromLocal8Bit("胶量不正常"));
        break;
    case errorfmater:
        ui->label_error->setText(QString::fromLocal8Bit("led有异物"));
        break;
    case errorfoot:
        ui->label_error_2->setText(QString::fromLocal8Bit("led背面引脚有问题"));
        break;
    case errorotherback:
        ui->label_error_2->setText(QString::fromLocal8Bit("led背面有缺陷，问题未知"));
        break;
    case errorotherfront:
        ui->label_error->setText(QString::fromLocal8Bit("led正面有缺陷，问题未知"));
        break;
    case returndeal:
        ui->label_error->setText(QString::fromLocal8Bit("led需要返回重新识别"));
        break;
    default:
        break;
    }
}

void MainWindow::RecClassifySuccess(int i)
{
    switch (i) {
    case 0:
        ui->label_error->setText(QString::fromLocal8Bit("处于工位1检测\nled正面正常"));
        break;
    case 1:
        ui->label_error_2->setText(QString::fromLocal8Bit("处于工位2检测\nled背面正常"));
        break;
    default:
        break;
    }

}
void MainWindow::on_pushButton_takeimg_clicked()
{
    //TODO:添加单拍代码
    //文件夹形式：使用列表选中的图片运行
    //相机形式：软件触发拍照
    if(ui->radioButton->isChecked()){
        cout<<"工位2"<<endl;
        switch (m_isignalsource1) {
        case 0:
            //获取列表图像 运行结果
            //工位2检测背面
            UpdateGUI(ui->lblOriginal,&m_fileimg);

            showres(ledc.ledback(m_fileimg),1);
            break;
        case 1:
            //软触发
            cout<<"cam1"<<endl;
            ledcam.camuninit(0);
            ledcam.caminit(m_scamname1,0,1);
            ledcam.softtrigger(0);
            //CameraSoftTrigger(m_hCamera[m_isignalsource1]);
            break;

        }
    }else if(ui->radioButton_2->isChecked()){
        cout<<"工位1"<<endl;
        switch (m_isignalsource2) {
        case 0:
            //获取列表图像 运行结果
            //工位1检测前面
            UpdateGUI(ui->lblOriginal_2,&m_fileimg);
            showres(ledc.ledfront(m_fileimg),0);
            break;
        case 1:
            //软触发
            cout<<"cam2"<<endl;
            ledcam.camuninit(1);
            ledcam.caminit(m_scamname2,1,1);
            ledcam.softtrigger(1);
            //CameraSoftTrigger(m_hCamera[m_isignalsource1]);
            break;

        }
    }
}

void MainWindow::on_pushButton_save_clicked()
{

    QString filename = QFileDialog::getSaveFileName(this,
            tr("Save"),"",tr("*.zv")); //选择路径
    QFile ExpandData(filename);
    if(ExpandData.open(QIODevice::WriteOnly))
    {
        /*文本输出流，用于保存数据*/
        QTextStream out(&ExpandData);

        ledc.ledsave(out);
        savecam(out);
        ExpandData.close();
    }
}

void MainWindow::on_pushButton_clicked()
{
    setparam();
}

void MainWindow::on_pushButton_load_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
            tr("Load"),"",tr("*.zv")); //选择路径
    cout<<filename.toStdString()<<endl;
    QFile ExpandData(filename);
    if(ExpandData.open(QIODevice::ReadWrite))
    {
        /*文本输出流，用于保存数据*/

        QTextStream in(&ExpandData);
        ledc.ledload(in);
        loadcam(in);
        ExpandData.close();
    }
    showparam();
    if(m_isignalsource1==1&&m_isignalsource2==1){
//        ledcam.camuninit(0);
//        ledcam.caminit(m_scamname1,0,1);
//        ledcam.camuninit(1);
//        ledcam.caminit(m_scamname2,1,1);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox::StandardButton res=QMessageBox::question(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("作为默认参数"));
    cout<<res<<QMessageBox::Ok<<endl;
    if(res!=16384){
        return;
    }
    QFile ExpandData(m_morenfile);
    if(ExpandData.open(QIODevice::WriteOnly))
    {
        /*文本输出流，用于保存数据*/
        QTextStream out(&ExpandData);
        ledc.ledsave(out);
        ExpandData.close();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::StandardButton res=QMessageBox::question(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("确认开始标定?\n如果确定，请放置一个工件于转动盘\n期间请勿触碰，标定结束会提醒"));
    if(res!=16384){
        return;
    }
    //初始化相机
    if(m_isignalsource1==1&&m_isignalsource2==1){
        //TODO:开启两个图像线程

        if(conthread==NULL){
            ledcam.camuninit(0);
            ledcam.camuninit(1);
            conthread=new controlthread(m_scamname1,m_scamname2);
            connect(conthread,SIGNAL(sendui(bool,int)),this,SLOT(Recupdateui(bool,int)));
            conthread->startthread1();
            conthread->startthread2();
        }
    }else{
        QMessageBox::information(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("存在信号源不是相机模式"));
    }
}
