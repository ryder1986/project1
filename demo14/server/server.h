#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "pd.h"
#include "filedatabase.h"
#include "cameramanager.h"
#include "clientsession.h"
class LocationService : public QObject{
    Q_OBJECT
public:
    LocationService(){
        timer=new QTimer();
        connect(timer,SIGNAL(timeout()),this,SLOT(check_client_msg()));//TODO:maybe replace with readReady signal
        udp_skt = new QUdpSocket();
        udp_skt->bind(Pvd::SERVER_REPORTER_PORT,QUdpSocket::ShareAddress);
    }
    ~LocationService()
    {
        disconnect(timer);
        delete timer;
        delete udp_skt;
    }
    void start()
    {
        timer->start(100);
    }

    void stop()
    {
        timer->stop();
    }

public  slots:
    void check_client_msg()
    {
        QByteArray client_msg;
        char *msg;
        if(udp_skt->hasPendingDatagrams())
        {
            client_msg.resize((udp_skt->pendingDatagramSize()));
            QHostAddress sender;
            quint16 senderPort;
            udp_skt->readDatagram(client_msg.data(),client_msg.size(),&sender,&senderPort);
            prt(info,"get client broadcasted code :%s",msg=client_msg.data());
            if(!strcmp(msg,"pedestrian")){

                //    send_info_to_client();
                send_info_to_client(sender);
            }
            else{
                prt(error,"client code :%s NOT MATCH pedestrian,we will not answer",msg=client_msg.data());
            }
            //   udp_skt->flush();
        }else{
            //prt(debug,"searching client on port %d",Pvd::SERVER_REPORTER_PORT)
        }
    }

    void send_info_to_client()
    {
        QByteArray datagram;
        datagram.clear();
        QList <QNetworkInterface>list_interface=QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface i, list_interface) {
            if(i.name()!="lo"){
                QList<QNetworkAddressEntry> list_entry=i.addressEntries();
                foreach (QNetworkAddressEntry e, list_entry) {
                    if(e.ip().protocol()==QAbstractSocket::IPv4Protocol)
                    {
                        datagram.append(QString(e.ip().toString())).append(QString(",")).\
                                append(QString(e.netmask().toString())).append(QString(",")).append(QString(e.broadcast().toString()));
                    }

                }
            }
        }
#if 1
        //broadcast
        udp_skt->writeDatagram(datagram.data(), datagram.size(),
                               QHostAddress::Broadcast, Pvd::CLIENT_REPORTER_PORT);
#else
        //send to single ip. problem in windows
#endif
    }
    void send_info_to_client(const QHostAddress &addr)
    {
        QByteArray datagram;
        datagram.clear();
        QList <QNetworkInterface>list_interface=QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface i, list_interface) {
            if(i.name()!="lo"){
                QList<QNetworkAddressEntry> list_entry=i.addressEntries();
                foreach (QNetworkAddressEntry e, list_entry) {
                    if(e.ip().protocol()==QAbstractSocket::IPv4Protocol)
                    {
                        datagram.append(QString(e.ip().toString())).append(QString(",")).\
                                append(QString(e.netmask().toString())).append(QString(",")).append(QString(e.broadcast().toString()));
                    }

                }
            }
        }
#if 1
        //     /   qDebug()<<"addr :"<<addr.toString();
        //broadcast
        udp_skt->writeDatagram(datagram.data(), datagram.size(),
                               addr, Pvd::CLIENT_REPORTER_PORT);
#else
        //send to single ip. problem in windows
#endif
    }
private:
    QTimer *timer;
    QUdpSocket *udp_skt;
};

class Server:public QObject
{
    Q_OBJECT
    typedef struct configure{
        QString server_name;
        int dev_id;
        QString sig_ip;
        int sig_port;
        QString ntp_ip;
        int ntp_port;
        QJsonValue cams_cfg;
    }Configture_t;
public:

