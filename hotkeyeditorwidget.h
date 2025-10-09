#ifndef HOTKEYEDITORWIDGET_H
#define HOTKEYEDITORWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QKeySequenceEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class HotkeyEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HotkeyEditorWidget(QWidget *parent = nullptr);

signals:
    void hotkeyAssigned(const QString &name, const QString &sequence);

private slots:
    void assignHotkey();

private:
    QLineEdit *nameEdit;
    QKeySequenceEdit *sequenceEdit;
    QPushButton *assignButton;
};

#endif // HOTKEYEDITORWIDGET_H
