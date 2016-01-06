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

#ifndef CMS_CAMERA_H
#define CMS_CAMERA_H

#include <QObject>
#include <vector>
#include <string>

namespace CMS {

class Camera : public QObject
{
    Q_OBJECT
public:
    Camera(QObject *parent=0);
    Camera(std::string name, int id, QObject *parent=0);
    std::string getName();
    int getId();

    static std::vector<Camera*> listCameras();
private:
    std::string name;
    int id;
};

#ifdef Q_OS_LINUX

class LinuxCamera
{
public:
    static std::vector<Camera*> getCameraList();
};

#elif defined Q_OS_WIN

class WindowsCamera
{
public:
    static std::vector<Camera*> getCameraList();
};

#elif defined Q_OS_MAC

class OSXCamera
{
public:
    static std::vector<Camera*> getCameraList();
};

#endif

} // namespace CMS

#endif // CMS_CAMERA_H
