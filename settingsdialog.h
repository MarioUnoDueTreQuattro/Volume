#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "nativehotkeymanager.h"

namespace Ui
{
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void on_buttonBox_accepted();
    void on_pushButtonResetVolUp_clicked();
    void on_pushButtonResetVolDown_clicked();
    void on_pushButtonResetMute_clicked();
    void on_pushButtonResetTop_clicked();
    void on_pushButtonResetTransparent_clicked();
private:
    Ui::SettingsDialog *ui;
    NativeHotkeyManager *hotkeyMgr;
    void loadHotKeys();
};

#endif // SETTINGSDIALOG_H