    Server(FileDatabase *db);
    ~Server()
    {
        delete camera_manager;
    }

public slots:
    int handle_client_request(QByteArray request,QByteArray &ret,void *addr)
    {

        ClientSession *cs=(ClientSession *)addr;
        DataPacket data_src(request);
        int type=data_src.get_value("type").toInt();
        int idx=data_src.get_value("cam_index").toInt();

        DataPacket data_dst;
        data_dst.set_value("type",type);
        if(!type){
            ret=data_dst.data();//tell client to update
            return 1;
        }
        bool config_changed=false;
        switch(type){
        case Pvd::GET_CONFIG:// if client wants get config, client is ok
            cs->set_valid(true);
            break;
        case Pvd::SET_CONFIG:
        case Pvd::INSERT_CAMERA:
        case Pvd::DELETE_CAMERA:
        case Pvd::MOD_CAMERA_ALG:
        case Pvd::MOD_CAMERA_ATTR:
        case Pvd::MOD_DEVICE_ATTR:
        default:
            if(cs->is_valid())
                config_changed=true;// if client is valid , change can be made
            break;
        }

        if(config_changed){//if valid change happen, other clients will be invalid
            foreach (ClientSession *session, clients) {
                if(session!=addr)
                    session->set_valid(false);
            }
        }
        if(!cs->is_valid()&&type!=Pvd::GET_CONFIG){// no valid, no update
            data_dst.set_value("type",Pvd::NEED_UPDATE);
            ret=data_dst.data();
            return 1;
        }

        switch(type){
        case Pvd::GET_CONFIG:
        {
            QJsonObject cfg;
            cfg_2_obj(cfg);
            data_dst.set_value("config",cfg);
            break;
        }

        case Pvd::SET_CONFIG:
        {
            obj_2_cfg(data_src.get_value("config").toObject());
            save_cfg();
            camera_manager->restart_cameras(cfg.cams_cfg);
            break;
        }

        case Pvd::CAM_OUTPUT_OPEN:
        {
            if(idx>camera_manager->cameras.size()||idx<1){
                prt(info,"%d out of range ",idx);
                data_dst.set_value("type",Pvd::NEED_UPDATE);
            }else
                camera_manager->cameras[idx-1]->add_watcher(cs->ip());
            break;
        }
        case Pvd::CAM_OUTPUT_CLOSE:
        {
            if(idx>camera_manager->cameras.size()||idx<1){
                prt(info,"%d out of range ",idx);
                data_dst.set_value("type",Pvd::NEED_UPDATE);

            }else{
                camera_manager->cameras[idx-1]->del_watcher(cs->ip());
            }
            break;
        }

        case Pvd::MOD_CAMERA_ALG:
        {
            if(idx>camera_manager->cameras.size()||idx<1){
                prt(info,"%d out of range ",idx);
                data_dst.set_value("type",Pvd::NEED_UPDATE);
                break;
            }
            if(camera_manager->modify_camera(idx,data_src.get_value("alg"),CameraManager::MODIFY_ALG)){

                cfg.cams_cfg=camera_manager->config();
                save_cfg();

            }else{
                data_dst.set_value("type",Pvd::NEED_UPDATE);
            }

            break;
        }


        case Pvd::MOD_CAMERA_ATTR:
        {
            if(idx>camera_manager->cameras.size()||idx<1){
                prt(info,"%d out of range ",idx);
                data_dst.set_value("type",Pvd::NEED_UPDATE);
                break;
            }

            if(idx<=camera_manager->cameras.size()&&idx>0){
                camera_manager->modify_attr(idx,data_src.get_value("camera_args"));
                QJsonValue jv= camera_manager->config();
                cfg.cams_cfg=jv;
                save_cfg();
            }

            break;
        }
        case Pvd::INSERT_CAMERA:
        {
            if(camera_manager->insert_camera(idx,data_src.get_value("camera"))){

                cfg.cams_cfg=camera_manager->config();
                save_cfg();
            }else{
                data_dst.set_value("type",Pvd::NEED_UPDATE);
            }
            break;
        }

        case Pvd::DELETE_CAMERA:
        {
            if(idx>camera_manager->cameras.size()||idx<1){
                prt(info,"%d out of range ",idx);
                data_dst.set_value("type",Pvd::NEED_UPDATE);
            }else{
                camera_manager->delete_camera(idx);
                cfg.cams_cfg=camera_manager->config();
                save_cfg();
            }
            break;
        }

        case Pvd::HEART:
        {
            break;
        }

        case Pvd::REBOOT:
        {

            break;
        }

        case Pvd::MOD_DEVICE_ATTR:
        {
            cfg.dev_id=data_src.get_value("deviceID").toInt();
            cfg.server_name=data_src.get_value("device_name").toString();
            save_cfg();
            break;
        }

        default:break;
        }
        ret=data_dst.data();
        return 0;
    }

