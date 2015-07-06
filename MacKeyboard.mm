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

#include <QDebug>

#include "Keyboard.h"

#ifdef Q_OS_MAC

#import <AppKit/AppKit.h>

namespace CMS {

MacKeyboard::MacKeyboard()
{
    // register for keys outside of the application
    [NSEvent addGlobalMonitorForEventsMatchingMask:NSFlagsChangedMask handler:^(NSEvent *theEvent)
    {
        if ([theEvent modifierFlags] & NSCommandKeyMask)
        {
            events.push(KeyEvent(Key::KEY_COMMAND, KeyState::KEY_STATE_DOWN));
        }
        else if ([theEvent modifierFlags] & NSControlKeyMask)
        {
            events.push(KeyEvent(Key::KEY_CONTROL, KeyState::KEY_STATE_DOWN));
        }
    }];
    // register for keys inside the application
    [NSEvent addLocalMonitorForEventsMatchingMask:NSFlagsChangedMask handler:^(NSEvent *theEvent)
    {
        if ([theEvent modifierFlags] & NSCommandKeyMask)
        {
            events.push(KeyEvent(Key::KEY_COMMAND, KeyState::KEY_STATE_DOWN));
        }
        else if ([theEvent modifierFlags] & NSControlKeyMask)
        {
            events.push(KeyEvent(Key::KEY_CONTROL, KeyState::KEY_STATE_DOWN));
        }
        return theEvent;
    }];
}

MacKeyboard::~MacKeyboard()
{
}

KeyEvent MacKeyboard::nextEvent()
{
    if (!hasNextEvent())
    {
        throw std::logic_error("No events available");
    }

    mutex.lock();
    KeyEvent event = events.front();
    events.pop();
    mutex.unlock();
    return event;
}

bool MacKeyboard::hasNextEvent()
{
    return !events.empty();
}

} // namespace CMS

#endif
