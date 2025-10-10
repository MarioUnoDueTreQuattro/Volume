#ifndef NATIVEHOTKEYMANAGER_H
#define NATIVEHOTKEYMANAGER_H

#include <QObject>
#include <QWidget>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <QMap>
#include <QSet>

struct ParsedKey
{
    UINT mainKey;     // The main key (e.g. 'R', VK_F5, VK_NUMPAD1, etc.)
    UINT modifiers;   // Combination of MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN
};

class NativeHotkeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    static NativeHotkeyManager *instance(QWidget *targetWindow = nullptr);
    //~NativeHotkeyManager();
    bool registerHotkey(int id, UINT modifiers, UINT vk);
    void unregisterHotkey(int id);
    void saveHotkeyConfig(const QString &name, const QString &keySequence);
    QString loadHotkeyConfig(const QString &name);
    bool parseKeySequence(const QString &sequence, UINT &modifiers, UINT &vk);
    bool loadHotkeys();
    bool saveHotkey(const QString &name, const QString &sequence);
    QStringList registeredHotkeyNames() const;
    void bindAction(int id, std::function<void()> action);
    bool isHotkeyBlocked(UINT modifiers, UINT vk);
    void unregisterAllHotkeys();
    void setOrganizationName(const QString &sOrganizationName);
    void setApplicationName(const QString &sApplicationName);
    // QString normalizeKeySequence(const QString &sequence);
    //ParsedKey parseKeySequence(const QString &sequence);
    QString keySequenceToString(const ParsedKey &parsedKey);
signals:
    void hotkeyPressed(int id);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

private:
    explicit NativeHotkeyManager(QWidget *targetWindow);
    static NativeHotkeyManager *singleton;
    HWND hwnd;
    QSet<int> registeredHotkeyIds;
    QMap<int, std::function<void()>> hotkeyActions;
    QString normalizeKeySequence(const QString &sequence);
    QString m_sOrganizationName;
    QString m_sApplicationName;
};

#endif // NATIVEHOTKEYMANAGER_H
