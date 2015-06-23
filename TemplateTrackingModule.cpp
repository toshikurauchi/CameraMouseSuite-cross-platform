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

#include "TemplateTrackingModule.h"

namespace CMS {

TemplateTrackingModule::TemplateTrackingModule(cv::Size templateSize) :
    sanityCheck(this),
    isInitialized(false),
    workingWidth(640),
    templateSize(templateSize)
{
}

Point TemplateTrackingModule::track(cv::Mat &frame)
{
    sanityCheck.checkInitialized();
    sanityCheck.checkFrameNotEmpty(frame);
    sanityCheck.checkFrameSize(frame);

    cv::Mat smallFrame = workingFrame(frame);

    // Do the Matching and Normalize
    int match_method = CV_TM_SQDIFF;
    cv::matchTemplate(smallFrame, templ, result, match_method);
    cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    // Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::Point matchLoc;

    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }

    matchLoc = adjustPoint(matchLoc, imageSize - templateSize);

    // Update to center of template
    cv::Point2f curPoint(matchLoc.x + templateSize.width/2, matchLoc.y + templateSize.height/2);

    Point imagePoint(curPoint);

    // Update template
    prevPoint = curPoint;
    cv::Rect roi(matchLoc.x, matchLoc.y, templateSize.width, templateSize.height);
    templ = smallFrame(roi);

    return imagePoint / scaleFactor;
}

void TemplateTrackingModule::setTrackPoint(cv::Mat frame, Point point)
{
    sanityCheck.checkFrameNotEmpty(frame);

    imageSize = frame.size();
    if (point.X() < 0 || point.X() >= imageSize.width ||
        point.Y() < 0 || point.Y() >= imageSize.height)
    {
        return;
    }

    cv::Mat smallFrame = workingFrame(frame);
    // scaleFactor is only set now
    point = point * scaleFactor;

    // Get positions of the top-left corner of the regio of interest (template) centered in (x,y)
    cv::Point roiOrigin = adjustPoint(point.asCVIntPoint() - cv::Point(templateSize.width/2, templateSize.height/2), imageSize - templateSize);
    cv::Rect roi(roiOrigin.x, roiOrigin.y, templateSize.width, templateSize.height);
    templ = smallFrame(roi);
    prevPoint = cv::Point2f(roiOrigin.x + templateSize.width/2, roiOrigin.y + templateSize.height/2);

    // Create the result matrix
    int result_cols =  smallFrame.cols - templ.cols + 1;
    int result_rows = smallFrame.rows - templ.rows + 1;
    result.create(result_rows, result_cols, CV_32FC1);

    isInitialized = true;
}

cv::Size TemplateTrackingModule::getImageSize()
{
    return imageSize;
}

bool TemplateTrackingModule::initialized()
{
    return isInitialized;
}

int TemplateTrackingModule::adjustPosition(int pos, int limit)
{
    if (pos < 0)
    {
        return 0;
    }
    else if (pos >= limit)
    {
        return limit - 1;
    }
    return pos;
}

cv::Point TemplateTrackingModule::adjustPoint(cv::Point point, cv::Size limits)
{
    int x = adjustPosition(point.x, limits.width);
    int y = adjustPosition(point.y, limits.height);
    return cv::Point(x, y);
}

cv::Mat TemplateTrackingModule::workingFrame(cv::Mat &frame)
{
    cv::Mat resizedFrame;
    scaleFactor = (float) workingWidth / frame.size().width;
    cv::resize(frame, resizedFrame, cv::Size(), scaleFactor, scaleFactor);
    return resizedFrame;
}

} // namespace CMS

