#ifndef DBFACTORY_H
#define DBFACTORY_H

#include <QSqlDatabase>
#include "UserDao.h"
#include <map>

using namespace std;

class DBFactory
{
public:
    static DBFactory* getInstance();
    static void releaseInstance();
    UserDao* getUserDao(QString id);
private:
    DBFactory();
    DBFactory(const DBFactory& other);
    ~DBFactory();
    
    static DBFactory* instance;
    //QSqlDatabase db;
    map<QString, QSqlDatabase> dbs;//key 是线程id
    //UserDao* userDao;
    map<QString, UserDao*> userDaos;//不同的userDao要使用不同的db  key 是线程id
};

#endif // DBFACTORY_H
