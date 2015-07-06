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

#ifndef CMS_VIDEOMANAGERSURFACE_H
#define CMS_VIDEOMANAGERSURFACE_H

#include <QList>
#include <QLabel>
#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include <opencv/cv.h>

#include "TrackingModule.h"
#include "MouseControlModule.h"
#include "Keyboard.h"

namespace CMS {

// TODO I think this class has too many resposibilities

class VideoManagerSurface : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    VideoManagerSurface(QLabel *imageLabel, QObject *parent = 0);
    ~VideoManagerSurface();
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool present(const QVideoFrame &frame);

protected slots:
    void mousePressEvent(QMouseEvent *event);

private:
    QLabel *m_imageLabel;
    QList<QVideoFrame::PixelFormat> supportedFormats;
    QSize frameSize;
    ITrackingModule *trackingModule;
    MouseControlModule *controlModule;
    cv::Mat prevMat;
    bool controlling;
    bool restarted;
    IKeyboard *keyboard;
};

} // namespace CMS

#endif // CMS_VIDEOMANAGERSURFACE_H
