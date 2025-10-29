#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>

class MyServer : public QTcpServer
{
    Q_OBJECT//所有派生自QObject的类，都要在私有权限中添加Q_OBJECT宏，帮助我们定义元对象的属性
public:
    MyServer();
    
signals:
    //当有新的连接时，会在incomingConnection函数中发送这个信号
    void newMyconnection(qintptr handle);
    
private:
    //重写函数，当有新的连接时会自动执行这个函数
    void incomingConnection(qintptr handle) override;
};

#endif // MYSERVER_H
