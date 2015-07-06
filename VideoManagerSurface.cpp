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

VideoManagerSurface::VideoManagerSurface(QLabel *imageLabel, QObject *parent) :
    QAbstractVideoSurface(parent),
    controlling(false),
    restarted(true),
    keyboard(KeyboardFactory::newKeyboard())
{
    trackingModule = new TemplateTrackingModule(0.08); // TODO magic constants are not nice :(
    controlModule = new MouseControlModule;
    m_imageLabel = imageLabel;
    supportedFormats = QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB24
                                                         << QVideoFrame::Format_RGB32;
    connect(imageLabel, SIGNAL(mousePressed(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
}

VideoManagerSurface::~VideoManagerSurface()
{
    delete(trackingModule);
    delete(keyboard);
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
    while (keyboard->hasNextEvent())
    {
        KeyEvent event = keyboard->nextEvent();
        if (event.getKey() == KEY_CONTROL && event.getState() == KEY_STATE_DOWN)
        {
            controlling = !controlling;
            if (controlling)
            {
                restarted = true;
            }
        }
    }

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
        if (trackingModule->isInitialized())
        {
            Point featurePosition = trackingModule->track(mat);
            if (!featurePosition.empty())
            {
                cv::circle(mat, featurePosition.asCVPoint(), 10, cv::Scalar(255, 255, 0));
                if (controlling)
                {
                    if (restarted)
                    {
                        controlModule->setFeatureReference(featurePosition);
                        restarted = false;
                    }
                    controlModule->update(featurePosition);
                }
            }
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
    Point position(x, y);
    if (!prevMat.empty())
    {
        trackingModule->setTrackPoint(prevMat, position);
        controlModule->setFeatureReference(position);
    }
}

} // namespace CMS

