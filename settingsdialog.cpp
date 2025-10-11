#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    // 2. Remove the specific hints for minimize and maximize
    //flags |= Qt::Tool;
    setWindowFlags( Qt::MSWindowsFixedSizeDialogHint);
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowMinimizeButtonHint;
    setWindowFlags (flags);
    hotkeyMgr = NativeHotkeyManager::instance(this);
    hotkeyMgr->unregisterAllHotkeys ();
    loadHotKeys ();
    loadSettings();
    adjustSize();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveHotKeys()
{
    UINT mod = 0, vk = 0;
    QKeySequence seq = ui->keySequenceEditMute->keySequence();
    QString sequence = seq.toString(QKeySequence::NativeText);
    if (hotkeyMgr->parseKeySequence(sequence, mod, vk))
        hotkeyMgr->saveHotkeyConfig ("1", sequence);
    else qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "parseKeySequence==false";
    seq = ui->keySequenceEditOnTop->keySequence();
    sequence = seq.toString(QKeySequence::NativeText);
    if (hotkeyMgr->parseKeySequence(sequence, mod, vk))
        hotkeyMgr->saveHotkeyConfig ("2", sequence);
    else qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "parseKeySequence==false";
    seq = ui->keySequenceEditVolUp->keySequence();
    sequence = seq.toString(QKeySequence::NativeText);
    if (hotkeyMgr->parseKeySequence(sequence, mod, vk))
        hotkeyMgr->saveHotkeyConfig ("3", sequence);
    else qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "parseKeySequence==false";
    seq = ui->keySequenceEditVolDown->keySequence();
    sequence = seq.toString(QKeySequence::NativeText);
    if (hotkeyMgr->parseKeySequence(sequence, mod, vk))
        hotkeyMgr->saveHotkeyConfig ("4", sequence);
    else qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "parseKeySequence==false";
    seq = ui->keySequenceEditTransparent->keySequence();
    sequence = seq.toString(QKeySequence::NativeText);
    if (hotkeyMgr->parseKeySequence(sequence, mod, vk))
        hotkeyMgr->saveHotkeyConfig ("5", sequence);
    else qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "parseKeySequence==false";
    // settings.setValue(QString::number(1), sequence);
    // seq = ui->keySequenceEditOnTop->keySequence();
    // sequence = seq.toString(QKeySequence::NativeText);
    // settings.setValue(QString::number(2), sequence);
    // seq = ui->keySequenceEditVolUp->keySequence();
    // sequence = seq.toString(QKeySequence::NativeText);
    // settings.setValue(QString::number(3), sequence);
    // seq = ui->keySequenceEditVolDown->keySequence();
    // sequence = seq.toString(QKeySequence::NativeText);
    // settings.setValue(QString::number(4), sequence);
    // seq = ui->keySequenceEditTransparent->keySequence();
    // sequence = seq.toString(QKeySequence::NativeText);
    // settings.setValue(QString::number(5), sequence);
    // settings.beginGroup("Hotkeys");
    //settings.remove(""); // clear previous
    // for (auto id : hotkeys.keys())
}

void SettingsDialog::on_buttonBox_accepted()
{
    QSettings settings;
    settings.setValue ("TrayIcon", ui->checkBoxTray->isChecked ());
    settings.setValue ("OSD_Enabled", ui->checkBoxOSD->isChecked ());
    settings.setValue ("OSD_TextSize", ui->spinBoxOSDTextSize->value ());
    settings.setValue ("OSD_Duration", ui->spinBoxOSDDuration->value ());
    if (ui->radioButtonOSDCenter->isChecked ())
        settings.setValue ("OSD_Position", "Center");
    else if (ui->radioButtonOSDBottomRight->isChecked ())
        settings.setValue ("OSD_Position", "BottomRight");
    settings.sync ();
    saveHotKeys ();
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    bool bTray = settings.value ("TrayIcon", true).toBool ();
    ui->checkBoxTray->setChecked (bTray);
    bool bOSD_Enabled = settings.value ("OSD_Enabled", true).toBool ();
    int iTextSize = settings.value ("OSD_TextSize", 16).toInt ();
    int iDuration = settings.value ("OSD_Duration", 2000).toInt ();
    if (bOSD_Enabled)
    {
        ui->checkBoxOSD->setChecked(true);
        ui->spinBoxOSDTextSize->setEnabled (true);
        ui->spinBoxOSDDuration->setEnabled (true);
        ui->radioButtonOSDCenter->setEnabled (true);
        ui->radioButtonOSDBottomRight->setEnabled (true);
    }
    else
    {
        ui->checkBoxOSD->setChecked(false);
        ui->spinBoxOSDTextSize->setEnabled (false);
        ui->spinBoxOSDDuration->setEnabled (false);
        ui->radioButtonOSDCenter->setEnabled (false);
        ui->radioButtonOSDBottomRight->setEnabled (false);
    }
    ui->spinBoxOSDTextSize->setValue (iTextSize);
    ui->spinBoxOSDDuration->setValue (iDuration);
    QString sPosition = settings.value ("OSD_Position", "Center").toString ();
    if (sPosition == "Center") ui->radioButtonOSDCenter->setChecked (true);
    else if (sPosition == "BottomRight") ui->radioButtonOSDBottomRight->setChecked (true);
}

