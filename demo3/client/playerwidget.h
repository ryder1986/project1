#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QPainter>
#include <QTimer>
#include <QMutex>
#include "tool.h"
class PlayerWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    PlayerWidget()
    {
        //        QTimer *t=new QTimer();
        //        connect(t,SIGNAL(timeout()),this,SLOT(update()));
        //        t->start(10);
        frame_rate=0;

        QTimer *t=new QTimer();
        connect(t,SIGNAL(timeout()),this,SLOT(check_rate()));
        t->start(1000);

    }


    void set_image(QImage img1)

    {
        lock.lock();
        img=img1;
        lock.unlock();
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        // if(img1.width()>0){
        if(1){
            lock.lock();
            frame_rate++;
            QPainter painter(this);
            if(!img.isNull()){
           //     prt(info,"%d %d",img.byteCount(),img.bytesPerLine());
                painter.drawImage(QRect(0,0,this->width(),this->height()),img);
            }

#if 0
            QBrush blue_brush_trans(QColor(0,222,200,255));
            painter.setBrush(blue_brush_trans);

            //   painter.drawRect(0,0,this->width(),this->height());
            painter.drawRect(100,100,300,300);
#endif
            lock.unlock();
        }
    }
    void  initializeGL()
    {

    }
public slots:
    void check_rate()
    {
        //frame_rate++;
        prt(info,"frame rate :%d ",frame_rate);
        frame_rate=0;
    }


private:
    QImage img;
    QMutex lock;
    int frame_rate;
};

#endif // PLAYERWIDGET_H
