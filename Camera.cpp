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
#include <QRegularExpression>
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

std::vector<Camera*> LinuxCamera::getCameraList()
{
    std::vector<Camera*> cameras;
    QString listString = QString::fromStdString(exec("uvcdynctrl -l"));
    QRegularExpressionMatchIterator matched = QRegularExpression("video(\\d)\\s+?(.*?)\n").globalMatch(listString);
    if (!matched.hasNext())
    {
        qDebug() << "No camera found. Please verify that you have installed uvcdynctrl and that the camera is properly connected.";
        return cameras;
    }
    while (matched.hasNext())
    {
        QRegularExpressionMatch videoInfo = matched.next();
        int id = videoInfo.captured(1).trimmed().toInt();
        QString name = videoInfo.captured(2).trimmed();
        cameras.push_back(new Camera(name.toStdString(), id));
    }
    return cameras;
}

std::string LinuxCamera::exec(std::string cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    const int bufferSize = 1024;
    char buffer[bufferSize];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, bufferSize, pipe) != NULL)
                result += buffer;
    }
    pclose(pipe);
    return result;
}

} // namespace CMS

