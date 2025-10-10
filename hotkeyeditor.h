#ifndef HOTKEYEDITOR_H
#define HOTKEYEDITOR_H

// hotkeyeditor.h
#pragma once
#include <QDialog>
#include <QTableWidgetItem>
#include "nativehotkeymanager.h"

namespace Ui { class HotkeyEditor; }

struct HotkeyEntry {
    int id;
    QString sequence;
    std::function<void()> action;
};

class HotkeyEditor : public QDialog
{
    Q_OBJECT
public:
    explicit HotkeyEditor(QWidget *parent = nullptr);
    ~HotkeyEditor();

private slots:
    void on_btnRegister_clicked();
    void on_btnRemove_clicked();
    void on_btnUpdate_clicked();
private:
    Ui::HotkeyEditor *ui;
    NativeHotkeyManager *hotkeyMgr;
    int nextHotkeyId = 1;
    QMap<int, HotkeyEntry> hotkeys; // id -> hotkey info
    void addHotkeyToTable(const HotkeyEntry &entry);
    void setupTable();
    void loadHotkeys();
    void saveHotkeys();
};

#endif // HOTKEYEDITOR_H
