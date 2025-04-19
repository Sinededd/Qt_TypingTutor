#include "keybutton.h"

KeyButton::KeyButton(QGraphicsScene *scene, QRectF rect, QChar letter, QGraphicsItem *parent) : QGraphicsItem(parent)
{
    this->scene = scene;
    this->rect = rect;
    this->letter = letter;
    this->setPos((scene->width() - 660) / 2, scene->height() / 2 + 20);
    // timer = new QTimer();
}

void KeyButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPointF currentPos = rect.topLeft();

    if(isPressed == 1)
    {
        // painter->setBrush(QBrush("hotpink"));
        // painter->setPen(QPen("hotpink"));
        painter->setBrush(QBrush(QColor(150, 150, 150, 200)));
        painter->setPen(QPen(QColor(150, 150, 150, 200)));
    }
    else
    {
        // painter->setBrush(QBrush(QColor(178, 52, 146)));
        // painter->setPen(QPen(QColor(178, 52, 146)));
        painter->setBrush(QBrush(QColor(150, 150, 150, 50)));
        painter->setPen(QPen(QColor(150, 150, 150, 50)));
    }

    painter->drawRoundedRect(QRectF(currentPos, rect.size()), 5, 5);

    QFont font("Helvetica", 20);
    painter->setBrush(QBrush("white"));
    painter->setPen(QPen("white"));
    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawText(QPointF(currentPos.x() + (rect.size().width() - font.pointSizeF()) / 2,
                              currentPos.y() + (rect.size().width() + font.pointSizeF()) / 2), letter);

    // painter->drawRect(this->boundingRect());
}

QRectF KeyButton::boundingRect() const
{
    return rect;
}

void KeyButton::pressOn()
{
    isPressed = 1;
    scene->update();
    QTimer::singleShot(50, [this]() -> void {
        isPressed = 0;
        scene->update();
    });
}

