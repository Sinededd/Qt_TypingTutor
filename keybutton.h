#ifndef KEYBUTTON_H
#define KEYBUTTON_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QTimer>

class KeyButton : public QGraphicsItem
{
public:
    KeyButton(QGraphicsScene *scene, QRectF rect, QChar letter,  QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

    void pressOn();
    void pressOff();


private:
    bool isPressed = 0;

    QGraphicsScene *scene;
    QRectF rect;
    QChar letter;
};

#endif // KEYBUTTON_H
