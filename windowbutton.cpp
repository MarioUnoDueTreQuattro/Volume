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
    //setFixedWidth (12);
    //setMinimumWidth (12);
    setMouseTracking (true);
    setFocusPolicy (Qt::NoFocus);
    //QSizePolicy sizePolicy = this->sizePolicy();
    //    // Imposta la policy orizzontale su 'Minimum'.
    //    // Questo dice al layout di usare il setMinimumWidth() come limite inferiore.
    //    // L'altezza (Vertical) viene lasciata su Preferred o come è.
    // sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
    //QFont defFont=this->font ();
    //defFont.setPointSize (8);
    //this->setFont (defFont);
}

void WindowButton::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << __PRETTY_FUNCTION__;
    setCursor(Qt::ArrowCursor);
    emit mouseMoved();
    QPushButton::mouseMoveEvent(event);
}
