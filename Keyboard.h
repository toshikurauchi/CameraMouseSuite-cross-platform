/*                         Camera Mouse Suite
 *  Copyright (C) 2015, Andrew Kurauchi
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CMS_KEYBOARD_H
#define CMS_KEYBOARD_H

#include <QObject> // Included to have the OS defines

#ifdef Q_OS_LINUX
#elif defined Q_OS_WIN
#elif defined Q_OS_MAC
#include <QMutex>
#include <queue>
#else
#endif

namespace CMS {

enum Key
{
    KEY_CONTROL,
    KEY_COMMAND,
    KEY_ALT,
    KEY_NONE
};

enum KeyState
{
    KEY_STATE_DOWN,
    KEY_STATE_UP,
    KEY_STATE_NONE
};

class KeyEvent
{
public:
    KeyEvent(Key key, KeyState state);
    Key getKey();
    KeyState getState();
private:
    Key key;
    KeyState state;
};

class IKeyboard
{
public:
    virtual ~IKeyboard();
    virtual KeyEvent nextEvent() = 0;
    virtual bool hasNextEvent() = 0;
};

class KeyboardFactory
{
public:
    static IKeyboard *newKeyboard();
};

#ifdef Q_OS_LINUX

class LinuxKeyboard : public IKeyboard
{
public:
    LinuxKeyboard();
    ~LinuxKeyboard();
    KeyEvent nextEvent();
    bool hasNextEvent();
};

#elif defined Q_OS_WIN

class WindowsKeyboard : public IKeyboard
{
public:
    WindowsKeyboard();
    ~WindowsKeyboard();
    KeyEvent nextEvent();
    bool hasNextEvent();
};

#elif defined Q_OS_MAC

class MacKeyboard : public IKeyboard
{
public:
    MacKeyboard();
    ~MacKeyboard();
    KeyEvent nextEvent();
    bool hasNextEvent();
private:
    std::queue<KeyEvent> events;
    QMutex mutex;
};

#endif

} // namespace CMS

#endif // CMS_KEYBOARD_H
