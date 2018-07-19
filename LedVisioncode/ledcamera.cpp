#include "ledcamera.h"
ledcamera ledcam;
void __stdcall GrabImageCallback0(CameraHandle hCamera, BYTE *pFrameBuffer, tSdkFrameHead* pFrameHead,PVOID pContext)
{
    cout<<"callback0"<<" first step"<<endl;
    CameraSdkStatus status;
    //IplImage *g_iplImage1 = NULL;
    ledcamera *pThis = (ledcamera*)pContext;
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
#if 0
        if (g_iplImage1)
        {
            cvReleaseImageHeader(&g_iplImage1);
        }
        g_iplImage1 = cvCreateImageHeader(cvSize(pFrameHead->iWidth,pFrameHead->iHeight),IPL_DEPTH_8U,pThis->m_sFrInfo[0].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3);
        cvSetData(g_iplImage1,pThis->m_pFrameBuffer[0],pFrameHead->iWidth*(pThis->m_sFrInfo[0].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3));
        pThis->m_workmat1=Mat(g_iplImage1);
#endif
        cv::Mat matImage(
        cvSize(pFrameHead->iWidth,pFrameHead->iHeight),
        pFrameHead->uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
        pThis->m_pFrameBuffer[0]);

        if(!matImage.empty())
        {
            bool res;
            res=ledc.ledfront(matImage);
            //复制到相机类的mat
            //imshow("ss",matImage);
            matImage.copyTo(pThis->m_workmat1);
            pThis->sendupdateui(res,0);
        }
    }

    memcpy(&pThis->m_sFrInfo[0],pFrameHead,sizeof(tSdkFrameHead));

}

void __stdcall GrabImageCallback1(CameraHandle hCamera, BYTE *pFrameBuffer, tSdkFrameHead* pFrameHead,PVOID pContext)
{
    cout<<"callback1"<<" second step"<<endl;
    CameraSdkStatus status;
    //IplImage *g_iplImage2 = NULL;
    ledcamera *pThis = (ledcamera*)pContext;


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
#if 0
        if (g_iplImage2)
        {
            cvReleaseImageHeader(&g_iplImage2);
        }
        g_iplImage2 = cvCreateImageHeader(cvSize(pFrameHead->iWidth,pFrameHead->iHeight),IPL_DEPTH_8U,pThis->m_sFrInfo[1].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3);
        cvSetData(g_iplImage2,pThis->m_pFrameBuffer[1],pFrameHead->iWidth*(pThis->m_sFrInfo[1].uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3));
        pThis->m_workmat2=Mat(g_iplImage2);
#endif
        cv::Mat matImage2(
        cvSize(pFrameHead->iWidth,pFrameHead->iHeight),
        pFrameHead->uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
        pThis->m_pFrameBuffer[1]);

        if(!matImage2.empty())
        {
            bool res;
            res=ledc.ledfront(matImage2);
            //复制到相机类的mat
            //imshow("ss",matImage2);
            waitKey();
            matImage2.copyTo(pThis->m_workmat2);
            pThis->sendupdateui(res,0);
        }
    }

    memcpy(&pThis->m_sFrInfo[1],pFrameHead,sizeof(tSdkFrameHead));
    //第一个工位是所有的零件都会经过个的，所以在第一工位记录总数
    //pThis->m_icountall++;
}
ledcamera::ledcamera()
{
    CameraSdkInit(1);
}
bool ledcamera::caminit(string str,int signalnode,int model){
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
        cout<<"no cam"<<endl;
        return false;
    }
    int i;
    for(i=0;i<iCameraCounts;i++){
        string strtemp=tCameraEnumList[i].acFriendlyName;
        cout<<strtemp<<" " <<str<<endl;
        if(str==strtemp){
           iStatus = CameraInit(&tCameraEnumList[i],-1,-1,&m_hCamera[i]);
           if(iStatus!=CAMERA_STATUS_SUCCESS){
               cout<< CameraGetErrorString(iStatus)<<endl;
               return false;
           }
           CameraGetCapability(m_hCamera[i],&g_tCapability);

           m_pFrameBuffer[i] = (BYTE *)CameraAlignMalloc(g_tCapability.sResolutionRange.iWidthMax*g_tCapability.sResolutionRange.iHeightMax*3,16);
           CameraSetTriggerMode(m_hCamera[i],model);
           /*让SDK进入工作模式，开始接收来自相机发送的图像
           数据。如果当前相机是触发模式，则需要接收到
           触发帧以后才会更新图像。    */
           CameraPlay(m_hCamera[i]);
           cout<<"success"<<endl;
           switch (signalnode) {
           case 0:
               //与信号1的回调函数绑定
               m_bsignal1inited=true;
               m_isignal1camnode=i;
               if(model==0){
                   return true;
               }
               CameraSetCallbackFunction(m_hCamera[i],GrabImageCallback0,(PVOID)this,NULL);//"设置图像抓取的回调函数";
               break;
           case 1:
               //与信号2的回调函数绑定
               m_isignal2camnode=i;
               m_bsignal2inited=true;
               if(model==0){
                   return true;
               }
               CameraSetCallbackFunction(m_hCamera[i],GrabImageCallback1,(PVOID)this,NULL);//"设置图像抓取的回调函数";
               break;
           default:
               break;
           }
           return true;
        }
    }
}
bool ledcamera::camuninit(int i){

    switch (i) {
    case 0:
        if(m_bsignal1inited){
            CameraUnInit(m_hCamera[m_isignal1camnode]);
            m_bsignal1inited=false;
        }
        else
            return false;
        break;
    case 1:
        if(m_bsignal2inited){
            CameraUnInit(m_hCamera[m_isignal2camnode]);
            m_bsignal2inited=false;
        }
        else
            return false;
        break;
    default:
        break;
    }
    return true;
}
void ledcamera::imgsetting(int i,int j)
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

