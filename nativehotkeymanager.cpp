#include "nativehotkeymanager.h"
#include <QAbstractEventDispatcher>
#include <QSettings>
#include <QDebug>
#include <QTimer>

static const QMap<QString, UINT> keyMap =
{
    // Lettere
    {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'}, {"F", 'F'}, {"G", 'G'},
    {"H", 'H'}, {"I", 'I'}, {"J", 'J'}, {"K", 'K'}, {"L", 'L'}, {"M", 'M'}, {"N", 'N'},
    {"O", 'O'}, {"P", 'P'}, {"Q", 'Q'}, {"R", 'R'}, {"S", 'S'}, {"T", 'T'}, {"U", 'U'},
    {"V", 'V'}, {"W", 'W'}, {"X", 'X'}, {"Y", 'Y'}, {"Z", 'Z'},

    // Numeri (riga superiore)
    {"0", '0'}, {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'},
    {"5", '5'}, {"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'},

    // Tasti funzione
    {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5},
    {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8}, {"F9", VK_F9}, {"F10", VK_F10},
    {"F11", VK_F11}, {"F12", VK_F12},

    // Tasti speciali
    {"ESC", VK_ESCAPE}, {"TAB", VK_TAB}, {"SPACE", VK_SPACE}, {"ENTER", VK_RETURN},
    {"BACKSPACE", VK_BACK}, {"DELETE", VK_DELETE}, {"INSERT", VK_INSERT},
    {"HOME", VK_HOME}, {"END", VK_END}, {"PAGEUP", VK_PRIOR}, {"PAGEDOWN", VK_NEXT},
    {"LEFT", VK_LEFT}, {"RIGHT", VK_RIGHT}, {"UP", VK_UP}, {"DOWN", VK_DOWN},

    // Simboli comuni
    {"-", VK_OEM_MINUS}, {"=", VK_OEM_PLUS}, {"[", VK_OEM_4}, {"]", VK_OEM_6},
    {";", VK_OEM_1}, {"'", VK_OEM_7}, {",", VK_OEM_COMMA}, {".", VK_OEM_PERIOD},
    {"/", VK_OEM_2}, {"\\", VK_OEM_5},

    // Tastierino numerico (con alias)
    {"+NUM", VK_ADD}, {"NumPad+", VK_ADD}, {"NUMPAD+", VK_ADD},
    {"NUMPADPLUS", VK_ADD},
    {"-NUM", VK_SUBTRACT}, {"NumPad-", VK_SUBTRACT}, {"NUMPAD-", VK_SUBTRACT},
    {"*NUM", VK_MULTIPLY}, {"NumPad*", VK_MULTIPLY}, {"NUMPAD*", VK_MULTIPLY},
    {"/NUM", VK_DIVIDE}, {"NumPad/", VK_DIVIDE}, {"NUMPAD/", VK_DIVIDE},
    {".NUM", VK_DECIMAL}, {"NumPad.", VK_DECIMAL}, {"NUMPAD.", VK_DECIMAL},

    {"0NUM", VK_NUMPAD0}, {"NumPad0", VK_NUMPAD0}, {"NUMPAD0", VK_NUMPAD0},
    {"1NUM", VK_NUMPAD1}, {"NumPad1", VK_NUMPAD1}, {"NUMPAD1", VK_NUMPAD1},
    {"2NUM", VK_NUMPAD2}, {"NumPad2", VK_NUMPAD2}, {"NUMPAD2", VK_NUMPAD2},
    {"3NUM", VK_NUMPAD3}, {"NumPad3", VK_NUMPAD3}, {"NUMPAD3", VK_NUMPAD3},
    {"4NUM", VK_NUMPAD4}, {"NumPad4", VK_NUMPAD4}, {"NUMPAD4", VK_NUMPAD4},
    {"5NUM", VK_NUMPAD5}, {"NumPad5", VK_NUMPAD5}, {"NUMPAD5", VK_NUMPAD5},
    {"6NUM", VK_NUMPAD6}, {"NumPad6", VK_NUMPAD6}, {"NUMPAD6", VK_NUMPAD6},
    {"7NUM", VK_NUMPAD7}, {"NumPad7", VK_NUMPAD7}, {"NUMPAD7", VK_NUMPAD7},
    {"8NUM", VK_NUMPAD8}, {"NumPad8", VK_NUMPAD8}, {"NUMPAD8", VK_NUMPAD8},
    {"9NUM", VK_NUMPAD9}, {"NumPad9", VK_NUMPAD9}, {"NUMPAD9", VK_NUMPAD9}
};

NativeHotkeyManager *NativeHotkeyManager::singleton = nullptr;

NativeHotkeyManager::NativeHotkeyManager(QWidget *targetWindow)
    : QObject(nullptr)
{
    targetWindow->winId(); // forza HWND
    hwnd = reinterpret_cast<HWND>(targetWindow->winId());
    //qDebug() << "HWND:" << hwnd;
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
    //qDebug() << "Native event filter installed";
}

NativeHotkeyManager *NativeHotkeyManager::instance(QWidget *targetWindow)
{
    if (!singleton && targetWindow)
        singleton = new NativeHotkeyManager(targetWindow);
    return singleton;
}

//NativeHotkeyManager::~NativeHotkeyManager()
//{
// QAbstractEventDispatcher::instance()->removeNativeEventFilter(this);
//}

bool NativeHotkeyManager::registerHotkey(int id, UINT modifiers, UINT vk)
{
    qDebug() << __PRETTY_FUNCTION__ << "hwnd:" << hwnd << "id:" << id << "mod:" << modifiers << "vk:" << vk;
    BOOL success = RegisterHotKey(hwnd, id, modifiers, vk);
    if (!success)
        qDebug() << "RegisterHotKey failed for ID:" << id << "Error:" << GetLastError();
    else
        registeredHotkeyIds.insert(id); // salva l’ID se registrato
    return success;
}

bool NativeHotkeyManager::isHotkeyBlocked(UINT modifiers, UINT vk)
{
    static int testId = 9999; // ID temporaneo per test
    HWND hwnd = this->hwnd;   // Assicurati che sia valido
    BOOL success = RegisterHotKey(hwnd, testId, modifiers, vk);
    if (success)
    {
        UnregisterHotKey(hwnd, testId); // Rilascia subito
        return false; // Non bloccata
    }
    DWORD err = GetLastError();
    qDebug() << "Hotkey test failed. modifiers:" << modifiers << "vk:" << vk << "Error:" << err;
    return true; // Bloccata o già registrata
}

void NativeHotkeyManager::unregisterHotkey(int id)
{
    qDebug() << __PRETTY_FUNCTION__;
    UnregisterHotKey(hwnd, id);
}

bool NativeHotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY)
        {
            // qDebug() << __PRETTY_FUNCTION__;
            int id = static_cast<int>(msg->wParam);
            qDebug() << "Received WM_HOTKEY with ID:" << id;
            if (hotkeyActions.contains(id))
            {
                // qDebug() << "Executing action for hotkey ID:" << id;
                // Copia sicura dell'azione
                std::function<void()> safeAction = hotkeyActions[id];
                // Esecuzione asincrona nel contesto Qt
                QTimer::singleShot(0, nullptr, [safeAction]()
                {
                    try
                    {
                        safeAction();
                    }
                    catch (...)
                    {
                        qDebug() << "Exception in hotkey action";
                    }
                });
            }
            else
            {
                qDebug() << "No action bound for hotkey ID:" << id;
            }
            emit hotkeyPressed(msg->wParam);
            if (result)
                *result = 0;
            // qDebug() << "Result pointer:" << result;
            return true;
        }
    }
    return false;
}

