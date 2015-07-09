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

#ifndef CMS_MOUSE_H
#define CMS_MOUSE_H

#include <QObject> // Included to have the OS defines

#include "Point.h"

namespace CMS {

class IMouse
{
public:
    virtual ~IMouse();
    void move(Point p);
    virtual void move(double x, double y) = 0;
    virtual void click() = 0;
};

class MouseFactory
{
public:
    static IMouse* newMouse();
};

#ifdef Q_OS_LINUX

class LinuxMouse : public IMouse
{
public:
    void move(double x, double y);
    void click();
};

#elif defined Q_OS_WIN

class WindowsMouse : public IMouse
{
public:
    void move(double x, double y);
    void click();
};

#elif defined Q_OS_MAC

class MacMouse : public IMouse
{
public:
    void move(double x, double y);
    void click();
};

#endif

} // namespace CMS

#endif // CMS_MOUSE_H
