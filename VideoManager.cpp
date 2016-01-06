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

// This code is partially based on http://stackoverflow.com/questions/26229633/use-of-qabstractvideosurface

#include <QMouseEvent>

#include "VideoManager.h"
#include "asmOpenCV.h"
#include "Point.h"
#include "TemplateTrackingModule.h"

namespace CMS {

VideoManager::VideoManager(Settings &settings, CameraMouseController *controller, QLabel *imageLabel, QObject *parent) :
    QObject(parent),
    captureId(-1),
    timer(new QTimer(this)),
    settings(settings),
    controller(controller)
{
    this->imageLabel = imageLabel;
    timer->setInterval(30);
    connect(timer, SIGNAL(timeout()), this, SLOT(present()));
    timer->start();
    connect(imageLabel, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
}

VideoManager::~VideoManager()
{
    // TODO Move to MainWindow if we decide to keep the pointer there
    delete(controller);

    delete(timer);
}

void VideoManager::setCamera(Camera *camera)
{
    if (captureId != camera->getId())
    {
        capture.open(camera->getId());
    }
    captureId = camera->getId();
}

void VideoManager::present()
{
    if (!capture.isOpened()) return;

    cv::Mat mat;
    capture >> mat;
    if (mat.empty()) return;

    // The kind of mirroring needed depends on the OS
    cv::Mat mirrored;
    #ifdef Q_OS_LINUX
        cv::flip(mat, mirrored, 1);
    #elif defined Q_OS_WIN
        cv::flip(mat, mirrored, -1);
    #elif defined Q_OS_MAC
        cv::flip(mat, mirrored, 1);
    #endif
    mat = mirrored;

    controller->processFrame(mat);

    QImage image = ASM::cvMatToQImage(mat);
    QImage scaledImage = image.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (frameSize.isEmpty() || frameSize != image.size())
    {
        settings.setFrameSize(Point(image.size()));
        frameSize = image.size();
        scaledFrameSize = scaledImage.size();
        frameOffset = Point(imageLabel->size().width() - scaledImage.width(), imageLabel->size().height() - scaledImage.height())/2;
    }

    // QPixmap::fromImage create a new buffer for the pixmap
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));

    imageLabel->update();
}

void VideoManager::mousePressEvent(QMouseEvent *event)
{
    if (frameSize.isEmpty())
        return;
    double offX = frameOffset.X();
    double offY = frameOffset.Y();
    double x = (double) frameSize.width() * (event->x() - offX) / scaledFrameSize.width();
    double y = (double) frameSize.height() * (event->y() - offY) / scaledFrameSize.height();
    controller->processClick(Point(x, y));
}

} // namespace CMS

