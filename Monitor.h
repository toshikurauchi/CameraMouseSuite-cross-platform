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

#ifndef CMS_MONITOR_H
#define CMS_MONITOR_H

#include <QObject> // Included to have the OS defines

#include "Point.h"

namespace CMS {

class IMonitor
{
public:
    virtual Point getResolution() = 0;
};

class MonitorFactory
{
public:
    static IMonitor* newMonitor();
};

#ifdef Q_OS_LINUX

class LinuxMonitor : public IMonitor
{
public:
    Point getResolution();
};

#elif defined Q_OS_WIN

class WindowsMonitor : public IMonitor
{
public:
    Point getResolution();
};

#elif defined Q_OS_MAC

class MacMonitor : public IMonitor
{
public:
    Point getResolution();
};

#endif

} // namespace CMS

#endif // CMS_MONITOR_H
