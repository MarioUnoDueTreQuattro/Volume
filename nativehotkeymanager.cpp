#include "nativehotkeymanager.h"
#include <QAbstractEventDispatcher>
#include <QSettings>
#include <QDebug>
#include <QTimer>

static const QMap<QString, UINT> keyMap =
{
    // --- Letters ---
    {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'}, {"F", 'F'}, {"G", 'G'},
    {"H", 'H'}, {"I", 'I'}, {"J", 'J'}, {"K", 'K'}, {"L", 'L'}, {"M", 'M'}, {"N", 'N'},
    {"O", 'O'}, {"P", 'P'}, {"Q", 'Q'}, {"R", 'R'}, {"S", 'S'}, {"T", 'T'}, {"U", 'U'},
    {"V", 'V'}, {"W", 'W'}, {"X", 'X'}, {"Y", 'Y'}, {"Z", 'Z'},

    // --- Numbers (top row) ---
    {"0", '0'}, {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'},
    {"5", '5'}, {"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'},

    // --- Function keys ---
    {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5},
    {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8}, {"F9", VK_F9}, {"F10", VK_F10},
    {"F11", VK_F11}, {"F12", VK_F12},

    // --- Special keys ---
    {"ESC", VK_ESCAPE}, {"TAB", VK_TAB}, {"SPACE", VK_SPACE}, {"ENTER", VK_RETURN},
    {"BACKSPACE", VK_BACK}, {"DELETE", VK_DELETE}, {"INSERT", VK_INSERT},
    {"HOME", VK_HOME}, {"END", VK_END}, {"PAGEUP", VK_PRIOR}, {"PAGEDOWN", VK_NEXT},
    {"LEFT", VK_LEFT}, {"RIGHT", VK_RIGHT}, {"UP", VK_UP}, {"DOWN", VK_DOWN},

    // --- Aliases for navigation ---
    {"PGUP", VK_PRIOR},   // Page Up alias
    {"PGDN", VK_NEXT},    // Page Down alias

    // --- Common symbols ---
    {"-", VK_OEM_MINUS}, {"=", VK_OEM_PLUS},
    {"PLUS", VK_OEM_PLUS}, {"MINUS", VK_OEM_MINUS},
    {"+NUM", VK_ADD}, {"-NUM", VK_SUBTRACT},
    {"[", VK_OEM_4}, {"]", VK_OEM_6},
    {";", VK_OEM_1}, {"'", VK_OEM_7}, {",", VK_OEM_COMMA},
    {".", VK_OEM_PERIOD}, {"/", VK_OEM_2}, {"\\", VK_OEM_5},

    // --- Numeric keypad ---
    {"+NUM", VK_ADD}, {"NUMPAD+", VK_ADD}, {"NUMPADPLUS", VK_ADD},
    {"-NUM", VK_SUBTRACT}, {"NUMPAD-", VK_SUBTRACT},
    {"*NUM", VK_MULTIPLY}, {"NUMPAD*", VK_MULTIPLY},
    {"/NUM", VK_DIVIDE}, {"NUMPAD/", VK_DIVIDE},
    {".NUM", VK_DECIMAL}, {"NUMPAD.", VK_DECIMAL},

    {"0NUM", VK_NUMPAD0}, {"NUMPAD0", VK_NUMPAD0},
    {"1NUM", VK_NUMPAD1}, {"NUMPAD1", VK_NUMPAD1},
    {"2NUM", VK_NUMPAD2}, {"NUMPAD2", VK_NUMPAD2},
    {"3NUM", VK_NUMPAD3}, {"NUMPAD3", VK_NUMPAD3},
    {"4NUM", VK_NUMPAD4}, {"NUMPAD4", VK_NUMPAD4},
    {"5NUM", VK_NUMPAD5}, {"NUMPAD5", VK_NUMPAD5},
    {"6NUM", VK_NUMPAD6}, {"NUMPAD6", VK_NUMPAD6},
    {"7NUM", VK_NUMPAD7}, {"NUMPAD7", VK_NUMPAD7},
    {"8NUM", VK_NUMPAD8}, {"NUMPAD8", VK_NUMPAD8},
    {"9NUM", VK_NUMPAD9}, {"NUMPAD9", VK_NUMPAD9},

    // --- Modifier keys ---
    {"SHIFT", VK_SHIFT}, {"CTRL", VK_CONTROL}, {"ALT", VK_MENU},
    {"WIN", VK_LWIN}, {"LWIN", VK_LWIN}, {"RWIN", VK_RWIN},

    // --- Lock keys ---
    {"CAPSLOCK", VK_CAPITAL}, {"NUMLOCK", VK_NUMLOCK}, {"SCROLLLOCK", VK_SCROLL},

    // --- Misc keys ---
    {"PRINTSCREEN", VK_SNAPSHOT}, {"PAUSE", VK_PAUSE}, {"BREAK", VK_CANCEL},
    {"MENU", VK_APPS}
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

QString NativeHotkeyManager::normalizeKeySequence(const QString &sequence)
{
    QString normalized = sequence.trimmed().toUpper();

    // --- Remove spaces around plus signs ---
    normalized.replace(" +", "+");
    normalized.replace("+ ", "+");
    normalized.replace(" ", ""); // remove any remaining spaces

    // --- Replace common synonyms / variants ---
    normalized.replace("CONTROL", "CTRL");
    normalized.replace("ESCAPE", "ESC");
    normalized.replace("RETURN", "ENTER");
    normalized.replace("DEL", "DELETE");
    normalized.replace("PGUP", "PAGEUP");
    normalized.replace("PGDN", "PAGEDOWN");
    normalized.replace("WINKEY", "WIN");

    // --- Normalize numeric keypad naming ---
    normalized.replace("NUMPAD ", "NUMPAD"); // handle "NumPad 5" etc.
    normalized.replace("NUMPAD", "NUMPAD"); // ensure consistent case
    normalized.replace("NUMPAD+", "NUMPAD+");
    normalized.replace("NUMPAD-", "NUMPAD-");
    normalized.replace("NUMPAD*", "NUMPAD*");
    normalized.replace("NUMPAD/", "NUMPAD/");
    normalized.replace("NUMPAD.", "NUMPAD.");
    normalized.replace("NUMPAD0", "NUMPAD0");
    normalized.replace("NUMPAD1", "NUMPAD1");
    normalized.replace("NUMPAD2", "NUMPAD2");
    normalized.replace("NUMPAD3", "NUMPAD3");
    normalized.replace("NUMPAD4", "NUMPAD4");
    normalized.replace("NUMPAD5", "NUMPAD5");
    normalized.replace("NUMPAD6", "NUMPAD6");
    normalized.replace("NUMPAD7", "NUMPAD7");
    normalized.replace("NUMPAD8", "NUMPAD8");
    normalized.replace("NUMPAD9", "NUMPAD9");

    // --- Optional: make sure there are no double "++" errors ---
    while (normalized.contains("++"))
        normalized.replace("++", "+");

    return normalized;
}

// ---------------------------------------------------------

ParsedKey NativeHotkeyManager::parseKeySequence(const QString &sequence)
{
    ParsedKey result;
    result.mainKey = 0;
    result.modifiers = 0;

    // --- 1. Normalize input ---
    QString normalized = normalizeKeySequence(sequence);

    // --- 2. Split by '+' ---
    QStringList parts = normalized.split('+', Qt::SkipEmptyParts);

    // --- 3. Parse each part ---
    for (const QString &part : parts)
    {
        QString key = part.trimmed();

        if (key == "CTRL")
            result.modifiers |= MOD_CONTROL;
        else if (key == "SHIFT")
            result.modifiers |= MOD_SHIFT;
        else if (key == "ALT")
            result.modifiers |= MOD_ALT;
        else if (key == "WIN" || key == "LWIN" || key == "RWIN")
            result.modifiers |= MOD_WIN;
        else
        {
            // Lookup in keyMap for the main key
            if (keyMap.contains(key))
                result.mainKey = keyMap.value(key);
            else
                qWarning() << "Unknown key in sequence:" << key;
        }
    }

    return result;
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
        else keyPart = part;
    }

    // --- Normalize aliases for PLUS and MINUS ---
    if (keyPart == "+" || keyPart == "PLUS")
        keyPart = "PLUS";
    else if (keyPart == "-" || keyPart == "MINUS")
        keyPart = "MINUS";
    else if (keyPart == "NUMPAD+" || keyPart == "NUMPADPLUS" || keyPart == "ADD" || keyPart == "+NUM")
        keyPart = "+NUM";
    else if (keyPart == "NUMPAD-" || keyPart == "NUMPADMINUS" || keyPart == "SUBTRACT" || keyPart == "-NUM")
        keyPart = "-NUM";

    // --- Lookup key ---
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
        qDebug() << "Unrecognized key:" << keyPart << "from sequence:" << sequence;
        return false;
    }

    qDebug() << "Normalized:" << normalized
             << "→ keyPart:" << keyPart
             << "→ vk:" << vk
             << "→ modifiers:" << modifiers;

    return true;
}

QString NativeHotkeyManager::keySequenceToString(const ParsedKey &parsedKey)
{
    QStringList parts;

    // --- 1. Add modifier names ---
    if (parsedKey.modifiers & MOD_CONTROL)
        parts << "Ctrl";
    if (parsedKey.modifiers & MOD_SHIFT)
        parts << "Shift";
    if (parsedKey.modifiers & MOD_ALT)
        parts << "Alt";
    if (parsedKey.modifiers & MOD_WIN)
        parts << "Win";

    // --- 2. Find the key name (reverse lookup in keyMap) ---
    QString mainKeyName;

    for (auto it = keyMap.constBegin(); it != keyMap.constEnd(); ++it)
    {
        if (it.value() == parsedKey.mainKey)
        {
            mainKeyName = it.key();
            break;
        }
    }

    // --- 3. Handle unknown key codes ---
    if (mainKeyName.isEmpty())
    {
        if (parsedKey.mainKey >= 'A' && parsedKey.mainKey <= 'Z')
            mainKeyName = QChar(parsedKey.mainKey);
        else if (parsedKey.mainKey >= '0' && parsedKey.mainKey <= '9')
            mainKeyName = QChar(parsedKey.mainKey);
        else
            mainKeyName = QString("VK_%1").arg(parsedKey.mainKey);
    }

    // --- 4. Add main key ---
    parts << mainKeyName;

    // --- 5. Join with '+' ---
    return parts.join("+");
}

//QString NativeHotkeyManager::normalizeKeySequence(const QString &sequence)
//{
//    QString s = sequence.trimmed();
//    QStringList parts;
//    QString current;

//    // --- Manual parser to preserve '+' and '-' ---
//    for (int i = 0; i < s.length(); ++i)
//    {
//        QChar c = s.at(i);

//        if (c == '+')
//        {
//            if (!current.isEmpty())
//            {
//                parts << current.trimmed();
//                current.clear();
//            }

//            // double '++' → '+' key
//            if (i + 1 < s.length() && s.at(i + 1) == '+')
//            {
//                parts << "+";
//                ++i;
//            }
//            else if (i == s.length() - 1)
//            {
//                parts << "+";
//            }
//        }
//        else
//        {
//            current.append(c);
//        }
//    }

//    if (!current.isEmpty())
//        parts << current.trimmed();

//    QStringList normalized;
//    QString keyPart;

//    for (const QString &rawPart : parts)
//    {
//        QString part = rawPart.trimmed();

//        // --- Modifiers ---
//        if (part.compare("Ctrl", Qt::CaseInsensitive) == 0)
//            normalized << "CTRL";
//        else if (part.compare("Alt", Qt::CaseInsensitive) == 0)
//            normalized << "ALT";
//        else if (part.compare("Shift", Qt::CaseInsensitive) == 0)
//            normalized << "SHIFT";
//        else if (part.compare("Win", Qt::CaseInsensitive) == 0 ||
//                 part.compare("Meta", Qt::CaseInsensitive) == 0)
//            normalized << "META";

//        // --- PLUS / MINUS ---
//        else if (part.compare("Plus", Qt::CaseInsensitive) == 0)
//            keyPart = "PLUS";
//        else if (part.compare("Minus", Qt::CaseInsensitive) == 0)
//            keyPart = "MINUS";
//        else if (part == "+")
//            keyPart = "PLUS";
//        else if (part == "-")
//            keyPart = "MINUS";

//        // --- PAGEUP / PAGEDOWN (aliases PgUp/PgDn) ---
//        else if (part.compare("PgUp", Qt::CaseInsensitive) == 0)
//            keyPart = "PAGEUP";
//        else if (part.compare("PgDn", Qt::CaseInsensitive) == 0)
//            keyPart = "PAGEDOWN";
//        else if (part.compare("PageUp", Qt::CaseInsensitive) == 0)
//            keyPart = "PAGEUP";
//        else if (part.compare("PageDown", Qt::CaseInsensitive) == 0)
//            keyPart = "PAGEDOWN";

//        // --- ARROW KEYS ---
//        else if (part.compare("Up", Qt::CaseInsensitive) == 0)
//            keyPart = "UP";
//        else if (part.compare("Down", Qt::CaseInsensitive) == 0)
//            keyPart = "DOWN";
//        else if (part.compare("Left", Qt::CaseInsensitive) == 0)
//            keyPart = "LEFT";
//        else if (part.compare("Right", Qt::CaseInsensitive) == 0)
//            keyPart = "RIGHT";

//        // --- Default case ---
//        else
//            keyPart = part.toUpper();
//    }

//    if (!keyPart.isEmpty())
//        normalized << keyPart;

//    return normalized.join("+");
//}

//QString NativeHotkeyManager::normalizeKeySequence(const QString &sequence)
//{
//    QStringList parts = sequence.split('+', Qt::SkipEmptyParts);
//    QStringList modifiers;
//    QString keyPart;
//    for (const QString &rawPart : parts)
//    {
//        QString part = rawPart.trimmed();
//        if (part.compare("Ctrl", Qt::CaseInsensitive) == 0) modifiers << "CTRL";
//        else if (part.compare("Alt", Qt::CaseInsensitive) == 0) modifiers << "ALT";
//        else if (part.compare("Shift", Qt::CaseInsensitive) == 0) modifiers << "SHIFT";
//        else if (part.compare("Win", Qt::CaseInsensitive) == 0 ||
//            part.compare("Meta", Qt::CaseInsensitive) == 0) modifiers << "META";
//        else keyPart += part.toUpper(); // concatena tutto ciò che non è modificatore
//    }
//    if (!keyPart.isEmpty())
//        modifiers << keyPart;
//    return modifiers.join("+");
//}

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
