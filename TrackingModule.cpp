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

#include <stdexcept>
#include <QDebug>

#include "TrackingModule.h"
#include "ImageProcessing.h"

namespace CMS {

ITrackingModule::~ITrackingModule()
{}

void ITrackingModule::drawOnFrame(cv::Mat &frame, Point point)
{
    float ratio = 0.03;
    int width = (int) (frame.size().width * ratio);
    int height = (int) (frame.size().height * ratio);
    cv::Rect rectangle(point.X() - width / 2, point.Y() - height / 2, width, height);
    ImageProcessing::drawGreenRectangle(frame, rectangle);
}

TrackingModuleSanityCheck::TrackingModuleSanityCheck(ITrackingModule *trackingModule) :
    trackingModule(trackingModule)
{
}

bool TrackingModuleSanityCheck::checkInitialized()
{
    if (!trackingModule->isInitialized())
    {
        qDebug() << "No point set to be tracked";
        return false;
    }
    return true;
}

bool TrackingModuleSanityCheck::checkFrameNotEmpty(cv::Mat &frame)
{
    if (frame.empty())
    {
        qDebug() << "Frame is empty!";
        return false;
    }
    return true;
}

bool TrackingModuleSanityCheck::checkFrameSize(cv::Mat &frame)
{
    if (frame.size() != trackingModule->getImageSize())
    {
        trackingModule->reset();
        return false;
    }
    return true;
}

// find magnitude of change between cur and last TrackPoint in
// camera image coordinates. If too far limit change.
void TrackingModuleSanityCheck::limitTPDelta(cv::Point2f &cur, cv::Point2f &last)
{
    double difX = cur.x - last.x;
    double difY = cur.y - last.y;
    double dist = difX * difX + difY * difY;
    if (dist > 35*35)
    {
        cur.x = last.x;
        cur.y = last.y;
    }
}

} // namespace CMS
