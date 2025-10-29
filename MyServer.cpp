#include "MyServer.h"

MyServer::MyServer()
{
    
}

void MyServer::incomingConnection(qintptr handle)
{
    emit newMyconnection(handle);
}