void NativeHotkeyManager::saveHotkeyConfig(const QString &name, const QString &keySequence)
{
    qDebug() << __PRETTY_FUNCTION__;
    QSettings settings(m_sOrganizationName, m_sApplicationName);
    settings.setValue("Hotkeys/" + name, keySequence);
}

QString NativeHotkeyManager::loadHotkeyConfig(const QString &name)
{
    qDebug() << __PRETTY_FUNCTION__;
    QSettings settings(m_sOrganizationName, m_sApplicationName);
    return settings.value("Hotkeys/" + name).toString();
}

bool NativeHotkeyManager::saveHotkey(const QString &name, const QString &sequence)
{
    qDebug() << __PRETTY_FUNCTION__;
    QSettings settings(m_sOrganizationName, m_sApplicationName);
    settings.setValue("Hotkeys/" + name, sequence);
    UINT mod = 0, vk = 0;
    if (parseKeySequence(sequence, mod, vk))
    {
        int id = name.toInt(); // chiave numerica coerente
        return registerHotkey(id, mod, vk);
    }
    return false;
}

bool NativeHotkeyManager::loadHotkeys()
{
    qDebug() << __PRETTY_FUNCTION__;
    QSettings settings(m_sOrganizationName, m_sApplicationName);
    settings.beginGroup("Hotkeys");
    QStringList keys = settings.childKeys();
    for (const QString &name : keys)
    {
        QString sequence = settings.value(name).toString();
        saveHotkey(name, sequence); // riusa la logica
    }
    settings.endGroup();
    return true;
}

