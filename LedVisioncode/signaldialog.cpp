#include "signaldialog.h"
#include "ui_signaldialog.h"
#ifdef _WIN64
#pragma comment(lib,"MVCAMSDK_X64.lib")
#else
#pragma comment(lib,"MVCAMSDK.lib")
#endif

signalDialog::signalDialog(int i, int j):ui(new Ui::signalDialog),m_iwhere(i),m_iwhere2(j)
{

    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);
    initform();
    btngroup1 = new QButtonGroup(this);
    btngroup1->addButton(ui->radioButton_cam,0);
    btngroup1->addButton(ui->radioButton_file,1);

    btngroup2 = new QButtonGroup(this);
    btngroup2->addButton(ui->radioButton_cam_2,0);
    btngroup2->addButton(ui->radioButton_file_2,1);
    cout<<m_iwhere<<"  "<<m_iwhere2<<endl;
    switch (m_iwhere) {
    case 0:
        ui->radioButton_file->setChecked(true);
        ui->comboBox->hide();
        break;
    case 1:
        ui->radioButton_cam->setChecked(true);
        ui->comboBox->show();
        break;
    default:
        break;
    }
    switch (m_iwhere2) {
    case 0:
        ui->radioButton_file_2->setChecked(true);
        ui->comboBox_2->hide();
        break;
    case 1:
        ui->radioButton_cam_2->setChecked(true);
        ui->comboBox->show();
        break;
    default:
        break;
    }
}

signalDialog::signalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signalDialog)
{

}

signalDialog::~signalDialog()
{
    cout<<"close"<<endl;
    delete ui;
    delete btngroup1;
    delete btngroup2;
}

void signalDialog::setwhere(int i, int j)
{
    m_iwhere=i;
    m_iwhere2=j;
}
void signalDialog::initform()
{
    //this->location = this->geometry();
    this->setProperty("form", true);
    this->setProperty("canMove", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    //ui->mainToolBar->hide();
    QString qssFile=":/qss/flatwhite.css";
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = file.readAll();
        QString paletteColor = qss.mid(20, 7);
        this->setPalette(QPalette(QColor(paletteColor)));
        this->setStyleSheet(qss);
        file.close();
    }

}

bool signalDialog::getcamdevice()
{
    int                     iStatus=-1;
    tSdkCameraDevInfo       tCameraEnumList[4];
    tSdkCameraCapbility     g_tCapability;

    CameraEnumerateDevice(tCameraEnumList,&m_iCameraCounts);
    if(m_iCameraCounts==0){
        m_bfinddevice=false;
        return false;
    }
    m_bfinddevice=true;
    int i;
    for(i=0;i<m_iCameraCounts;i++)
    {

        string str=tCameraEnumList[i].acFriendlyName;
        m_devicevec.push_back(str);
        ui->comboBox->addItem(QString::fromStdString(str));
        ui->comboBox_2->addItem(QString::fromStdString(str));
    }
    return true;
}

void signalDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->button(QDialogButtonBox::Ok)==button){

        if(ui->radioButton_file->isChecked()){
            m_iwhere=0;
        }else{
            m_iwhere=1;
        }
        if(ui->radioButton_file_2->isChecked()){
            m_iwhere2=0;
        }else{
            m_iwhere2=1;
        }
        confirm();
    }else if(ui->buttonBox->button(QDialogButtonBox::Cancel)==button){
        this->close();
    }
}
bool signalDialog::confirm(){

    if(m_iwhere==1&&ui->comboBox->currentText()==""){
        QMessageBox::information(this,tr("fail"),QString::fromStdString("you should select signals"));
        return false;
    }
    if(m_iwhere2==1&&ui->comboBox_2->currentText()==""){
        QMessageBox::information(this,tr("fail"),QString::fromStdString("you should select signals"));
        return false;
    }
    if(m_iwhere==1&&m_iwhere2==1){
        if(ui->comboBox->currentText()==ui->comboBox_2->currentText()){
            QMessageBox::information(this,tr("fail"),QString::fromStdString("you should not set the same signal"));
            return false;
        }
    }
    emit sendsignalset(m_iwhere,m_iwhere2,ui->comboBox->currentText().toStdString(),ui->comboBox_2->currentText().toStdString());
    this->close();
    return true;
}


void signalDialog::on_radioButton_cam_2_clicked(bool checked)
{
    cout<<"radiocam2"<<endl;

    if(!m_bcaminited&&checked){
        if(!getcamdevice()){
            QMessageBox::information(this,tr("fail"),QString::fromStdString("no device"));
        }
        m_bcaminited=true;
    }
    if(!m_bfinddevice){
        ui->radioButton_file_2->setChecked(true);
    }else{
        ui->comboBox_2->show();
    }

}

void signalDialog::on_radioButton_cam_clicked(bool checked)
{

    cout<<"radiocam1"<<endl;

    if(!m_bcaminited&&checked){
        if(!getcamdevice()){
            QMessageBox::information(this,tr("fail"),QString::fromStdString("no device"));
        }
        m_bcaminited=true;
    }
    if(!m_bfinddevice){
        ui->radioButton_file->setChecked(true);
    }else{
        ui->comboBox->show();
    }

}


void signalDialog::on_comboBox_currentTextChanged(const QString &arg1)
{

}

void signalDialog::on_comboBox_2_currentTextChanged(const QString &arg1)
{

}

void signalDialog::on_radioButton_file_clicked(bool checked)
{

    ui->comboBox->hide();
}

void signalDialog::on_radioButton_file_2_clicked(bool checked)
{
    ui->comboBox_2->hide();
}
