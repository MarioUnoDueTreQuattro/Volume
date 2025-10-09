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
#include "windowbutton.h"
#include "nativehotkeymanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
private:
    Ui::Widget *ui;
    SystemVolumeController *m_systemVolumeController;
    bool m_bIsOnTop;
    bool m_bIsTransparent;
    qreal m_dOpacity;
    qreal m_dLastOpacity;
    bool mousePressed;
    bool resizing;
    int resizeMargin;
    int topButtonsMargin;
    int m_iButtonSize;
    void updateMuteButtonIcon();
    void saveSettings();
    void loadSettings();
    QPoint mouseStartPos;
    QPoint windowStartPos;
    WindowButton *closeButton;
    WindowButton *minimizeButton;
    WindowButton *onTopButton;
    WindowButton *transparentButton;
    // QPushButton *closeButton;
    // QPushButton *minimizeButton;
    // QPushButton *onTopButton;
    // QPushButton *transparentButton;
    QRect windowStartRect;
    Qt::Edges resizeDirection;
    void updateCursorShape(const QPoint &pos);
    void updateButtons();
    void handleVolumeDown();
    void handleVolumeUp();
    
protected:
    void moveEvent(QMoveEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
private slots:
    void onHotkeyAssigned(const QString &name, const QString &sequence);
    void onOpacityActionTriggered();
    void showOpacityContextMenu(const QPoint &pos);
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
