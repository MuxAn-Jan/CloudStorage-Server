#ifndef USERDAO_H
#define USERDAO_H

#include <QString>
#include <QSqlDatabase>

struct UserEntity
{
    QString userid;
    QString pwd;
};

class UserDao
{
public:
    UserDao(QSqlDatabase db);
    /*
        插入一个用户，用于注册
        参数1：用户的信息
        参数2：插入是否成功，用户名不可以重复
    */
    bool addUser(UserEntity& ue, bool& ok);
    /*
        查询一个用户，用于登录
        参数1：用户的信息
        参数2：查询是否成功
    */
    bool checkUser(UserEntity& ue, bool& ok);
private:
    QSqlDatabase db;
};

#endif // USERDAO_H
