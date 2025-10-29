#ifndef MYTASK_H
#define MYTASK_H

#include <QObject>
#include <QRunnable>
#include <QTcpSocket>
#include "Utils.h"
#include <QFile>
/*
如果MyTask只继承QRunnable，那么它不是派生自QObject的类，它不具备元对象属性，不能使用信号与槽。
如果想让MyTask类使用信号与槽，需要继承QObject
*/
class MyTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MyTask(qintptr handle);
    
private:
    //在线程中执行的逻辑
    void run() override;
    void readData();//读取数据
    
    //处理客户端请求的函数都以do开头(我规定的)
    void doRegister(Data& data);
    void doLogin(Data& data);
    void doUpload(Data& data);
    void doList(Data& data);
    void doDownload(Data& data);
private:
    QTcpSocket* socket;
    qintptr handle;
    QFile* file = nullptr;//用来接收上传文件，因为doUpload函数会执行多次，而上传的文件只应该打开一次
    //因为我们的数据包可能被截断分两次发送，所以这样的数据需要分两次读取
    QByteArray tempData;//读取数据的临时缓冲区
};

#endif // MYTASK_H
