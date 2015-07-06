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

#include <stdexcept>
#include <QDebug>

#include "Keyboard.h"

namespace CMS {

KeyEvent::KeyEvent(Key key, KeyState state) :
    key(key), state(state)
{}

Key KeyEvent::getKey()
{
    return key;
}

KeyState KeyEvent::getState()
{
    return state;
}

IKeyboard::~IKeyboard()
{}

IKeyboard* KeyboardFactory::newKeyboard()
{
#ifdef Q_OS_LINUX
    return new LinuxKeyboard;
#elif defined Q_OS_WIN
    return new WindowsKeyboard;
#elif defined Q_OS_MAC
    return new MacKeyboard;
#else
    std::runtime_error("Operating System not supported. Cannot control mouse.");
    return 0;
#endif
}

#ifdef Q_OS_LINUX

KeyEvent LinuxKeyboard::nextEvent()
{}

bool LinuxKeyboard::hasNextEvent()
{
    return false;
}

#elif defined Q_OS_WIN

KeyEvent WindowsKeyboard::nextEvent()
{}

bool WindowsKeyboard::hasNextEvent()
{
    return false;
}

#elif defined Q_OS_MAC



#else
#endif

} // namespace CMS

