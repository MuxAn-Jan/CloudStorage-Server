#include "MyTask.h"
#include <QDir>
#include "DBFactory.h"
#include <QThread>

MyTask::MyTask(qintptr handle):handle(handle)
{
    setAutoDelete(true);
    //不能在构造函数中创建QTcpSocket对象，因为构造函数不是在子线程中执行的
}

void MyTask::run()
{
    //在子线程中创建socket
    socket = new QTcpSocket();
    socket->setSocketDescriptor(handle);
    //只要连接断开就停止循环
    while(socket->state() != QAbstractSocket::UnconnectedState)
    {
        //子线程可以有死循环，所以我们不需要连接socket的信号，因为可以在循环中等待
        //代码尽量写成并列结构，不要嵌套
        if(!socket->waitForReadyRead())
        {
            continue;
        }
        readData();//读取数据
        
    }
    
    delete socket;//删除socket
}

void MyTask::readData()
{
    Data data;//联合体对象
    int readSize;
    //读取数据 ， 每次执行onReadyRead 可能不只有一包数据
    while((readSize = socket->read(data.transData, sizeof(data.transData)-tempData.size()))>0)
    {   
        //如果发现读取的数据小于数据包，说明这包数据被打断了，数据包需要拼接
        if(readSize < sizeof(data.transData))
        {
            tempData.append(data.transData, readSize);//向缓存中追加数据
            if(tempData.size() == sizeof(data.transData))//组成了一包数据
            {
                //将缓存中的数据拷贝到数据包对象中
                memcpy(data.transData, tempData.data(), sizeof(data.transData));
                tempData.clear();//清空缓存
            }
            else//不完全的数据包不能执行下面的操作
            {
                break;
            }
        } 
            
        switch(data.data.type)//判断数据包类型
        {
        case registerType:
            doRegister(data);
            break;
        case loginType:
            doLogin(data);
            break;
        case uploadType:
            doUpload(data);
            break;
        case listType:
            doList(data);
            break;
        case downloadType:
            doDownload(data);
            break;
        }
    }
}

void MyTask::doRegister(Data &data)
{
    QString id(data.data.name);//创建id字符串对象
    QString pwd(data.data.passwd);//创建密码字符串对象
    
    UserEntity ue;//数据库的数据对象
    ue.userid = id;
    ue.pwd = pwd;
    bool ok;
    //qintptr是和指针大小一样的整数类型
    qintptr nid = reinterpret_cast<qintptr>(QThread::currentThreadId());
    QString strId = QString::number(nid);//根据整数id创建字符串
    DBFactory::getInstance()->getUserDao(strId)->addUser(ue, ok);
    Data redata;//返回的数据
    if(ok)
    {
        redata.data.type = successType;//注册成功
        //注册成功，为用户创建一个文件夹
        QDir dir(".");
        dir.mkdir(id);
    }
    else
    {
        redata.data.type = faildType;//注册失败
    }
    //给客户端返回数据
    socket->write(redata.transData, sizeof(redata.transData));
}

void MyTask::doLogin(Data &data)
{
    QString id(data.data.name);
    QString pwd(data.data.passwd);
    
    UserEntity ue;
    ue.userid = id;
    ue.pwd = pwd;
    bool ok;
    //qintptr是和指针大小一样的整数类型
    qintptr nid = reinterpret_cast<qintptr>(QThread::currentThreadId());
    QString strId = QString::number(nid);//根据整数id创建字符串
    DBFactory::getInstance()->getUserDao(strId)->checkUser(ue, ok);
    
    Data redata;
    if(ok)
    {
        redata.data.type = successType;
        strcpy(redata.data.name, data.data.name);//将id返回给客户端
    }
    else
    {
        redata.data.type = faildType;
    }
    //给客户端返回数据
    socket->write(redata.transData, sizeof(redata.transData));
}

void MyTask::doUpload(Data &data)
{
    //接收上传文件的第一包数据
    if(file == nullptr)
    {
        //创建文件对象
        file = new QFile(QString(data.data.name)+"/"+QString(data.data.fileName));
        //打开文件
        file->open(QIODevice::WriteOnly);
    }
    //接收上传文件的最后一包数据
    if(data.data.dataSize == -1)
    {
        file->close();//关闭文件
        delete file;
        file = nullptr;
        return ;
    }
    file->write(data.data.data, data.data.dataSize);//向文件中写入数据
}

void MyTask::doList(Data &data)
{
    QDir dir(QString::fromUtf8(data.data.name));
    QStringList sl = dir.entryList();//获得所有文件的名字
    QString sendStr;
    for(auto item : sl)
    {
        sendStr += item;
        sendStr += ",";
    }
    strcpy(data.data.data, sendStr.toUtf8().data());
    socket->write(data.transData, sizeof(data.transData));
    socket->waitForBytesWritten();
}

void MyTask::doDownload(Data &data)
{
    QFile file(QString::fromUtf8(data.data.name)+"/"+QString::fromUtf8(data.data.fileName));
    if(!file.open(QIODevice::ReadOnly))
    {
        return ;
    }
    Data redata;
    strcpy(redata.data.fileName, data.data.fileName);//写入文件名
    while(!file.atEnd())
    {
        redata.data.type = downloadType;//下载数据类型
        qint64 size = file.read(redata.data.data, sizeof(redata.data.data));//读取文件数据
        redata.data.dataSize = size;//数据长度
        socket->write(redata.transData, sizeof(redata.transData));
        socket->waitForBytesWritten();//等待数据完全写入，阻塞函数
    }
    redata.data.dataSize = -1;//下载完毕
    socket->write(redata.transData, sizeof(redata.transData));
    socket->waitForBytesWritten();
    file.close();
}
