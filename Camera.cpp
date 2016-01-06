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

#ifdef Q_OS_LINUX
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#endif

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

#ifdef Q_OS_LINUX
std::vector<Camera*> LinuxCamera::getCameraList()
{
    std::vector<Camera*> cameras;

    for (int id = 0; true; id++)
    {
        std::stringstream stream;
        stream << "/dev/video";
        stream << id;
        int fd = open(stream.str().c_str(), O_RDWR);
        if (fd == -1) break;

        v4l2_capability capability;
        ioctl(fd, VIDIOC_QUERYCAP, &capability);
        cameras.push_back(new Camera(std::string(reinterpret_cast<char*>(capability.card)), id));

        close(fd);
    }
    if (cameras.size() == 0)
        qDebug() << "No camera found. Please verify that the camera is properly connected.";

    return cameras;
}
#endif

} // namespace CMS

