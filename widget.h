#ifndef WIDGET_H
#define WIDGET_H

#include "systemvolumecontroller.h"
#include <QWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QPushButton>
#include <QPainter>
#include <QPen>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    SystemVolumeController *m_systemVolumeController;
    bool mousePressed;
    bool resizing;
    int resizeMargin;
        int topButtonsMargin;
void updateMuteButtonIcon();
    void saveSettings();
    void loadSettings();
    QPoint mouseStartPos;
    QPoint windowStartPos;
    QPushButton *closeButton;
    QPushButton *minimizeButton;
    QPushButton *onTopButton;
    QPushButton *transparentButton;
    QRect windowStartRect;
    Qt::Edges resizeDirection;
    void updateCursorShape(const QPoint &pos);
protected:
    void moveEvent(QMoveEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onSystemVolumeChanged(float newVolume);
    void onSystemMuteChanged(bool muted);
    void onDefaultDeviceChanged();
    void onDeviceChanged(const QString &deviceId, const QString &friendlyName);
    void on_volumeSlider_valueChanged(int value);
    void on_muteButton_clicked();
    void toggleOnTop();
 void toggleTransparency();
 };
#endif // WIDGET_H
