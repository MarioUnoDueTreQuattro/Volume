#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QToolTip>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_systemVolumeController = new SystemVolumeController(this);
    connect(m_systemVolumeController, SIGNAL(volumeChanged(float)),
        this, SLOT(onSystemVolumeChanged(float)));
    connect(m_systemVolumeController, SIGNAL(muteStateChanged(bool)),
        this, SLOT(onSystemMuteChanged(bool)));
    // connect(m_systemVolumeController, SIGNAL(defaultDeviceChanged()),
    // this, SLOT(onDefaultDeviceChanged()));
    connect(m_systemVolumeController, SIGNAL(defaultDeviceChanged(QString, QString)),
        this, SLOT(onDeviceChanged(QString, QString)));
        int iVol = int (m_systemVolumeController->volume () * 100.01f);
        qDebug() << "Current master volume:" << iVol;
        QSignalBlocker blocker(ui->volumeSlider);
        ui->volumeSlider->setValue (iVol);
        ui->volumeLabel->setText(QString::number(iVol) + "%");
        ui->muteButton->setChecked (m_systemVolumeController->isMuted ());
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onSystemVolumeChanged(float newVolume)
{
    qDebug() << "System volume changed: " << newVolume;
        int iVol = int (m_systemVolumeController->volume () * 100.01f);
        qDebug() << "Current master volume:" << iVol;
        QSignalBlocker blocker(ui->volumeSlider);
        ui->volumeSlider->setValue (iVol);
        ui->volumeLabel->setText(QString::number(iVol) + "%");
}

void Widget::onSystemMuteChanged(bool muted)
{
    qDebug() << "System mute state changed: " << muted;
        updateMuteButtonIcon ();
        if (muted) ui->volumeLabel->setText("0%");
}

void Widget::onDeviceChanged(const QString &deviceId, const QString &friendlyName)
{
    qDebug() << __PRETTY_FUNCTION__ << "Device name: " << friendlyName;
    QString sToolTipMessage = "Default audio playback device changed to:<br><b>" + friendlyName + "</b>";
    QPoint globalPos = ui->volumeSlider->mapToGlobal(QPoint(ui->volumeSlider->width() / 2, ui->volumeSlider->height() / 2));
    QToolTip::showText(globalPos, sToolTipMessage, ui->volumeSlider);
    //QMessageBox::warning (this, "Audio device changed", "Default audio playback device changed.");
    bool wasPlaying = false;
    m_systemVolumeController->blockSignals(true);
    m_systemVolumeController->cleanup();
    m_systemVolumeController->initialize();
    m_systemVolumeController->blockSignals(false);
    // Sync slider/label with new device volume
    //handleVolumeChanged(int(m_systemVolumeController->volume() * 100.0f));
    onSystemVolumeChanged(m_systemVolumeController->volume());
}

void Widget::onDefaultDeviceChanged()
{
    qDebug() << "Widget: Detected new default output device";
    // Recreate QAudioOutput or QMediaPlayer
    // delete m_audioOutput;
    // m_audioOutput = new QAudioOutput(this);
    // m_player->setAudioOutput(m_audioOutput);
    // 2. Reinitialize SystemVolumeController safely
    m_systemVolumeController->blockSignals(true);
    m_systemVolumeController->cleanup();
    m_systemVolumeController->initialize();
    m_systemVolumeController->blockSignals(false);
    // Sync slider/label with new device volume
    //handleVolumeChanged(int(m_systemVolumeController->volume() * 100.0f));
    // Refresh slider & label with new device volume
    onSystemVolumeChanged(m_systemVolumeController->volume());
    QMessageBox::warning (this, "Audio device changed", "Default audio playback device changed.");
}

void Widget::updateMuteButtonIcon()
{
        bool bMuted = m_systemVolumeController->isMuted ();
        if (bMuted)
        {
            ui->muteButton->setChecked (true);
            ui->muteButton->setIcon(QIcon(":/img/img/icons8-mute-48.png"));
        }
        else
        {
            ui->muteButton->setChecked (false);
            ui->muteButton->setIcon(QIcon(":/img/img/icons8-sound-48.png"));
        }

}

void Widget::on_volumeSlider_valueChanged(int value)
{
        float fVol = float (float(value) / 100.0f);
        int iVol = int (fVol * 100.01f);
        // qDebug() << "Current master volume f:" << fVol;
        // qDebug() << "Current master volume i:" << iVol;
        m_systemVolumeController->setVolume (fVol);
        ui->volumeSlider->setValue (iVol);
        ui->volumeLabel->setText(QString::number(iVol) + "%");
}
