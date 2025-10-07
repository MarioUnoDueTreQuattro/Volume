#ifndef WINDOWBUTTON_H
#define WINDOWBUTTON_H

#include <QPushButton>
#include <QObject>
#include <QMouseEvent>

class WindowButton : public QPushButton
{
    Q_OBJECT
public:
    explicit WindowButton(const QString &text, QWidget *parent = nullptr);
protected:
    // void moveEvent(QMoveEvent *event) override;
    // void closeEvent(QCloseEvent *event) override;
    // void resizeEvent(QResizeEvent *event) override;
    // void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
signals:
    void mouseMoved();
};

#endif // WINDOWBUTTON_H
