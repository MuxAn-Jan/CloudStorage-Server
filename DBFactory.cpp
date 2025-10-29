#include "DBFactory.h"

DBFactory* DBFactory::instance = nullptr;

DBFactory *DBFactory::getInstance()
{
    if(instance == nullptr)
    {
        instance = new DBFactory;
    }
    return instance;
}

void DBFactory::releaseInstance()
{
    if(instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

UserDao *DBFactory::getUserDao(QString id)
{
    if(dbs.count(id) > 0)
    {
        return userDaos[id];//如果对应id的数据库已经打开过，那么直接返回id对应的userDao
    }
    //创建新的数据库链接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", id);
    db.setHostName("old.db");
    db.open();
    dbs[id] = db;
    //创建新的UserDao对象
    UserDao* dao = new UserDao(db);
    userDaos[id] = dao;
    return dao;
}

DBFactory::DBFactory()
{
//    db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("old.db");
//    db.open();
    
//    userDao = new UserDao;
}

DBFactory::DBFactory(const DBFactory &other)
{
    Q_UNUSED(other)
}

DBFactory::~DBFactory()
{
    //db.close();
    //关闭所有数据库
    for(auto i : dbs)//使用auto遍历map，auto是pair类型
    {
        i.second.close();
    }
    //删除所有dao对象
    for(auto i : userDaos)
    {
        delete i.second;
    }
}
