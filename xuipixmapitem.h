#ifndef XUIPIXMAPITEM_H
#define XUIPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

class XuiPixmapItem : public QGraphicsPixmapItem
{
public:
    XuiPixmapItem(QPixmap pixmap);
    void collide();
    QGraphicsItem * item;
private slots:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPointF presspos;

};

#endif // XUIPIXMAPITEM_H