QStringList NativeHotkeyManager::registeredHotkeyNames() const
{
    qDebug() << __PRETTY_FUNCTION__;
    QSettings settings(m_sOrganizationName, m_sApplicationName);
    settings.beginGroup("Hotkeys");
    return settings.childKeys();
}

bool NativeHotkeyManager::parseKeySequence(const QString &sequence, UINT &modifiers, UINT &vk)
{
    modifiers = 0;
    vk = 0;
    QString normalized = normalizeKeySequence(sequence);
    QStringList parts = normalized.split('+', Qt::SkipEmptyParts);
    QString keyPart;
    for (const QString &part : parts)
    {
        if (part == "CTRL") modifiers |= MOD_CONTROL;
        else if (part == "ALT") modifiers |= MOD_ALT;
        else if (part == "SHIFT") modifiers |= MOD_SHIFT;
        else if (part == "WIN" || part == "META") modifiers |= MOD_WIN;
        else keyPart = part; // ultimo elemento è il tasto finale
    }
    if (keyMap.contains(keyPart))
    {
        vk = keyMap[keyPart];
    }
    else if (keyPart.length() == 1)
    {
        vk = VkKeyScan(keyPart.at(0).toLatin1()) & 0xFF;
    }
    if (vk == 0)
    {
        qDebug() << "Unrecognized key part:" << keyPart;
        return false;
    }
    qDebug() << "Normalized sequence:" << normalized;
    qDebug() << "Final keyPart:" << keyPart << " → vk:" << vk;
    return true;
}

QString NativeHotkeyManager::normalizeKeySequence(const QString &sequence)
{
    QStringList parts = sequence.split('+', Qt::SkipEmptyParts);
    QStringList modifiers;
    QString keyPart;
    for (const QString &rawPart : parts)
    {
        QString part = rawPart.trimmed();
        if (part.compare("Ctrl", Qt::CaseInsensitive) == 0) modifiers << "CTRL";
        else if (part.compare("Alt", Qt::CaseInsensitive) == 0) modifiers << "ALT";
        else if (part.compare("Shift", Qt::CaseInsensitive) == 0) modifiers << "SHIFT";
        else if (part.compare("Win", Qt::CaseInsensitive) == 0 ||
            part.compare("Meta", Qt::CaseInsensitive) == 0) modifiers << "META";
        else keyPart += part.toUpper(); // concatena tutto ciò che non è modificatore
    }
    if (!keyPart.isEmpty())
        modifiers << keyPart;
    return modifiers.join("+");
}

void NativeHotkeyManager::setApplicationName(const QString &sApplicationName)
{
    m_sApplicationName = sApplicationName;
}

void NativeHotkeyManager::setOrganizationName(const QString &sOrganizationName)
{
    m_sOrganizationName = sOrganizationName;
}

void NativeHotkeyManager::unregisterAllHotkeys()
{
    for (int id : registeredHotkeyIds)
    {
        UnregisterHotKey(hwnd, id);
        qDebug() << "Unregistered hotkey ID:" << id;
    }
    registeredHotkeyIds.clear();
}

void NativeHotkeyManager::bindAction(int id, std::function<void()> action)
{
    qDebug() << __PRETTY_FUNCTION__ << "binding id:" << id;
    hotkeyActions[id] = action;
}