int ledcamera::isignal1camnode() const
{
    return m_isignal1camnode;
}

void ledcamera::getimg(int i)
{
    tSdkFrameHead 	sFrameInfo;
    CameraSdkStatus status;
    BYTE*			m_pRawBuffer;
    CameraSdkStatus status2;
    status2=CameraGetImageBuffer(m_hCamera[i],&sFrameInfo,&m_pRawBuffer,1000);
    cout<<status2<<endl;
    if (status2 == CAMERA_STATUS_SUCCESS)
    {
        status=CameraImageProcess(m_hCamera[i],m_pRawBuffer,m_pFrameBuffer[i],&sFrameInfo);
        cout<<status<<endl;
        if (m_sFrInfo[i].iWidth != sFrameInfo.iWidth || m_sFrInfo[i].iHeight != sFrameInfo.iHeight)
        {
            m_sFrInfo[i].iWidth = sFrameInfo.iWidth;
            m_sFrInfo[i].iHeight = sFrameInfo.iHeight;
        }
        if(status == CAMERA_STATUS_SUCCESS)
        {
            //调用SDK封装好的显示接口来显示图像,您也可以将m_pFrameBuffer中的RGB数据通过其他方式显示，比如directX,OpengGL,等方式。
            CameraImageOverlay(m_hCamera[i], m_pFrameBuffer[i], &sFrameInfo);
#if 0
            if (iplImage)
            {
                cvReleaseImageHeader(&iplImage);
            }
            iplImage = cvCreateImageHeader(cvSize(sFrameInfo.iWidth,sFrameInfo.iHeight),IPL_DEPTH_8U,sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3);
            cvSetData(iplImage,m_pFrameBuffer,sFrameInfo.iWidth*(sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8?1:3));
            cvShowImage(g_CameraName,iplImage);
#else
            cv::Mat matImage(
                cvSize(sFrameInfo.iWidth,sFrameInfo.iHeight),
                sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
                m_pFrameBuffer[i]
                );
            //imshow("dd",matImage);
            //cout<<"getimg"<<endl;
            if(!matImage.empty()){
                switch (i) {
                case 0:
                    matImage.copyTo(m_workmat1);
                    break;
                case 1:
                    matImage.copyTo(m_workmat2);
                    break;
                default:
                    break;
                }
            }
#endif
            //m_iDispFrameNum++;
        }

        //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
        //否则再次调用CameraGetImageBuffer时，程序将被挂起，知道其他线程中调用CameraReleaseImageBuffer来释放了buffer
        CameraReleaseImageBuffer(m_hCamera[i],m_pRawBuffer);
        memcpy(&m_sFrInfo[i],&sFrameInfo,sizeof(tSdkFrameHead));

    }
}

int ledcamera::isignal2camnode() const
{
    return m_isignal2camnode;
}

void ledcamera::softtrigger(int i)
{
    switch (i) {
    case 0:
        CameraSoftTrigger(m_hCamera[m_isignal1camnode]);
        break;
    case 1:
        CameraSoftTrigger(m_hCamera[m_isignal2camnode]);
        break;
    }
}

int ledcamera::getIsignal1camnode() const
{
    return m_isignal1camnode;
}

int ledcamera::getIsignal2camnode() const
{
    return m_isignal2camnode;
}

void ledcamera::starttime(int node)
{
    switch (node) {
    case 0:
        m_time1.TimerStart();
        break;
    case 1:
        m_time2.TimerStart();
        break;
    default:
        break;
    }
}
void ledcamera::recordtime(int node){
    switch (node) {
    case 0:
        m_firsttime1=m_time1.TimerFinish();
        cout<<"time1 first:"<<m_firsttime1<<endl;
        break;
    case 1:
        m_firsttime2=m_time2.TimerFinish();
        cout<<"time2 first:"<<m_firsttime2<<endl;
        break;
    default:
        break;
    }
}
void ledcamera::stoptime(int node)
{
    switch (node) {
    case 0:
        m_secondtime1=m_time1.TimerFinish();
        m_delaytime1=(m_firsttime1+m_secondtime1)*0.5;
        cout<<"time1 second:"<<m_secondtime1<<endl;
        cout<<"time1 end:"<<m_delaytime1<<endl;
        break;
    case 1:
        m_secondtime2=m_time2.TimerFinish();
        m_delaytime2=(m_firsttime2+m_secondtime2)*0.5;
        cout<<"time2 second:"<<m_secondtime2<<endl;
        cout<<"time2 end:"<<m_delaytime2<<endl;
        break;
    default:
        break;
    }
}
