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

#include "Camera.h"

namespace CMS {

Camera::Camera(QObject *parent) :
    QObject(parent), name(""), id(0)
{}

Camera::Camera(std::string name, int id, QObject *parent) :
    QObject(parent), name(name), id(id)
{}

std::string Camera::getName()
{
    return name;
}

int Camera::getId() {
    return id;
}

std::vector<Camera*> Camera::listCameras()
{
#ifdef Q_OS_LINUX
    return LinuxCamera::getCameraList();
#elif defined Q_OS_WIN
    return WindowsCamera::getCameraList();
#elif defined Q_OS_MAC
    return OSXCamera::getCameraList();
#endif
}

} // namespace CMS

