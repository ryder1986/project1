#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include "filedatabase.h"
#include "QJsonObject"
#include "videosource.h"
#include "pddemoprocessor.h"
#include "processeddatasender.h"
#include "pdprocessor.h"
#define TEST
#undef TEST
using namespace cv;
class Camera : public QThread
{
    Q_OBJECT
    typedef struct Camera_config{
        QString url;
        int direction;
        int camera_id;
        QString user_name;
        QString password;
        QString camera_ip;
        int camera_port;
        QJsonValue alg;

    }Configture_t;
public:
    explicit Camera(QJsonValue config);
    ~Camera()
    {
        stop_cam();
    }

    Rect area_2_rect(QJsonValue area)
    {
        int x_min=10000;
        int y_min=10000;
        int x_max=0;
        int y_max=0;
        foreach (QJsonValue v, area.toArray()) {
            int x= v.toObject()["x"].toInt();
            int y= v.toObject()["y"].toInt();
            if(x<x_min)
                x_min=x;
            if(x>x_max)
                x_max=x;
            if(y<y_min)
                y_min=y;
            if(y>y_max)
                y_max=y;
        }
       return Rect(x_min,y_min,x_max-x_min,y_max-y_min);
    }
    bool process(Mat mt,QByteArray &rst)
    {

        rst.clear();
        Rect rct=processor->rect();
        Mat detect=mt(rct);
        bool  ret=false;
        int mat_w=mt.cols;
        int mat_h=mt.rows;
        vector <Rect> rects;

        if(processor->real_process(detect,rects)){
            rst.append(QString::number(mat_w)).append(",").append(QString::number(mat_h)).append(":");
            foreach (Rect r, rects) {
                QString x_str=QString::number(r.x+rct.x/2);
                QString y_str=QString::number(r.y+rct.y/2);
                QString width_str=QString::number(r.width);
                QString height_str=QString::number(r.height);
                rst.append(x_str).append(",").append(y_str).append(",").append(width_str).append(",").append(height_str).append(":");
            }
            ret=true;
        }
        return ret;

    }
    bool modify_alg(QJsonValue cfg)
    {
        int ret=true;
        lock.lock();
        QString str=cfg.toObject()["selected_alg"].toString();
        if(str=="pd"){
            delete  processor;
            cam_cfg.alg=cfg;
            QJsonValue v=cam_cfg.alg.toObject()["pd"].toObject()["detect_area"]=cfg.toObject()["pd"].toObject()["detect_area"];
            processor=new PdProcessor(v);
        }else if(str=="pd-demo"){
            delete  processor;
            cam_cfg.alg=cfg;
            QJsonValue v=cam_cfg.alg.toObject()["pd-demo"].toObject()["detect_area"]=cfg.toObject()["pd-demo"].toObject()["detect_area"];
            processor=new PdDemoProcessor(v);
        }else{
            ret=false;
        }

        lock.unlock();
        return ret;
    }

    void modify_attr(QJsonValue v)
    {
        QJsonObject obj=v.toObject();
        int di= obj["direction"].toInt();
        int no=obj["camera_id"].toInt();
        cam_cfg.direction=di;
        cam_cfg.camera_id=no;
    }

    QJsonValue config()
    {
        return cfg_2_jv();
    }
    void add_watcher(QString ip)
    {
        if(ip_list.contains(ip)){

        }else{
            prt(info,"client %s require output",ip.toStdString().data());
            ip_list.append(ip);
        }

    }
    void del_watcher(QString ip)
    {
        if(ip_list.contains(ip)){
            prt(info,"client %s stop output",ip.toStdString().data());
            ip_list.removeOne(ip);
        }else{
        }
    }
private:
    void start_cam()
    {
        src=new VideoSource(cam_cfg.url);
        QString str=cam_cfg.alg.toObject()["selected_alg"].toString();
        if(str=="pd"){
            QJsonValue v=cam_cfg.alg.toObject()["pd"].toObject()["detect_area"];
            processor=new PdProcessor(v);
            //   processor=new PdProcessor();
        }else if(str=="pd-demo"){
            QJsonValue v=cam_cfg.alg.toObject()["pd-demo"].toObject()["detect_area"] ;
            processor=new PdDemoProcessor(v);
        }

        start();
    }
    void stop_cam()
    {
        delete tmr;
        quit=true;
        this->wait();
        delete src;
        delete processor;
        src=NULL;
        processor=NULL;
    }

    virtual QJsonValue cfg_2_jv()
    {
        QJsonObject jo;
        QJsonValue jv;
        jo["url"]= cam_cfg.url;
        jo["direction"]=cam_cfg.direction;
        jo["camera_id"]= cam_cfg.camera_id;
        jo["user_name"]= cam_cfg.user_name;
        jo["password"]= cam_cfg.password;
        jo["camera_ip"]= cam_cfg.camera_ip;
        jo["camera_port"]= cam_cfg.camera_port;
        jo["alg"]=cam_cfg.alg;
        jv=jo;
        return jv;

    }
    virtual void jv_2_cfg(QJsonValue cfg)
    {
        cam_cfg.url = cfg.toObject()["url"].toString();
        cam_cfg.direction=cfg.toObject()["direction"].toInt();
        cam_cfg.camera_id=cfg.toObject()["camera_id"].toInt();
        cam_cfg.user_name=cfg.toObject()["user_name"].toString();
        cam_cfg.password=cfg.toObject()["password"].toString();
        cam_cfg.camera_ip=cfg.toObject()["camera_ip"].toString();
        cam_cfg.camera_port=cfg.toObject()["camera_port"].toInt();
        cam_cfg.alg=cfg.toObject()["alg"];



    }
    void send_out(QByteArray ba)
    {
        //  emit output(ba);
        ProcessedDataSender *s=ProcessedDataSender::get_instance();
        foreach (QString ip, ip_list) {
            QString str(ba);
            prt(debug,"send %s to %s",str.toStdString().data(),ip.toStdString().data())
            s->send(ba,QHostAddress(ip));
        }
    }
protected:
    void run()
    {
        int i=0;
        QByteArray ba;
        Mat frame;
        Mat frame960x540;
        threadid=(int)QThread::currentThread();
        QByteArray rst;
        while(!quit){
            //   prt(info,"runing %s",cam_cfg.url.toStdString().data());
            lock.lock();
            if(src->get_frame(frame)&&frame.cols>0&&frame.rows>0){
                frame_rate++;
                //  frame960x540=frame.resize(960,540);
                cv::resize(frame, frame960x540, cv::Size(), 960.0/frame.cols,540/frame.rows);
                bool ret=process(frame960x540,rst);
                if(ret){
                    ba.clear();
                    ba.append(rst);
                    //  emit output(ba);
                    send_out(ba);
                }
            }else{
                //prt(info,"get no frame");
            }
            lock.unlock();

        }
        QThread::msleep(10);
    }

signals:
    void output(QByteArray ba);
public slots:
    void handle_time_up()
    {
        // prt(info,"%s framerate:%d,(id:%x)",cam_cfg.url.toStdString().data(),frame_rate,threadid);
        frame_rate=0;
    }

private:
    QList <QString> ip_list;
    int frame_rate;
    int threadid;
    QTimer *tmr;
    VideoSource *src;
    VideoProcessor *processor;
    Configture_t cam_cfg;
    bool quit;
    QMutex lock;
};

#endif // CAMERA_H
