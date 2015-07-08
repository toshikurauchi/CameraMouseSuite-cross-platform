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

#include "Monitor.h"

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#elif defined Q_OS_WIN
#include <Windows.h>
#elif defined Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#else
#endif

namespace CMS {

IMonitor* MonitorFactory::newMonitor()
{
#ifdef Q_OS_LINUX
    return new LinuxMonitor;
#elif defined Q_OS_WIN
    return new WindowsMonitor;
#elif defined Q_OS_MAC
    return new MacMonitor;
#else
    std::runtime_error("Operating System not supported. Cannot get monitor information.");
    return 0;
#endif
}

#ifdef Q_OS_LINUX

Point LinuxMonitor::getResolution()
{
    Display* disp = XOpenDisplay(NULL);
    Screen*  scrn = DefaultScreenOfDisplay(disp);
    int width  = scrn->width;
    int height = scrn->height;
    return Point(width, height);
}

#elif defined Q_OS_WIN

Point WindowsMonitor::getResolution()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    return Point(width, height);
}

#elif defined Q_OS_MAC

Point MacMonitor::getResolution()
{
    CGDirectDisplayID display = CGMainDisplayID();
    size_t width = CGDisplayPixelsWide(display);
    size_t height = CGDisplayPixelsHigh(display);
    return Point(width, height);
}

#endif

} // namespace CMS