void SettingsDialog::loadHotKeys()
{
    QString sequence;
    UINT modifiers = 0, vk = 0;
    sequence = hotkeyMgr->loadHotkeyConfig ("1").trimmed ();
    if (!hotkeyMgr->parseKeySequence(sequence, modifiers, vk))
    {
        qDebug() << "Skipping invalid hotkey sequence:" << sequence;
    }
    else
    {
        ui->keySequenceEditMute->setKeySequence (QKeySequence(sequence, QKeySequence::NativeText));
    }
    sequence = hotkeyMgr->loadHotkeyConfig ("2").trimmed ();
    if (!hotkeyMgr->parseKeySequence(sequence, modifiers, vk))
    {
        qDebug() << "Skipping invalid hotkey sequence:" << sequence;
    }
    else
    {
        ui->keySequenceEditOnTop->setKeySequence (QKeySequence(sequence, QKeySequence::NativeText));
    }
    sequence = hotkeyMgr->loadHotkeyConfig ("3").trimmed ();
    if (!hotkeyMgr->parseKeySequence(sequence, modifiers, vk))
    {
        qDebug() << "Skipping invalid hotkey sequence:" << sequence;
    }
    else
    {
        ui->keySequenceEditVolUp->setKeySequence (QKeySequence(sequence, QKeySequence::NativeText));
    }
    sequence = hotkeyMgr->loadHotkeyConfig ("4").trimmed ();
    if (!hotkeyMgr->parseKeySequence(sequence, modifiers, vk))
    {
        qDebug() << "Skipping invalid hotkey sequence:" << sequence;
    }
    else
    {
        ui->keySequenceEditVolDown->setKeySequence (QKeySequence(sequence, QKeySequence::NativeText));
    }
    sequence = hotkeyMgr->loadHotkeyConfig ("5").trimmed ();
    if (!hotkeyMgr->parseKeySequence(sequence, modifiers, vk))
    {
        qDebug() << "Skipping invalid hotkey sequence:" << sequence;
    }
    else
    {
        ui->keySequenceEditTransparent->setKeySequence (QKeySequence(sequence, QKeySequence::NativeText));
    }
}

void SettingsDialog::on_pushButtonResetVolUp_clicked()
{
    ui->keySequenceEditVolUp->clear ();
    ui->keySequenceEditVolUp->setFocus ();
}

void SettingsDialog::on_pushButtonResetVolDown_clicked()
{
    ui->keySequenceEditVolDown->clear ();
    ui->keySequenceEditVolDown->setFocus ();
}

void SettingsDialog::on_pushButtonResetMute_clicked()
{
    ui->keySequenceEditMute->clear ();
    ui->keySequenceEditMute->setFocus ();
}

void SettingsDialog::on_pushButtonResetTop_clicked()
{
    ui->keySequenceEditOnTop->clear ();
    ui->keySequenceEditOnTop->setFocus ();
}

void SettingsDialog::on_pushButtonResetTransparent_clicked()
{
    ui->keySequenceEditTransparent->clear ();
    ui->keySequenceEditTransparent->setFocus ();
}

void SettingsDialog::on_checkBoxOSD_stateChanged(int state)
{
    if (state == 0)
    {
        ui->spinBoxOSDTextSize->setEnabled (false);
        ui->spinBoxOSDDuration->setEnabled (false);
        ui->radioButtonOSDCenter->setEnabled (false);
        ui->radioButtonOSDBottomRight->setEnabled (false);
    }
    else
    {
        ui->spinBoxOSDTextSize->setEnabled (true);
        ui->spinBoxOSDDuration->setEnabled (true);
        ui->radioButtonOSDCenter->setEnabled (true);
        ui->radioButtonOSDBottomRight->setEnabled (true);
    }
}
