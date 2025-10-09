#include "hotkeyeditorwidget.h"

HotkeyEditorWidget::HotkeyEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    nameEdit = new QLineEdit(this);
    sequenceEdit = new QKeySequenceEdit(this);
    assignButton = new QPushButton("Assign", this);

    QLabel *nameLabel = new QLabel("Hotkey Name:", this);
    QLabel *sequenceLabel = new QLabel("Key Sequence:", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);
    layout->addWidget(sequenceLabel);
    layout->addWidget(sequenceEdit);
    layout->addWidget(assignButton);

    connect(assignButton, SIGNAL(clicked()), this, SLOT(assignHotkey()));
}

void HotkeyEditorWidget::assignHotkey()
{
    QString name = nameEdit->text().trimmed();
    QString sequence = sequenceEdit->keySequence().toString(QKeySequence::NativeText);

    if (!name.isEmpty() && !sequence.isEmpty())
        emit hotkeyAssigned(name, sequence);
}
