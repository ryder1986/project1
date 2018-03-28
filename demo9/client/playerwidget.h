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
    LayoutPainter(  QList <QPoint> ps):pns(ps)
    {
        index=-1;
        data.clear();
        picked=false;

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
    void try_pick(QPoint point)
    {
        int i=0;
        for (i=0;i<4;i++) {
            if(abs(point.x()-pns[i].x())<10&&abs(point.y()-pns[i].y())<10){
                prt(info,"%d slect",i+1);
                picked=true;
                index=i;
            }
        }
    }

    void try_move(QPoint point)
    {
            if(picked){
                pns[index]=point;
            }
    }
    void try_put()
    {   picked=false;

    }

    void paint_rect(QPainter *pt)
    {
        if(pns.size()==4){
            prt(info,"ps ok");

            QBrush brush1(QColor(0,220,0,111));
            QPen pen(brush1,20);
            pt->setPen(pen);
            QPointF p[4];
            p[0]=pns[0];
            p[1]=pns[1];
            p[2]=pns[2];
            p[3]=pns[3];
              pt->drawPolyline((QPointF *)p,4);
        }else{
            prt(info,"ps err");
        }
    }
    void set_data(QByteArray ba)
    {
        data=ba;
    }

private:
   // QPainter *p;
    QByteArray data;
    QList <QPoint> pns;
    bool picked;
    int index;
};

class PlayerWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    PlayerWidget( QList <QPoint> ps):painter(this),show_info(false)
    {


        pt=new LayoutPainter(ps);
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
    void set_show(bool flg)
    {
        show_info=flg;
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

            if(show_info){
                pt->paint_rect(&painter);
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
    void mousePressEvent(QMouseEvent *e)
    {
        pt->try_pick(e->pos());
    }
    void mouseReleaseEvent()
    {
            pt->try_put();
    }
    void mouseMoveEvent(QMouseEvent *e)
    {
        pt->try_move(e->pos());
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
    QList <QPoint> area_v;
    bool show_info;
  //  QList <QPoint> points;
    //    QByteArray ba;
};

#endif // PLAYERWIDGET_H
