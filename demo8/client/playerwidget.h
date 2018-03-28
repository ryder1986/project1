#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QPainter>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QMouseEvent>
#include "tool.h"
class LayoutPainter{
public:
    LayoutPainter(QPainter *pain)
    {
        data.clear();
        p=pain;
        //    p=new QPainter(par);
    }

    ~LayoutPainter()
    {
        //     delete p;
    }

    void paint(QPainter *test)
    {


        //       bool active=test->isActive();
        if(!data.isEmpty()){
            QString str(data.toStdString().data());
            prt(info,"-->%s",str.toStdString().data());


            //        QString str(rst.data());
            QStringList list=str.split(":");
            QStringList l;

#if 0
            QBrush blue_brush_trans(QColor(111,111,111,111));
            // blue_brush_trans.setStyle(Qt::BrushStyle);
            test->setBrush(blue_brush_trans);
#else
            QBrush brush1(QColor(255,0,0,111));
            QPen pen(brush1,10);
            test->setPen(pen);

#endif
            int w=test->window().width();
            int h=test->window().height();
            int wi=640;
            int he=480;
            foreach (QString s, list) {
                l=s.split(',');
                QRect r;

                if(l.size()==4){
                    r.setRect(l[0].toInt()*w/wi*2,l[1].toInt()*h/he*2,l[2].toInt()*w/wi*2,
                            l[3].toInt()*h/he*2);
                    // rcts.append(r);
                    //           test->isActive();
                    test->drawRect(r);
                }else
                    if(l.size()==2){
                        int wid=l[0].toInt();
                        int hei=l[1].toInt();
                     //   prt(info,"(get %d %d)",wid,hei);
                        wi=wid;
                        he=hei;
                    }

            }


            data.clear();
        }
    }
    void set_data(QByteArray ba)
    {
        data=ba;
    }

private:
    QPainter *p;
    QByteArray data;
};

class PlayerWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    PlayerWidget()
    {
        pt=new LayoutPainter(&painter);
        frame_rate=0;
        QTimer *t=new QTimer();
        connect(t,SIGNAL(timeout()),this,SLOT(check_rate()));
        t->start(1000);


        QBrush blue_brush_trans(QColor(0,222,200,255));
        // blue_brush_trans.setStyle(Qt::BrushStyle);
        painter.setBrush(blue_brush_trans);

        QPen p(blue_brush_trans,200);
        painter.setPen(p);

    }

    ~PlayerWidget()
    {
        delete pt;
    }

    void set_image(QImage img1)

    {
        lock.lock();
        img=img1;
        lock.unlock();
    }
    void set_title(QString t)
    {
        lock.lock();
        title=t;
        lock.unlock();
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        // if(img1.width()>0){
         if(1){
//            QThread::msleep(10);
             lock.lock();
                // QThread::msleep(2000);
//            frame_rate++;
            QPainter painter(this);
            if(!img.isNull()){
                //     prt(info,"%d %d",img.byteCount(),img.bytesPerLine());
                painter.drawImage(QRect(0,0,this->width(),this->height()),img);
                //      bool active=painter.isActive();
                pt->paint(&painter);


            }

//            painter.drawText(QPointF(111,111),title);

//#if 0
//            QBrush blue_brush_trans(QColor(0,222,200,255));
//            painter.setBrush(blue_brush_trans);

//            //   painter.drawRect(0,0,this->width(),this->height());
//            painter.drawRect(100,100,300,300);
//#endif
             lock.unlock();
         }
    }
    void  initializeGL()
    {

    }

public slots:
    void set_layout_data(QByteArray data)
    {
    //    lock.lock();
        pt->set_data(data);
    //    lock.unlock();
    }

    void check_rate()
    {
        //frame_rate++;
        prt(info,"frame rate :%d ",frame_rate);
        frame_rate=0;
    }

    void mouseDoubleClickEvent(QMouseEvent *)
    {
        emit selected(this);
    }
signals:
    void selected(PlayerWidget *w);

private:
    QImage img;
    QString title;
    QMutex lock;
    int frame_rate;
    LayoutPainter *pt;
    QPainter painter;
    //    QByteArray ba;
};

#endif // PLAYERWIDGET_H
