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

#include "Mouse.h"

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <unistd.h>
#elif defined Q_OS_WIN
#include <Windows.h>
#elif defined Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#else
#endif

namespace CMS {

IMouse::~IMouse()
{}

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
{
    Display *display;
    Window root_window;

    display = XOpenDisplay(NULL);

    if(display == NULL)
    {
        throw std::runtime_error("Couldn't open the display");
    }

    root_window = XRootWindow(display, 0);
    XSelectInput(display, root_window, KeyReleaseMask);

    XWarpPointer(display, None, root_window, 0, 0, 0, 0, x, y);

    XFlush(display); // Flushes the output buffer, therefore updates the cursor's position.
    XCloseDisplay(display);
}

void LinuxMouse::click()
{
    Display *display = XOpenDisplay(NULL);
    XEvent event;

    if(display == NULL)
    {
        throw std::runtime_error("Couldn't open the display");
    }

    memset(&event, 0x00, sizeof(event));

    event.type = ButtonPress;
    event.xbutton.button = Button1;
    event.xbutton.same_screen = True;

    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;

        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
    {
        throw std::runtime_error("Button could not be pressed");
    }
    XFlush(display);

    usleep(100000); // Time in microseconds

    event.type = ButtonRelease;
    event.xbutton.state = 0x100;
    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
    {
        throw std::runtime_error("Button could not be released");
    }
    XFlush(display);
    XCloseDisplay(display);
}

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
    // Mouse Down
    CGEventRef mouseDown = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, cursor, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, mouseDown);
    // Mouse Up
    CGEventRef mouseUp = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, cursor, kCGMouseButtonLeft);
    CGEventSetIntegerValueField(mouseUp, kCGMouseEventClickState, 1);
    CGEventPost(kCGHIDEventTap, mouseUp);
}

#endif

} // namespace CMS

