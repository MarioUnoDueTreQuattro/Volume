// hotkeyeditor.cpp
#include "hotkeyeditor.h"
#include "ui_hotkeyeditor.h"
#include <QSettings>
#include <QDebug>
#include <Windows.h>

HotkeyEditor::HotkeyEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HotkeyEditor)
{
    ui->setupUi(this);

    // Initialize NativeHotkeyManager on this dialog
    hotkeyMgr = NativeHotkeyManager::instance(this);

    setupTable();
    loadHotkeys();

    // Double click on table row to edit hotkey
    connect(ui->hotkeyTable, &QTableWidget::cellDoubleClicked, this, [=](int row, int){
        QTableWidgetItem *item = ui->hotkeyTable->item(row, 0);
        if (item)
            ui->hotkeyEdit->setKeySequence(QKeySequence(item->text()));
    });
}

HotkeyEditor::~HotkeyEditor()
{
    saveHotkeys();
    hotkeyMgr->unregisterAllHotkeys();
    delete ui;
}

// --- Table setup ---
void HotkeyEditor::setupTable()
{
    ui->hotkeyTable->setColumnCount(2);
    ui->hotkeyTable->setHorizontalHeaderLabels(QStringList() << "Hotkey" << "Action");
    ui->hotkeyTable->setSelectionBehavior(QTableWidget::SelectRows);
    ui->hotkeyTable->setEditTriggers(QTableWidget::NoEditTriggers);
}

// --- Add entry to table ---
void HotkeyEditor::addHotkeyToTable(const HotkeyEntry &entry)
{
    int row = ui->hotkeyTable->rowCount();
    ui->hotkeyTable->insertRow(row);
    ui->hotkeyTable->setItem(row, 0, new QTableWidgetItem(entry.sequence));
    ui->hotkeyTable->setItem(row, 1, new QTableWidgetItem("Action " + QString::number(entry.id)));
    ui->hotkeyTable->resizeColumnsToContents();
    ui->hotkeyTable->resizeRowsToContents();
}

// --- Register a new hotkey ---
void HotkeyEditor::on_btnRegister_clicked()
{
    QKeySequence seq = ui->hotkeyEdit->keySequence();
    QString sequence = seq.toString(QKeySequence::NativeText);
    if (sequence.isEmpty())
        return;

    UINT modifiers = 0;
    UINT vk = 0;
    if (!hotkeyMgr->parseKeySequence(sequence, modifiers, vk))
        return;

    int hotkeyId = nextHotkeyId++;
    HWND hwnd = reinterpret_cast<HWND>(this->winId());

    if (!RegisterHotKey(hwnd, hotkeyId, modifiers, vk))
    {
        qDebug() << "Failed to register hotkey:" << sequence << "Error:" << GetLastError();
        return;
    }

    HotkeyEntry entry;
    entry.id = hotkeyId;
    entry.sequence = sequence;
    entry.action = [sequence](){ qDebug() << "Hotkey triggered:" << sequence; };

    hotkeys[hotkeyId] = entry;
    hotkeyMgr->bindAction(hotkeyId, entry.action);
    addHotkeyToTable(entry);
    saveHotkeys();
    qDebug() << "Hotkey registered:" << sequence;
}

// --- Remove selected hotkey ---
void HotkeyEditor::on_btnRemove_clicked()
{
    auto ranges = ui->hotkeyTable->selectedRanges();
    if (ranges.isEmpty())
        return;

    int row = ranges.first().topRow();
    QTableWidgetItem *item = ui->hotkeyTable->item(row, 0);
    if (!item)
        return;

    QString sequence = item->text();
    int hotkeyId = -1;
    for (auto id : hotkeys.keys())
        if (hotkeys[id].sequence == sequence) { hotkeyId = id; break; }

    if (hotkeyId == -1)
        return;

    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    UnregisterHotKey(hwnd, hotkeyId);
    hotkeyMgr->bindAction(hotkeyId, nullptr);
    hotkeys.remove(hotkeyId);
    ui->hotkeyTable->removeRow(row);
    saveHotkeys();
    qDebug() << "Hotkey removed:" << sequence;
}

