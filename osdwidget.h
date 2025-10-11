#ifndef OSDWIDGET_H
#define OSDWIDGET_H

#include <QWidget>

class QLabel;
class QPropertyAnimation;
class QTimer;

class OSDWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OSDWidget(QWidget *parent = nullptr);
    ~OSDWidget();
    // Show text for visibleMs milliseconds (fade in/out handled automatically).
    void showMessage(const QString &text, int visibleMs = 2000);
    // Optional: enable/disable click-through (Windows only).
    void setClickThrough(bool enable);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onFadeInFinished();
    void onVisibilityTimeout();
    void onFadeOutFinished();

private:
    void positionAtBottomRight();
    void positionAtCenter();
    QLabel *m_label;
    QPropertyAnimation *m_fadeIn;
    QPropertyAnimation *m_fadeOut;
    QTimer *m_visibilityTimer;
    int m_visibleMs;
    bool m_clickThrough;
};

#endif // OSDWIDGET_H
