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

VideoManagerSurface::VideoManagerSurface(QLabel *imageLabel, QObject *parent) : QAbstractVideoSurface(parent)
{
    trackingModule = new TemplateTrackingModule(cv::Size(50, 50)); // TODO magic constants are not nice :(
    m_imageLabel = imageLabel;
    supportedFormats = QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB24
                                                         << QVideoFrame::Format_RGB32;
    connect(imageLabel, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
}

VideoManagerSurface::~VideoManagerSurface()
{
    delete(trackingModule);
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
        if (frameSize.isEmpty())
            frameSize = image.size();
        cv::Mat mat = ASM::QImageToCvMat(image);
        prevMat = mat;
        if (trackingModule->initialized())
        {
            Point p = trackingModule->track(mat);
            if (!p.empty())
                cv::circle(mat, p.asCVPoint(), 10, cv::Scalar(255, 255, 0));
        }

        image = ASM::cvMatToQImage(mat);
        image = image.scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // QPixmap::fromImage create a new buffer for the pixmap
        m_imageLabel->setPixmap(QPixmap::fromImage(image));

        // Release the data
        frameToProcess.unmap();

        m_imageLabel->update();

        return true;
    }
}

void VideoManagerSurface::mousePressEvent(QMouseEvent *event)
{
    if (frameSize.isEmpty())
        return;
    int x = frameSize.width() * event->x() / m_imageLabel->size().width();
    int y = frameSize.height() * event->y() / m_imageLabel->size().height();
    if (!prevMat.empty())
        trackingModule->setTrackPoint(prevMat, Point(x, y));
}

} // namespace CMS