    void handle_connection()
    {
        QTcpSocket *skt = server->nextPendingConnection();
        connect(skt, SIGNAL(disconnected()),skt, SLOT(deleteLater()));
        QString str(skt->peerAddress().toString());
        prt(info,"client %s:%d connected",str.toStdString().data(),skt->peerPort());
        ClientSession *client=new ClientSession(skt);
        connect(client,SIGNAL(socket_error(ClientSession*)),this,SLOT(delete_client(ClientSession*)));
        connect(skt,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
        connect(client,SIGNAL( client_request(QByteArray,QByteArray&,void *)),this,
                SLOT(handle_client_request(QByteArray,QByteArray&,void *)),Qt::DirectConnection);//important,in case of competition bugs
        clients.append(client);
    }

    void delete_client(ClientSession *c)
    {
        prt(info,"client %s disconnected",c->ip().toStdString().data());
        delete c ;
        clients.removeOne(c);
    }

    void  displayError(QAbstractSocket::SocketError socketError)
    {
        //prt(info,"client %s disconnected",c->ip().toStdString().data());
        //      prt(info,"client error");
        switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            prt(info,"client closed");
            break;
        case QAbstractSocket::HostNotFoundError:
            prt(info,"client error");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            prt(info,"client error");
            break;
        default:
            break;
        }
    }

private:
    void load_cfg()
    {
        QJsonObject obj;
        database->load(obj);
        obj_2_cfg(obj);
    }

    void save_cfg()
    {
        QJsonObject cfg;
        cfg_2_obj(cfg);
        database->save(cfg);
    }


    void cfg_2_obj(QJsonObject &obj)
    {

        obj["device_name"]=cfg.server_name;
        obj["deviceID"]=cfg.dev_id;
        obj["signal_machine_ip"]=cfg.sig_ip;
        obj["signal_machine_port"]=cfg.sig_port;
        obj["ntp_ip"]=cfg.ntp_ip;
        obj["ntp_port"]=cfg.ntp_port;
        obj["cameras"]=cfg.cams_cfg;
    }

    void obj_2_cfg(QJsonObject obj)
    {
        cfg.server_name=obj["device_name"].toString();
        cfg.dev_id=obj["deviceID"].toInt();
        cfg.sig_ip=obj["signal_machine_ip"].toString();
        cfg.sig_port= obj["signal_machine_port"].toInt();
        cfg.ntp_ip=obj["ntp_ip"].toString();
        cfg.ntp_port=obj["ntp_port"].toInt();
        cfg.cams_cfg=obj["cameras"];
    }

    QTcpServer *server;//server for reply all clients request ,execute client cmds,like add cam,del cam, reconfigure cam,etc..
    FileDatabase *database;//hold config data;
    CameraManager *camera_manager;//hold cameras
    QList <ClientSession *> clients;//hold sessions
    Configture_t cfg;
    char recv_buf[Pvd::BUFFER_MAX_LENGTH];
    char send_buf[Pvd::BUFFER_MAX_LENGTH];
    LocationService service;

};


#endif // SERVER_H
