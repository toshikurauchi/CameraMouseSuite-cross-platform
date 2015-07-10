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
    TemplateTrackingModule(double templateSizeRatio);
    Point track(cv::Mat &frame);
    void setTrackPoint(cv::Mat &frame, Point point);
    void drawOnFrame(cv::Mat &frame, Point point);
    cv::Size getImageSize();
    bool isInitialized();

private:
    TrackingModuleSanityCheck sanityCheck;
    bool initialized;
    int workingWidth;
    cv::Size imageSize;
    cv::Size fullImageSize;
    double templateSizeRatio;
    cv::Size templateSize;
    cv::Size fullTemplateSize;
    cv::Mat templ;
    cv::Mat fullTempl;
    cv::Mat result;
    cv::Mat fullResult;
    float scaleFactor;
    Point prevLoc;

    int adjustPosition(int pos, int limit);
    cv::Point adjustPoint(cv::Point point, cv::Size limits);
    cv::Mat workingFrame(cv::Mat &frame);
    cv::Point match(cv::Mat &frame, cv::Mat &tmpl, cv::Size limits, cv::Point searchCenter, cv::Size searchSize);
};

} // namespace CMS

#endif // CMS_TEMPLATETRACKINGMODULE_H
