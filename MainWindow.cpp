#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QHostInfo>
#include <QHostAddress>
#include <QAbstractSocket>
#include "Utils.h"
#include "DBFactory.h"
#include <QDir>
#include <QDebug>
#include "MyTask.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initHostInfo();
    initServer();
    QThreadPool::globalInstance()->setMaxThreadCount(10);//设置线程池最大线程数
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}

void MainWindow::initHostInfo()
{
    QString name = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(name);
    QList<QHostAddress> list = info.addresses();
    for(auto item : list)
    {
        if(item.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ui->comboBox->addItem(item.toString());
        }
    }
}

void MainWindow::initServer()
{
    //server = new QTcpServer(this);
    server = new MyServer;
    server->listen(QHostAddress::Any, 6688);
    //connect(server, &QTcpServer::newConnection, this, &MainWindow::onNewconnection);
    connect(server, &MyServer::newMyconnection, this, 
            &MainWindow::onNewMyconnection);
}

//void MainWindow::doRegister(Data& data)
//{
//    QString id(data.data.name);//创建id字符串对象
//    QString pwd(data.data.passwd);//创建密码字符串对象
    
//    UserEntity ue;//数据库的数据对象
//    ue.userid = id;
//    ue.pwd = pwd;
//    bool ok;
//    DBFactory::getInstance()->getUserDao()->addUser(ue, ok);
//    Data redata;//返回的数据
//    if(ok)
//    {
//        redata.data.type = successType;//注册成功
//        //注册成功，为用户创建一个文件夹
//        QDir dir(".");
//        dir.mkdir(id);
//    }
//    else
//    {
//        redata.data.type = faildType;//注册失败
//    }
//    //给客户端返回数据
//    socket->write(redata.transData, sizeof(redata.transData));
//}

//void MainWindow::doLogin(Data &data)
//{
//    QString id(data.data.name);
//    QString pwd(data.data.passwd);
    
//    UserEntity ue;
//    ue.userid = id;
//    ue.pwd = pwd;
//    bool ok;
//    DBFactory::getInstance()->getUserDao()->checkUser(ue, ok);
    
//    Data redata;
//    if(ok)
//    {
//        redata.data.type = successType;
//        strcpy(redata.data.name, data.data.name);//将id返回给客户端
//    }
//    else
//    {
//        redata.data.type = faildType;
//    }
//    //给客户端返回数据
//    socket->write(redata.transData, sizeof(redata.transData));
//}

//void MainWindow::doUpload(Data &data)
//{
//    //接收上传文件的第一包数据
//    if(file == nullptr)
//    {
//        //创建文件对象
//        file = new QFile(QString(data.data.name)+"/"+QString(data.data.fileName));
//        //打开文件
//        file->open(QIODevice::WriteOnly);
//    }
//    //接收上传文件的最后一包数据
//    if(data.data.dataSize == -1)
//    {
//        file->close();//关闭文件
//        delete file;
//        file = nullptr;
//        return ;
//    }
//    file->write(data.data.data, data.data.dataSize);//向文件中写入数据
//}

//void MainWindow::doList(Data &data)
//{
//    QDir dir(QString::fromUtf8(data.data.name));
//    QStringList sl = dir.entryList();//获得所有文件的名字
//    QString sendStr;
//    for(auto item : sl)
//    {
//        sendStr += item;
//        sendStr += ",";
//    }
//    strcpy(data.data.data, sendStr.toUtf8().data());
//    socket->write(data.transData, sizeof(data.transData));
//    socket->waitForBytesWritten();
//}

//void MainWindow::doDownload(Data &data)
//{
//    QFile file(QString::fromUtf8(data.data.name)+"/"+QString::fromUtf8(data.data.fileName));
//    if(!file.open(QIODevice::ReadOnly))
//    {
//        return ;
//    }
//    Data redata;
//    strcpy(redata.data.fileName, data.data.fileName);//写入文件名
//    while(!file.atEnd())
//    {
//        redata.data.type = downloadType;//下载数据类型
//        qint64 size = file.read(redata.data.data, sizeof(redata.data.data));//读取文件数据
//        redata.data.dataSize = size;//数据长度
//        socket->write(redata.transData, sizeof(redata.transData));
//        socket->waitForBytesWritten();//等待数据完全写入，阻塞函数
//    }
//    redata.data.dataSize = -1;//下载完毕
//    socket->write(redata.transData, sizeof(redata.transData));
//    socket->waitForBytesWritten();
//    file.close();
//}

void MainWindow::onNewMyconnection(qintptr handle)
{
    MyTask* mt = new MyTask(handle);//创建任务对象
    QThreadPool::globalInstance()->start(mt);//线程池执行任务对象
}

//void MainWindow::onNewconnection()
//{
//    socket = server->nextPendingConnection();
//    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
//    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
//    //显示客户端的ip
//    QHostAddress addr(socket->peerAddress().toIPv4Address());
//    ui->textBrowser->append(addr.toString()+":客户端连接");
//}

//void MainWindow::onReadyRead()
//{
//    Data data;//联合体对象
//    int readSize;
//    //读取数据 ， 每次执行onReadyRead 可能不只有一包数据
//    while((readSize = socket->read(data.transData, sizeof(data.transData)-tempData.size()))>0)
//    {   
//        //如果发现读取的数据小于数据包，说明这包数据被打断了，数据包需要拼接
//        if(readSize < sizeof(data.transData))
//        {
//            tempData.append(data.transData, readSize);//向缓存中追加数据
//            if(tempData.size() == sizeof(data.transData))//组成了一包数据
//            {
//                //将缓存中的数据拷贝到数据包对象中
//                memcpy(data.transData, tempData.data(), sizeof(data.transData));
//                tempData.clear();//清空缓存
//            }
//            else//不完全的数据包不能执行下面的操作
//            {
//                break;
//            }
//        } 
            
//        switch(data.data.type)//判断数据包类型
//        {
//        case registerType:
//            doRegister(data);
//            break;
//        case loginType:
//            doLogin(data);
//            break;
//        case uploadType:
//            doUpload(data);
//            break;
//        case listType:
//            doList(data);
//            break;
//        case downloadType:
//            doDownload(data);
//            break;
//        }
//    }
//}

//void MainWindow::onDisconnected()
//{
//    //显示客户端的ip
//    QHostAddress addr(socket->peerAddress().toIPv4Address());
//    ui->textBrowser->append(addr.toString()+":客户端断开连接");
//}

