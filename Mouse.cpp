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

#include "Mouse.h"

#ifdef Q_OS_LINUX
#elif defined Q_OS_WIN
#include <Windows.h>
#elif defined Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#else
#endif

namespace CMS {

void IMouse::move(Point p)
{
    move(p.X(), p.Y());
}

IMouse* MouseFactory::newMouse()
{
#ifdef Q_OS_LINUX
    return new LinuxMouse;
#elif defined Q_OS_WIN
    return new WindowsMouse;
#elif defined Q_OS_MAC
    return new MacMouse;
#else
    std::runtime_error("Operating System not supported. Cannot control mouse.");
    return 0;
#endif
}

#ifdef Q_OS_LINUX

void LinuxMouse::move(double x, double y)
{}

void LinuxMouse::click()
{}

#elif defined Q_OS_WIN

void WindowsMouse::move(double x, double y)
{
    SetCursorPos((int) x, (int) y);
}

void WindowsMouse::click()
{
    INPUT input={0};
    // left down
    input.type       = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    ::SendInput(1,&input,sizeof(INPUT));

    // left up
    ::ZeroMemory(&input,sizeof(INPUT));
    input.type      = INPUT_MOUSE;
    input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
    ::SendInput(1,&input,sizeof(INPUT));
}

#elif defined Q_OS_MAC

void MacMouse::move(double x, double y)
{
    CGEventRef move = CGEventCreateMouseEvent(NULL, NX_MOUSEMOVED, CGPointMake(x, y), kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, move);
}

void MacMouse::click()
{
    // Get cursor position
    CGEventRef event = CGEventCreate(NULL);
    CGPoint cursor = CGEventGetLocation(event);
    CFRelease(event);

    // Click
    CGEventRef mouseDown = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, cursor, kCGMouseButtonLeft);
    CGEventRef mouseUp = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, cursor, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, mouseDown);
    CGEventPost(kCGHIDEventTap, mouseUp);
}

#endif

} // namespace CMS

