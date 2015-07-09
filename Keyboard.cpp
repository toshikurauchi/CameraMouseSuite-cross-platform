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

#ifdef Q_OS_LINUX
#include <QThread>
#include <QMutex>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <ctype.h>
#include <queue>
#elif defined Q_OS_WIN
#include <queue>
#include <Windows.h>
#include <QMutex>
#elif defined Q_OS_MAC
#else
#endif

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

// Use an unnamed namespace to restrict global variables scope
namespace {
int instances = 0;
std::queue<KeyEvent> events;
QMutex mutex;
} // namespace

class KeyboardListener : public QThread
{
public:
    KeyboardListener() : running(true)
    {}

    void run()
    {
        // Code from: http://stackoverflow.com/questions/22749444/listening-to-keyboard-events-without-consuming-them-in-x11-keyboard-hooking
        Display* d = XOpenDisplay(NULL);
        Window root = DefaultRootWindow(d);
        Window curFocus;
        char buf[17];
        KeySym ks;
        XComposeStatus comp;
        int revert;

        XGetInputFocus(d, &curFocus, &revert);
        XSelectInput(d, curFocus, KeyPressMask|KeyReleaseMask|FocusChangeMask);

        bool stop;
        runningMutex.lock();
        stop = !running;
        runningMutex.unlock();
        while (!stop)
        {
            XEvent ev;
            XNextEvent(d, &ev); // Will block here, find out a way to move on if the program finishes
            switch (ev.type)
            {
                case FocusOut:
                    if (curFocus != root)
                        XSelectInput(d, curFocus, 0);
                    XGetInputFocus (d, &curFocus, &revert);
                    if (curFocus == PointerRoot)
                        curFocus = root;
                    XSelectInput(d, curFocus, KeyPressMask|KeyReleaseMask|FocusChangeMask);
                    break;

                case KeyPress:
                    XLookupString(&ev.xkey, buf, 16, &ks, &comp);
                    switch (ks)
                    {
                    case XK_Control_L:
                    case XK_Control_R:
                        mutex.lock();
                        events.push(KeyEvent(KEY_CONTROL, KEY_STATE_DOWN));
                        mutex.unlock();
                        break;
                    }
            }
            runningMutex.lock();
            stop = !running;
            runningMutex.unlock();
        }
    }

    void stop()
    {
        runningMutex.lock();
        running = false;
        runningMutex.unlock();
    }

private:
    bool running;
    QMutex runningMutex;
};

// Use an unnamed namespace to restrict global variables scope
namespace {
KeyboardListener listener;
} // namespace

LinuxKeyboard::LinuxKeyboard()
{
    instances++;
    if (instances == 1)
    {
        listener.start();
    }
}

LinuxKeyboard::~LinuxKeyboard()
{
    instances--;
    if (instances == 0)
    {
        listener.stop();
    }
}

KeyEvent LinuxKeyboard::nextEvent()
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

bool LinuxKeyboard::hasNextEvent()
{
    return !events.empty();
}

#elif defined Q_OS_WIN

// Use an unnamed namespace to restrict global variables scope
namespace {
int instances = 0;
HHOOK keyboardHook;
std::queue<KeyEvent> events;
QMutex mutex;
} // namespace

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    Key key = KEY_NONE;
    KeyState state = KEY_STATE_NONE;
    if (nCode == HC_ACTION)
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        switch (p->vkCode)
        {
        case VK_RCONTROL:
        case VK_LCONTROL:
        case VK_CONTROL:
            key = KEY_CONTROL;
            break;
        case VK_RMENU:
        case VK_LMENU:
        case VK_MENU:
            key = KEY_ALT;
            break;
        }

        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            state = KEY_STATE_DOWN;
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            state = KEY_STATE_UP;
            break;
        }
    }

    if (key != KEY_NONE && state != KEY_STATE_NONE)
    {
        mutex.lock();
        events.push(KeyEvent(key, state));
        mutex.unlock();
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

WindowsKeyboard::WindowsKeyboard()
{
    instances++;
    if (instances == 1)
    {
        keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, 0, 0);
    }
}

WindowsKeyboard::~WindowsKeyboard()
{
    instances--;
    if (instances == 0)
    {
        UnhookWindowsHookEx(keyboardHook);
    }
}

KeyEvent WindowsKeyboard::nextEvent()
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

bool WindowsKeyboard::hasNextEvent()
{
    return !events.empty();
}

#elif defined Q_OS_MAC
#else
#endif

} // namespace CMS

