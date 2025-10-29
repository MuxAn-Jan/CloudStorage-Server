#ifndef UTILS_H
#define UTILS_H

//数据包类型
union Data
{
    //用于解析数据的成员
    struct 
    {
        int type;//数据类型
        char name[20];//账号
        char passwd[20];//密码
        char fileName[20];//文件名
        char data[200];//文件数据
        int dataSize;//传输数据的大小
    }data;
    //用于收发数据的成员
    char transData[sizeof (data)];
};

//数据包的类型
enum DataType
{
    registerType,//注册
    loginType,//登录
    listType,//获取列表
    uploadType,//上传
    downloadType,//下载
    historyType,//历史记录
    successType,//操作成功
    faildType//操作失败
};

#endif // UTILS_H
