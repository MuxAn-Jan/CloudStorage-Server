#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QTcpServer>
#include "MyServer.h"
#include <QTcpSocket>
#include "Utils.h"
#include <QFile>
#include <QThreadPool>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private:
    void initHostInfo();//初始化本机信息
    void initServer();//初始化服务端
    
//    //处理客户端请求的函数都以do开头(我规定的)
//    void doRegister(Data& data);
//    void doLogin(Data& data);
//    void doUpload(Data& data);
//    void doList(Data& data);
//    void doDownload(Data& data);
private slots:
    //void onNewconnection();//QTcpServer newConnection信号
    void onNewMyconnection(qintptr handle);//MyServer newMyConnection信号
    //void onReadyRead();//QTcpSocket readyRead信号
    //void onDisconnected();//QTcpSocket disconnected信号
private:
    Ui::MainWindow *ui;
    //QTcpServer* server;
    MyServer* server;
    QTcpSocket* socket;
//    QFile* file = nullptr;//用来接收上传文件，因为doUpload函数会执行多次，而上传的文件只应该打开一次
//    //因为我们的数据包可能被截断分两次发送，所以这样的数据需要分两次读取
//    QByteArray tempData;//读取数据的临时缓冲区
};
#endif // MAINWINDOW_H
