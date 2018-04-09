#include "server.h"

Server::Server(FileDatabase *db):database(db),camera_manager(NULL)
{

    load_cfg();
    camera_manager=new CameraManager(cfg.cams_cfg);
    bool ret=false;
    server=new QTcpServer();
    ret=server->listen(QHostAddress::Any,Pvd::SERVER_PORT);
    if(ret){
        prt(info,"Server listen on port %d success!",Pvd::SERVER_PORT);
    } else {
        prt(fatal,"Server listen on port %d failed!",Pvd::SERVER_PORT);
        exit(1);
    }
    connect(server, &QTcpServer::newConnection, this, &Server::handle_connection);
    service.start();
}

