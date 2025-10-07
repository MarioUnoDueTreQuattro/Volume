#include "windowbutton.h"
#include <QDebug>

//WindowButton::WindowButton()
//{

//}
//CustomButton::CustomButton(const QString &text, QWidget *parent)
// : QPushButton(text, parent)
WindowButton::WindowButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
setMouseTracking (true);
}

void WindowButton::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << __PRETTY_FUNCTION__;
    setCursor(Qt::ArrowCursor);
    emit mouseMoved();
     QPushButton::mouseMoveEvent(event);
}
