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
    adjustSize();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    // QSettings settings;
    // settings.beginGroup("Hotkeys");
    //settings.remove(""); // clear previous
    // for (auto id : hotkeys.keys())
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
    // settings.sync ();
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