// --- Update selected hotkey ---
void HotkeyEditor::on_btnUpdate_clicked()
{
    auto ranges = ui->hotkeyTable->selectedRanges();
    if (ranges.isEmpty())
        return;

    int row = ranges.first().topRow();
    QTableWidgetItem *item = ui->hotkeyTable->item(row, 0);
    if (!item)
        return;

    QString oldSequence = item->text();

    int hotkeyId = -1;
    for (auto id : hotkeys.keys())
        if (hotkeys[id].sequence == oldSequence) { hotkeyId = id; break; }

    if (hotkeyId == -1)
        return;

    QKeySequence seq = ui->hotkeyEdit->keySequence();
    QString newSequence = seq.toString(QKeySequence::NativeText);
    if (newSequence.isEmpty())
        return;

    UINT modifiers = 0, vk = 0;
    if (!hotkeyMgr->parseKeySequence(newSequence, modifiers, vk))
        return;

    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    UnregisterHotKey(hwnd, hotkeyId);

    if (RegisterHotKey(hwnd, hotkeyId, modifiers, vk))
    {
        hotkeys[hotkeyId].sequence = newSequence;
        hotkeys[hotkeyId].action = [newSequence]() { qDebug() << "Hotkey triggered:" << newSequence; };
        hotkeyMgr->bindAction(hotkeyId, hotkeys[hotkeyId].action);
        item->setText(newSequence);
        ui->hotkeyTable->resizeColumnsToContents();
        saveHotkeys();
        qDebug() << "Hotkey updated:" << oldSequence << "→" << newSequence;
    }
    else
    {
        UINT oldMod = 0, oldVk = 0;
        hotkeyMgr->parseKeySequence(oldSequence, oldMod, oldVk);
        RegisterHotKey(hwnd, hotkeyId, oldMod, oldVk);
        qDebug() << "Failed to update hotkey, old one restored. Error:" << GetLastError();
    }
}

// --- Save all hotkeys to QSettings ---
void HotkeyEditor::saveHotkeys()
{
    QSettings settings;
    settings.beginGroup("Hotkeys");
    settings.remove(""); // clear previous
    for (auto id : hotkeys.keys())
        settings.setValue(QString::number(id), hotkeys[id].sequence);
    settings.endGroup();
}

// --- Load hotkeys from QSettings ---
void HotkeyEditor::loadHotkeys()
{
    ui->hotkeyTable->clearContents();
    ui->hotkeyTable->setRowCount(0);

    QSettings settings;
    settings.beginGroup("Hotkeys");
    QStringList keys = settings.childKeys();

    for (const QString &name : keys)
    {
        QString sequence = settings.value(name).toString().trimmed();
        if (sequence.isEmpty())
            continue;

        UINT modifiers = 0, vk = 0;
        if (!hotkeyMgr->parseKeySequence(sequence, modifiers, vk))
        {
            qDebug() << "Skipping invalid hotkey sequence:" << sequence;
            continue;
        }

        int hotkeyId = name.toInt();
        HWND hwnd = reinterpret_cast<HWND>(this->winId());
        if (!RegisterHotKey(hwnd, hotkeyId, modifiers, vk))
        {
            qDebug() << "Failed to register hotkey:" << sequence << "Error:" << GetLastError();
            continue;
        }

        HotkeyEntry entry;
        entry.id = hotkeyId;
        entry.sequence = sequence;
        entry.action = [sequence]() { qDebug() << "Hotkey pressed:" << sequence; };
        hotkeys[hotkeyId] = entry;
        hotkeyMgr->bindAction(hotkeyId, entry.action);
        addHotkeyToTable(entry);
    }

    settings.endGroup();
}
