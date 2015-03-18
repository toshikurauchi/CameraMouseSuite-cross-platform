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
#include <qdebug.h>

#include "StandardTrackingModule.h"
#include "asmOpenCV.h"

namespace CMS {

StandardTrackingModule::StandardTrackingModule() :
    isInitialized(false),
    winSize(10, 10),
    criteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.03),
    imageSize(0,0)
{
}

Point StandardTrackingModule::track(cv::Mat &frame)
{
    if (!isInitialized)
        throw std::logic_error("No point set to be tracked");

    if (frame.empty())
        throw std::invalid_argument("Frame is empty!");

    if (frame.size() != imageSize)
        throw std::invalid_argument("Invalid frame sizes");

    cv::Mat grey = ASM::convertToGray(frame);

    //SwapPoints(ref _current_track_points[0], ref _last_track_points[0]);

    std::vector<uchar> featuresFound;
    cv::Mat err;
    std::vector<cv::Point2f> currentTrackPoints;
    cv::calcOpticalFlowPyrLK(prevGrey, grey, prevTrackPoints, currentTrackPoints,
                             featuresFound, err, winSize, 3, criteria);

    Point imagePoint;

    limitTPDelta(currentTrackPoints[0], prevTrackPoints[0]);

    if (featuresFound[0])
    {
        imagePoint = Point(currentTrackPoints[0]);
    }

    prevGrey = grey;
    prevTrackPoints = currentTrackPoints;

    return imagePoint;
}

void StandardTrackingModule::setTrackPoint(cv::Mat frame, Point point)
{
    if (frame.empty())
        throw std::invalid_argument("Frame is empty!");

    imageSize = frame.size();
    if (point.X() < 0 || point.X() >= imageSize.width ||
        point.Y() < 0 || point.Y() >= imageSize.height)
    {
        return;
    }

    isInitialized = true;

    prevTrackPoints = std::vector<cv::Point2f>();
    prevTrackPoints.push_back(point.asCVPoint());
    prevGrey = ASM::convertToGray(frame);
}

bool StandardTrackingModule::initialized()
{
    return isInitialized;
}

// find magnitude of change between cur and last TrackPoint in
// camera image coordinates. If too far limit change.
void StandardTrackingModule::limitTPDelta(cv::Point2f &cur, cv::Point2f &last)
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
