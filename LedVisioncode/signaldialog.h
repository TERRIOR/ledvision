#ifndef SIGNALDIALOG_H
#define SIGNALDIALOG_H

#include <QDialog>
#include <qfile.h>
#include <QtWidgets>
#include <iostream>
#include <windows.h>
#include "CameraApi.h"
#include "CameraDefine.h"
#include <vector>
using namespace std;
namespace Ui {
class signalDialog;
}

class signalDialog : public QDialog
{
    Q_OBJECT

public:
    signalDialog(int,int);
    explicit signalDialog(QWidget *parent = 0);
    ~signalDialog();
    void setwhere(int ,int);
    void initform();
    bool getcamdevice();
    bool confirm();
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_radioButton_cam_2_clicked(bool checked);

    void on_radioButton_cam_clicked(bool checked);



    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_2_currentTextChanged(const QString &arg1);

    void on_radioButton_file_clicked(bool checked);

    void on_radioButton_file_2_clicked(bool checked);

signals:
    void sendsignalset(int,int,string,string);
private:
    bool m_bcaminited=false;
    bool m_bfinddevice=false;
    int m_iwhere;
    int m_iCameraCounts = 4;
    vector<string> m_devicevec;
    int m_iwhere2;
    Ui::signalDialog *ui;
    QButtonGroup *btngroup1;
    QButtonGroup *btngroup2;
};

#endif // SIGNALDIALOG_H
