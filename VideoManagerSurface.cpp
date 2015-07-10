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

#include "VideoManagerSurface.h"
#include "asmOpenCV.h"
#include "Point.h"
#include "TemplateTrackingModule.h"

namespace CMS {

VideoManagerSurface::VideoManagerSurface(CameraMouseController *controller, QLabel *imageLabel, QObject *parent) :
    QAbstractVideoSurface(parent),
    controller(controller)
{
    this->imageLabel = imageLabel;
    supportedFormats = QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB24
                                                         << QVideoFrame::Format_RGB32;
    connect(imageLabel, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
}

VideoManagerSurface::~VideoManagerSurface()
{
    // TODO Move to MainWindow if we decide to keep the pointer there
    delete(controller);
}

QList<QVideoFrame::PixelFormat> VideoManagerSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle)
    {
        return supportedFormats;
    }
    else
    {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool VideoManagerSurface::present(const QVideoFrame &frame)
{
    if (!supportedFormats.contains(frame.pixelFormat()))
    {
        setError(IncorrectFormatError);
        return false;
    }
    else
    {
        QVideoFrame frameToProcess(frame);

        if(!frameToProcess.map(QAbstractVideoBuffer::ReadWrite))
        {
           setError(ResourceError);
           return false;
        }

         // This is a shallow operation. it just refer the frame buffer
        QImage image(
                frameToProcess.bits(),
                frameToProcess.width(),
                frameToProcess.height(),
                frameToProcess.bytesPerLine(),
                QVideoFrame::imageFormatFromPixelFormat(frameToProcess.pixelFormat()));
        // The kind of mirroring needed depends on the OS
        #ifdef Q_OS_LINUX
            image = image.mirrored(true, false);
        #elif defined Q_OS_WIN
            image = image.mirrored(true, true);
        #elif defined Q_OS_MAC
            image = image.mirrored(true, false);
        #endif
        cv::Mat mat = ASM::QImageToCvMat(image);

        controller->processFrame(mat);

        image = ASM::cvMatToQImage(mat);
        QImage scaledImage = image.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        if (frameSize.isEmpty())
        {
            frameSize = image.size();
            scaledFrameSize = scaledImage.size();
            frameOffset = Point(imageLabel->size().width() - scaledImage.width(), imageLabel->size().height() - scaledImage.height())/2;
        }

        // QPixmap::fromImage create a new buffer for the pixmap
        imageLabel->setPixmap(QPixmap::fromImage(scaledImage));

        // Release the data
        frameToProcess.unmap();

        imageLabel->update();

        return true;
    }
}

void VideoManagerSurface::mousePressEvent(QMouseEvent *event)
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

