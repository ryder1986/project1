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
    LayoutPainter(  QList <QPoint> ps):pns(ps),wi_ori(640),he_ori(480)
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
            //  prt(info,"-->%s",str.toStdString().data());


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
            window_w=test->window().width();
            window_h=test->window().height();

            foreach (QString s, list) {
                l=s.split(',');
                QRect r;

                if(l.size()==4){
                    r.setRect(l[0].toInt()*window_w/wi_ori*2,l[1].toInt()*window_h/he_ori*2,l[2].toInt()*window_w/wi_ori*2,
                            l[3].toInt()*window_h/he_ori*2);
                    // rcts.append(r);
                    //           test->isActive();
                    test->drawRect(r);
                }else
                    if(l.size()==2){
                        int wid=l[0].toInt();
                        int hei=l[1].toInt();
                        //   prt(info,"(get %d %d)",wid,hei);
                        wi_ori=wid;
                        he_ori=hei;

                    }

            }


            data.clear();
        }
    }
    void try_pick(QPoint point)
    {
        int i=0;
        if(pns_now.size()==4)
            for (i=0;i<4;i++) {
                if(abs(point.x()-pns_now[i].x())<10&&abs(point.y()-pns_now[i].y())<10){
                    prt(info,"%d slect",i+1);
                    picked=true;
                    index=i;
                }
            }
    }

    bool try_move(QPoint point)
    {
        bool ret=false;
        if(picked){
            ret=true;
            pns_now[index]=point;
            set_points();
        }
        return ret;
    }
    QList <QPoint> points()
    {
        return pns;
    }

    bool try_put()
    {
        bool ret=false;
        if(picked){
            set_points();
            picked=false;
            index=-1;
            ret=true;
        }
        return ret;
    }
    QPoint get_points()
    {
        int i=0;
        //  pns_now.reserve(4);
        pns_now.clear();
        if(pns.size()==4){
            for(i=0;i<4;i++)
                pns_now.append((QPoint(pns[i].x()*window_w/wi_ori,pns[i].y()*window_h/he_ori)));
        }
    }
    void set_points()
    {
        int i=0;
        if(pns_now.size()==4){
            for(i=0;i<4;i++)
                pns[i]=QPoint(pns_now[i].x()*wi_ori/window_w,pns_now[i].y()*he_ori/window_h);
        }
    }

    void paint_rect(QPainter *pt)
    {
        window_w=pt->window().width();
        window_h=pt->window().height();
        get_points();
        if(pns.size()==4){
            //  prt(info,"ps ok");

            QBrush brush1(QColor(0,220,0,111));
            QPen pen(brush1,20);
            pt->setPen(pen);
            QPointF p[4];
            //            p[0]=pns[0];
            //            p[1]=pns[1];
            //            p[2]=pns[2];
            //            p[3]=pns[3];


            //            p[0]=get_point(pns[0],w,h);
            //            p[1]=get_point(pns[1],w,h);
            //            p[2]=get_point(pns[2],w,h);
            //            p[3]=get_point(pns[3],w,h);

            get_points();
            for(int i=0;i<4;i++){

                p[i]=pns_now[i];
            }



            //            p[0].setX(p[0].x()*w/wi); p[0].setY(p[0].y()*h/he);
            //            p[1].setX(p[1].x()*w/wi); p[1].setY(p[1].y()*h/he);
            //            p[2].setX(p[2].x()*w/wi); p[2].setY(p[2].y()*h/he);
            //            p[3].setX(p[3].x()*w/wi); p[3].setY(p[3].y()*h/he);

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
    QList <QPoint> pns_now;
    bool picked;
    int index;
    int wi_ori;
    int he_ori;
    int window_w;
    int window_h;
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
        //     prt(info,"frame rate :%d ",frame_rate);
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

    void mouseReleaseEvent(QMouseEvent *e )
    {
        if(pt->try_put()){
            emit rect_changed(pt->points());
        }
    }
    void mouseMoveEvent(QMouseEvent *e)
    {
        QList <QPoint>l;
        if(pt->try_move(e->pos())){
            l=pt->points();
        }
    }

signals:
    void selected(PlayerWidget *w);
    void rect_changed( QList <QPoint>);
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
