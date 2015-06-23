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

#ifndef CMS_TEMPLATETRACKINGMODULE_H
#define CMS_TEMPLATETRACKINGMODULE_H

#include "TrackingModule.h"

namespace CMS {

class TemplateTrackingModule : public ITrackingModule
{
public:
    TemplateTrackingModule(cv::Size templateSize);
    Point track(cv::Mat &frame);
    void setTrackPoint(cv::Mat frame, Point point);
    cv::Size getImageSize();
    bool initialized();

private:
    TrackingModuleSanityCheck sanityCheck;
    bool isInitialized;
    int workingWidth;
    cv::Size imageSize;
    cv::Size templateSize;
    cv::Mat templ;
    cv::Mat result;
    float scaleFactor;
    cv::Point2f prevPoint;

    int adjustPosition(int pos, int limit);
    cv::Point adjustPoint(cv::Point point, cv::Size limits);
    cv::Mat workingFrame(cv::Mat &frame);
};

} // namespace CMS

#endif // CMS_TEMPLATETRACKINGMODULE_H
