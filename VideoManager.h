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

#ifndef CMS_VIDEOMANAGER_H
#define CMS_VIDEOMANAGER_H

#include <QObject>
#include <QList>
#include <QLabel>
#include <QTimer>
#include <opencv/cv.h>
#include <highgui.h>

#include "TrackingModule.h"
#include "MouseControlModule.h"
#include "Keyboard.h"
#include "CameraMouseController.h"
#include "Point.h"
#include "Settings.h"
#include "Camera.h"

namespace CMS {

// TODO I think this class has too many resposibilities

class VideoManager : public QObject
{
    Q_OBJECT

public:
    VideoManager(Settings &settings, CameraMouseController *controller, QLabel *imageLabel, QObject *parent = 0);
    ~VideoManager();
    void setCamera(Camera *camera);

protected slots:
    void present();
    void mousePressEvent(QMouseEvent *event);

private:
    cv::VideoCapture capture;
    int captureId;
    QTimer *timer;
    Settings &settings;
    CameraMouseController *controller;
    QLabel *imageLabel;
    QSize frameSize;
    QSize scaledFrameSize;
    Point frameOffset;
};

} // namespace CMS

#endif // CMS_VIDEOMANAGER_H
