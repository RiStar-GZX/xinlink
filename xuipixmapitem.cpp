#include "xuipixmapitem.h"

XuiPixmapItem::XuiPixmapItem(QPixmap pixmap):QGraphicsPixmapItem(pixmap)
{
    //connect(this,!collidingItems().isEmpty(),this,collide());
}

void XuiPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    //printf("mouse move\n");
    QPointF p=event->scenePos();
    QSize s=pixmap().size();
    int x=p.x()-s.width()/2,y=p.y()-s.height()/2;
    QPointF p_old=pos();
    this->setPos(x,y);

    printf("p:%d %d\n",x,y);
    if(collidingItems().isEmpty()!=1){
        this->setPos(p_old.x(),p_old.y());
        //QList<QGraphicsItem *> s=collidingItems();

       // item=s.first();
    }
    else item=NULL;
}

void XuiPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    printf("mouse press\n");
    setCursor(Qt::ClosedHandCursor);
}

void XuiPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    printf("mouse release\n");
    setCursor(Qt::OpenHandCursor);
}

void XuiPixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){

}

void XuiPixmapItem::collide(){
    printf("collide!\n");
}
