#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QToolTip>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QTimer>
#include "hotkeyeditor.h"
#include "settingsdialog.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget), mousePressed(false), resizing(false), resizeMargin(4), topButtonsMargin(3)
{
    ui->setupUi(this);
    setWindowTitle ("Volume");
    //     // Remove the minimize and maximize/restore buttons, leaving only the close button.
    //    // The window will still have its title bar.
    // setWindowFlags(
    // Qt::Window |
    // Qt::WindowMinimizeButtonHint | // This flag is *kept*
    // Qt::WindowMaximizeButtonHint | // This flag is *kept*
    // Qt::CustomizeWindowHint |      // Tells Qt to ignore the system default flags
    // Qt::WindowCloseButtonHint      // Keep the close button
    // );
    Qt::WindowFlags flags = windowFlags();
    // 2. Remove the specific hints for minimize and maximize
    //flags |= Qt::Tool;
    //flags &= ~Qt::WindowMinimizeButtonHint;
    flags &= ~Qt::WindowMaximizeButtonHint;
    //flags &= ~ Qt::WindowCloseButtonHint;
    // 3. Set the modified flags back
    setWindowFlags(flags);
    setWindowFlags(Qt::FramelessWindowHint/* | Qt::Window*/);
    // winId(); // forza la creazione del HWND
    //setAttribute(Qt::WA_NativeWindow); // forza la creazione nativa
    //setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint);
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
    // bool bMuted = m_systemVolumeController->isMuted ();
    // if (bMuted)
    // {
    // ui->muteButton->setChecked (true);
    // }
    // else
    // {
    // ui->muteButton->setChecked (false);
    // }
    updateMuteButtonIcon ();
    m_iButtonSize = 12;
    closeButton = new WindowButton("", this);
    closeButton->setGeometry(width() - m_iButtonSize, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    closeButton->setStyleSheet("QPushButton { border: none; }");
    closeButton->setIconSize (QSize(m_iButtonSize, m_iButtonSize));
    closeButton->setIcon (QIcon(":/img/img/Close.png"));
    minimizeButton = new WindowButton("", this);
    minimizeButton->setGeometry(width() - m_iButtonSize * 2, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    minimizeButton->setStyleSheet("QPushButton { border: none; }");
    minimizeButton->setIconSize (QSize(m_iButtonSize, m_iButtonSize));
    minimizeButton->setIcon (QIcon(":/img/img/Minimize.png"));
    onTopButton = new WindowButton("", this);
    onTopButton->setGeometry(width() - m_iButtonSize * 3, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    onTopButton->setStyleSheet("QPushButton { border: none; }");
    onTopButton->setIconSize (QSize(m_iButtonSize, m_iButtonSize));
    onTopButton->setIcon (QIcon(":/img/img/Pin.png"));
    transparentButton = new WindowButton("", this);
    transparentButton->setGeometry(width() - m_iButtonSize * 4, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    transparentButton->setStyleSheet("QPushButton { border: none; }");
    transparentButton->setIconSize (QSize(m_iButtonSize, m_iButtonSize));
    transparentButton->setIcon (QIcon(":/img/img/Transparent.png"));
    connect(transparentButton, SIGNAL(clicked()), this, SLOT(toggleTransparency()));
    connect(onTopButton, SIGNAL(clicked()), this, SLOT(toggleOnTop()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(minimizeButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    closeButton->setToolTip ("Close");
    minimizeButton->setToolTip ("Minimize");
    onTopButton->setToolTip ("On top");
    transparentButton->setToolTip ("Transparent");
    closeButton->setFixedSize (m_iButtonSize, m_iButtonSize);
    minimizeButton->setFixedSize (m_iButtonSize, m_iButtonSize);
    onTopButton->setFixedSize (m_iButtonSize, m_iButtonSize);
    transparentButton->setFixedSize (m_iButtonSize, m_iButtonSize);
    transparentButton->setContextMenuPolicy (Qt::CustomContextMenu);
    connect(transparentButton, &QPushButton::customContextMenuRequested, this, &Widget::showOpacityContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &Widget::showContextMenu);
    loadSettings ();
    // adjustSize();
    //QTimer::singleShot(1000, this, SLOT(initializeHotkeyManager()));
    hotkeyMgr = NativeHotkeyManager::instance(this);
    // hotkeyMgr->setOrganizationName (qApp->organizationName ());
    // hotkeyMgr->setApplicationName (qApp->applicationName ());
    // manager->saveHotkey("1", "Meta+Alt+M");
    // manager->saveHotkey("2", "Ctrl+Alt+T");
    // manager->saveHotkey("3", "Meta+Alt++");
    // manager->saveHotkey("4", "Meta+Alt+-");
    // HotkeyEditor *hot=new HotkeyEditor(this);
    // hot->exec ();
    hotkeyMgr->bindAction(1, std::bind(&Widget::on_muteButton_clicked, this));
    hotkeyMgr->bindAction(2, std::bind(&Widget::toggleOnTop, this));
    hotkeyMgr->bindAction(3, std::bind(&Widget::handleVolumeUp, this));
    hotkeyMgr->bindAction(4, std::bind(&Widget::handleVolumeDown, this));
    hotkeyMgr->bindAction(5, std::bind(&Widget::toggleTransparency, this));
    hotkeyMgr->loadHotkeys();
    qDebug() << "Hotkey actions registered:" << hotkeyMgr->registeredHotkeyNames();
    // QDialog *dialog = new QDialog(this);
    // dialog->setWindowTitle("Hotkey Editor");
    // QVBoxLayout *layout = new QVBoxLayout(dialog);
    // HotkeyEditorWidget *editor = new HotkeyEditorWidget(dialog);
    // layout->addWidget(editor);
    // connect(editor, SIGNAL(hotkeyAssigned(QString, QString)),
    // this, SLOT(onHotkeyAssigned(QString, QString)));
    // dialog->exec(); // blocca finché non chiudi
    // testHotKey(manager);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *event)
{
    hotkeyMgr-> unregisterAllHotkeys();
    saveSettings ();
    event->accept();
    QWidget::closeEvent(event);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    // 1. Chiami la versione base del gestore di eventi
    QWidget::resizeEvent (event);
    closeButton->setGeometry(width() - m_iButtonSize, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    minimizeButton->setGeometry(width() - m_iButtonSize * 2, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    onTopButton->setGeometry(width() - m_iButtonSize * 3, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    transparentButton->setGeometry(width() - m_iButtonSize * 4, topButtonsMargin, m_iButtonSize, m_iButtonSize);
    // 2. Imposti il vincolo di larghezza
    // this->setFixedWidth (48);
    // 3. ✨ Forza il genitore ad aggiornare la sua dimensione preferita
    // Questo costringe Qt a ricalcolare i layout, tenendo conto del setFixedWidth
    // appena impostato.
    // this->updateGeometry(); // O, in alternativa,
    // this->adjustSize();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << __PRETTY_FUNCTION__;
    QRect rect = this->rect();
    QPoint pos = event->pos();
    if (pos.x() <= resizeMargin && pos.y() <= resizeMargin)
        setCursor(Qt::SizeFDiagCursor); // Top-left
    else if (pos.x() >= rect.width() - resizeMargin && pos.y() <= resizeMargin)
        setCursor(Qt::SizeBDiagCursor); // Top-right
    else if (pos.x() <= resizeMargin && pos.y() >= rect.height() - resizeMargin)
        setCursor(Qt::SizeBDiagCursor); // Bottom-left
    else if (pos.x() >= rect.width() - resizeMargin && pos.y() >= rect.height() - resizeMargin)
        setCursor(Qt::SizeFDiagCursor); // Bottom-right
    else if (pos.x() <= resizeMargin)
        setCursor(Qt::SizeHorCursor); // Left
    else if (pos.x() >= rect.width() - resizeMargin)
        setCursor(Qt::SizeHorCursor); // Right
    else if (pos.y() <= resizeMargin)
        setCursor(Qt::SizeVerCursor); // Top
    else if (pos.y() >= rect.height() - resizeMargin)
        setCursor(Qt::SizeVerCursor); // Bottom
    else
        setCursor(Qt::ArrowCursor); // Default
    // Store the position where the mouse was pressed
    if (event->button() == Qt::LeftButton)
    {
        mousePressed = true;
        mouseStartPos = event->globalPos();
        windowStartRect = geometry();
        // Detect if mouse is near border
        resizing = false;
        resizeDirection = Qt::Edges();
        if (pos.x() <= resizeMargin)
            resizeDirection |= Qt::LeftEdge;
        else if (pos.x() >= rect.width() - resizeMargin)
            resizeDirection |= Qt::RightEdge;
        if (pos.y() <= resizeMargin)
            resizeDirection |= Qt::TopEdge;
        else if (pos.y() >= rect.height() - resizeMargin)
            resizeDirection |= Qt::BottomEdge;
        if (resizeDirection != Qt::Edges())
            resizing = true;
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << __PRETTY_FUNCTION__;
    updateCursorShape(event->pos());
    QWidget *child = childAt(mapFromGlobal(QCursor::pos()));
    if (qobject_cast<QPushButton *>(child) )
    {
        setCursor(Qt::ArrowCursor);
        // return;
    }
    if (resizing)
    {
        QPoint delta = event->globalPos() - mouseStartPos;
        QRect newRect = windowStartRect;
        if (resizeDirection & Qt::LeftEdge)
            newRect.setLeft(windowStartRect.left() + delta.x());
        if (resizeDirection & Qt::RightEdge)
            newRect.setRight(windowStartRect.right() + delta.x());
        if (resizeDirection & Qt::TopEdge)
            newRect.setTop(windowStartRect.top() + delta.y());
        if (resizeDirection & Qt::BottomEdge)
            newRect.setBottom(windowStartRect.bottom() + delta.y());
        // Enforce minimum size
        if (newRect.width() >= minimumWidth() && newRect.height() >= minimumHeight())
            setGeometry(newRect);
    }
    else if (mousePressed)
    {
        QPoint delta = event->globalPos() - mouseStartPos;
        move(windowStartRect.topLeft() + delta);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << __PRETTY_FUNCTION__;
    updateCursorShape(event->pos());
    // Stop dragging when mouse is released
    if (event->button() == Qt::LeftButton)
    {
        mousePressed = false;
        resizing = false;
    }
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPen borderPen(Qt::darkGray);
    borderPen.setWidth(1); // Thin border
    painter.setPen(borderPen);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

void Widget::enterEvent(QEvent *event)
{
    if (m_bIsTransparent) this->setWindowOpacity (1.0);
}

void Widget::leaveEvent(QEvent *event)
{
    if (m_bIsTransparent) this->setWindowOpacity (m_dOpacity);
}

void Widget::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction *settingsAction = contextMenu.addAction(tr("Settings"));
    QAction *selectedAction = contextMenu.exec(this->mapToGlobal(pos));
    if (selectedAction == settingsAction)
    {
        // HotkeyEditor *hot=new HotkeyEditor(this);
        // hot->exec ();
        SettingsDialog settingsDialog;
        connect(&settingsDialog, SIGNAL(accepted()), this, SLOT(settingsDialogAccepted()));
        // connect(&settingsDialog, SIGNAL(applyClicked()), this, SLOT(settingsDialogAccepted()));
        //    // cd.setParent (this);
        settingsDialog.setWindowTitle("Settings");
        // NativeHotkeyManager *manager = NativeHotkeyManager::instance(this);
        // manager->unregisterAllHotkeys ();
        settingsDialog.exec();
        // hotkeyMgr->bindAction(1, std::bind(&Widget::on_muteButton_clicked, this));
        // hotkeyMgr->bindAction(2, std::bind(&Widget::toggleOnTop, this));
        // hotkeyMgr->bindAction(3, std::bind(&Widget::handleVolumeUp, this));
        // hotkeyMgr->bindAction(4, std::bind(&Widget::handleVolumeDown, this));
        // hotkeyMgr->bindAction(5, std::bind(&Widget::toggleTransparency, this));
        hotkeyMgr->loadHotkeys();
        qDebug() << "Hotkey actions registered:" << hotkeyMgr->registeredHotkeyNames();
    }
}

void Widget::settingsDialogAccepted()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void Widget::showOpacityContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction *removeTransparencyAction = contextMenu.addAction(tr("Remove transparency"));
    //removeTransparencyAction->setCheckable(true);
    //playerVolumeAction->setCheckable(true);
    if (m_bIsTransparent == false) removeTransparencyAction->setEnabled (false);
    contextMenu.addSeparator();
    // --- Create a submenu for opacity levels ---
    //QMenu *opacityMenu = contextMenu.addMenu(tr("Set Opacity"));
    // Create 10 actions: 10%, 20%, ..., 100%
    for (int i = 1; i < 10; ++i)
    {
        int level = 100 - i * 10;
        QAction *action = new QAction(/*"Set transparency to " +*/ QString::number(level) + "%", &contextMenu);
        action->setData(level);
        action->setCheckable(true);
        // Mark the current opacity as checked
        if (qRound(100 - m_dOpacity * 100) == level)
            action->setChecked(true);
        connect(action, SIGNAL(triggered()), this, SLOT(onOpacityActionTriggered()));
        contextMenu.addAction(action);
    }
    QAction *selectedAction = contextMenu.exec(transparentButton->mapToGlobal(pos));
    if (selectedAction == removeTransparencyAction)
    {
        this->setWindowOpacity (1.0);
        m_bIsTransparent = false;
        m_dOpacity = 1.0;
        QString sTransparentButtonTip = "Set transparency to ";
        sTransparentButtonTip.append (QString::number (qRound(100 - m_dLastOpacity * 100)) + "%");
        transparentButton->setToolTip (sTransparentButtonTip);
    }
}

void Widget::onOpacityActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;
    int level = 100 - action->data().toInt();
    qreal opacity = level / 100.0;
    this->setWindowOpacity(opacity);
    // Track transparency state
    m_bIsTransparent = (opacity < 1.0);
    m_dOpacity = opacity;
    m_dLastOpacity = opacity;
    transparentButton->setToolTip ("Remove transparency");
    qDebug() << "Window transparency set to" << 100 - level << "%";
    //saveSettings ();
}

void Widget::moveEvent(QMoveEvent *event)
{
    // qDebug() << __PRETTY_FUNCTION__;
    // Chiamiamo la funzione di base per garantire che l'evento venga gestito normalmente
    QWidget::moveEvent(event);
    // 2. Chiama la funzione di salvataggio
    saveSettings();
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
    // if (muted) ui->volumeLabel->setText("0%");
}

void Widget::onDeviceChanged(const QString &deviceId, const QString &friendlyName)
{
    qDebug() << __PRETTY_FUNCTION__ << "Device name: " << friendlyName;
    QString sToolTipMessage = "Default audio playback device changed to:<br><b>" + friendlyName + "</b>";
    QPoint globalPos = ui->volumeSlider->mapToGlobal(QPoint(ui->volumeSlider->width() / 2, ui->volumeSlider->height() / 2));
    QToolTip::showText(globalPos, sToolTipMessage, ui->volumeSlider);
    //QMessageBox::warning (this, "Audio device changed", "Default audio playback device changed.");
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

void Widget::handleVolumeUp()
{
    int iDiff = 10;
    int iCurVol = ui->volumeSlider->value ();
    iCurVol = qBound(0, iCurVol + iDiff, 100);
    float fVol = float (float(iCurVol) / 100.0f);
    m_systemVolumeController->setVolume (fVol);
    ui->volumeSlider->setValue (iCurVol);
    ui->volumeLabel->setText(QString::number(iCurVol) + "%");
}

void Widget::handleVolumeDown()
{
    int iDiff = -10;
    int iCurVol = ui->volumeSlider->value ();
    iCurVol = qBound(0, iCurVol + iDiff, 100);
    float fVol = float (float(iCurVol) / 100.0f);
    m_systemVolumeController->setVolume (fVol);
    ui->volumeSlider->setValue (iCurVol);
    ui->volumeLabel->setText(QString::number(iCurVol) + "%");
}

void Widget::on_muteButton_clicked()
{
    bool bMuted = m_systemVolumeController->isMuted ();
    if (bMuted)
    {
        // Unmute
        m_systemVolumeController->mute (false);
        int iVol = int (m_systemVolumeController->volume () * 100.01f);
        qDebug() << "Current master volume:" << iVol;
        ui->volumeSlider->setValue (iVol);
        ui->volumeLabel->setText(QString::number(iVol) + "%");
        ui->muteButton->setChecked (false);
    }
    else
    {
        // Mute
        m_systemVolumeController->mute (true);
        // ui->volumeLabel->setText("0%");
        ui->muteButton->setChecked (true);
    }
}

void Widget::toggleOnTop()
{
    //PostMessage(reinterpret_cast<HWND>(winId()), WM_HOTKEY, 1, 0); // simula hotkey ID 1
    Qt::WindowFlags flags = windowFlags();
    if (flags & Qt::WindowStaysOnTopHint)
    {
        flags &= ~Qt::WindowStaysOnTopHint;
        //onTopButton->setText("↑");
        onTopButton->setIcon (QIcon(":/img/img/Pin.png"));
    }
    else
    {
        flags |= Qt::WindowStaysOnTopHint;
        //onTopButton->setText("↓");
        onTopButton->setIcon (QIcon(":/img/img/UnPin.png"));
    }
    setWindowFlags(flags);
    show(); // Reapply flags
}

void Widget::toggleTransparency()
{
    if (m_bIsTransparent)
    {
        setWindowOpacity(1.0);
        //transparentButton->setText("■");
        m_bIsTransparent = false;
        m_dOpacity = 1.0;
        QString sTransparentButtonTip = "Set transparency to ";
        sTransparentButtonTip.append (QString::number (qRound(100 - m_dLastOpacity * 100)) + "%");
        transparentButton->setToolTip (sTransparentButtonTip);
    }
    else
    {
        setWindowOpacity(m_dLastOpacity); // Adjust as needed
        // transparentButton->setText("□");
        m_bIsTransparent = true;
        m_dOpacity = m_dLastOpacity;
        transparentButton->setToolTip ("Remove transparency");
        qDebug() << "Window transparency set to" << qRound((1.0 - m_dLastOpacity) * 100.0) << "%";
    }
}

void Widget::saveSettings()
{
    // QSettings richiede il nome dell'Organizzazione e dell'Applicazione
    // per creare un percorso di salvataggio univoco.
    QSettings settings;
    // Salva la posizione corrente del widget (coordinate x, y)
    settings.setValue("Position", pos());
    // Potresti anche salvare la dimensione (larghezza, altezza)
    settings.setValue("Size", size());
    Qt::WindowFlags flags = windowFlags();
    if (flags & Qt::WindowStaysOnTopHint) settings.setValue("OnTop", true);
    else settings.setValue("OnTop", false);
    // if (windowOpacity() == 1.0) settings.setValue("Transparent", false);
    // else settings.setValue("Transparent", true);
    if (m_bIsTransparent)
        settings.setValue("Transparent", qreal(m_dOpacity));
    else
        settings.setValue("Transparent", qreal(1.0));
    settings.setValue("LastTransparecyLevel", qreal(m_dLastOpacity));
    // In Qt, le impostazioni vengono salvate automaticamente
}

void Widget::loadSettings()
{
    QSettings settings;
    // 1. Carica la posizione
    QPoint savedPos = settings.value("Position", QPoint(100, 100)).toPoint();
    // 2. Carica la dimensione
    QSize savedSize = settings.value("Size", QSize(100, 200)).toSize();
    // 3. Applica le impostazioni
    //this->setFixedWidth (ui->muteButton->width () + 24);
    resize(savedSize);
    // Controlla se la posizione salvata è visibile su qualsiasi schermo
    // (Utile se l'utente ha staccato un monitor).
    QScreen *screen = QGuiApplication::screenAt(savedPos);
    if (screen)
    {
        move(savedPos);
    }
    else
    {
        // Se la posizione non è valida, centra la finestra
        // La posizione di default (100, 100) verrà usata se la chiave non esiste
        move(QPoint(100, 100));
    }
    bool onTop = settings.value("OnTop", false).toBool();
    qreal opacity = settings.value("Transparent", 1.0).toReal();
    m_dLastOpacity = settings.value("LastTransparecyLevel", 0.5).toReal();
    Qt::WindowFlags flags = windowFlags();
    if (onTop)
    {
        flags |= Qt::WindowStaysOnTopHint;
        //onTopButton->setText ("↑");
        m_bIsOnTop = true;
        onTopButton->setIcon (QIcon(":/img/img/Pin.png"));
    }
    else
    {
        flags &= ~Qt::WindowStaysOnTopHint;
        //onTopButton->setText ("↓");
        m_bIsOnTop = false;
        onTopButton->setIcon (QIcon(":/img/img/UnPin.png"));
    }
    setWindowFlags(flags);
    if (opacity < 1.0)
    {
        // transparentButton->setText("T");
        m_bIsTransparent = true;
        transparentButton->setToolTip ("Remove transparency");
    }
    else
    {
        // transparentButton->setText("t");
        m_bIsTransparent = false;
        QString sTransparentButtonTip = "Set transparency to ";
        sTransparentButtonTip.append (QString::number (qRound(100 - m_dLastOpacity * 100)) + "%");
        transparentButton->setToolTip (sTransparentButtonTip);
    }
    setWindowOpacity(opacity);
    m_dOpacity = opacity;
    updateButtons();
}

void Widget::updateButtons()
{
    if (m_bIsOnTop)
        // onTopButton->setText("↓");
        onTopButton->setIcon (QIcon(":/img/img/UnPin.png"));
    else
        // onTopButton->setText("↑");
        onTopButton->setIcon (QIcon(":/img/img/Pin.png"));
    // if (m_bIsTransparent) transparentButton->setText ("T");
    // else transparentButton->setText("t");
}

void Widget::updateCursorShape(const QPoint &pos)
{
    QRect rect = this->rect();
    if (pos.x() <= resizeMargin && pos.y() <= resizeMargin)
        setCursor(Qt::SizeFDiagCursor);
    else if (pos.x() >= rect.width() - resizeMargin && pos.y() <= resizeMargin)
        setCursor(Qt::SizeBDiagCursor);
    else if (pos.x() <= resizeMargin && pos.y() >= rect.height() - resizeMargin)
        setCursor(Qt::SizeBDiagCursor);
    else if (pos.x() >= rect.width() - resizeMargin && pos.y() >= rect.height() - resizeMargin)
        setCursor(Qt::SizeFDiagCursor);
    else if (pos.x() <= resizeMargin)
        setCursor(Qt::SizeHorCursor);
    else if (pos.x() >= rect.width() - resizeMargin)
        setCursor(Qt::SizeHorCursor);
    else if (pos.y() <= resizeMargin)
        setCursor(Qt::SizeVerCursor);
    else if (pos.y() >= rect.height() - resizeMargin)
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void Widget::onHotkeyAssigned(const QString &name, const QString &sequence)
{
    qDebug() << __PRETTY_FUNCTION__;
    hotkeyMgr->saveHotkey(name, sequence);
}
