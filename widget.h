#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "systemvolumecontroller.h"

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
    void updateMuteButtonIcon();
private slots:
    void onSystemVolumeChanged(float newVolume);
    void onSystemMuteChanged(bool muted);
    void onDefaultDeviceChanged();
    void onDeviceChanged(const QString &deviceId, const QString &friendlyName);

    void on_volumeSlider_valueChanged(int value);
};
#endif // WIDGET_H
