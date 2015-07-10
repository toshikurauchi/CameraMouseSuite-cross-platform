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

#ifndef CMS_STANDARDTRACKINGMODULE_H
#define CMS_STANDARDTRACKINGMODULE_H

#include <opencv/cv.h>
#include <vector>

#include "Point.h"
#include "TrackingModule.h"

namespace CMS {

class StandardTrackingModule : public ITrackingModule
{
public:
    StandardTrackingModule();
    Point track(cv::Mat &frame);
    void setTrackPoint(cv::Mat &frame, Point point);
    cv::Size getImageSize();
    bool isInitialized();

private:
    TrackingModuleSanityCheck sanityCheck;
    bool initialized;
    cv::Size winSize;
    cv::TermCriteria criteria;
    cv::Mat prevGrey;
    std::vector<cv::Point2f> prevTrackPoints;
    cv::Size imageSize;
};

} // namespace CMS

#endif // CMS_STANDARDTRACKINGMODULE_H
