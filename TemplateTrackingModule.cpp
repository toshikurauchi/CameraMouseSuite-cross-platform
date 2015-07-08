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

#include <opencv2/imgproc.hpp>

#include "TemplateTrackingModule.h"

namespace CMS {

// TODO: I think we can extract some methods for what we do for the scaled and full image...

TemplateTrackingModule::TemplateTrackingModule(double templateSizeRatio) :
    sanityCheck(this),
    initialized(false),
    workingWidth(640),
    templateSizeRatio(templateSizeRatio),
    templateSize((Point(workingWidth, workingWidth)*templateSizeRatio).asCVIntPoint())
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

    // Update template for scaled image
    cv::Rect roi(matchLoc.x, matchLoc.y, templateSize.width, templateSize.height);
    templ = smallFrame(roi);

    // Get search region around the best match for the scaled image
    int offX = (int) (matchLoc.x - 1) / scaleFactor;
    int offY = (int) (matchLoc.y - 1) / scaleFactor;
    int searchWidth = fullTemplateSize.width + (int) 2 / scaleFactor;
    int searchHeight = fullTemplateSize.height + (int) 2 / scaleFactor;
    cv::Mat searchRegion(frame(cv::Rect(offX, offY, searchWidth, searchHeight)));

    // Do the Matching and Normalize on the full image
    cv::matchTemplate(searchRegion, fullTempl, fullResult, match_method);
    cv::normalize(fullResult, fullResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    // Localize best match for full image (on the search region)
    cv::minMaxLoc(fullResult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }
    matchLoc = adjustPoint(matchLoc, fullImageSize - fullTemplateSize);

    // Update template for full image
    cv::Rect fullRoi(matchLoc.x + offX, matchLoc.y + offY, fullTemplateSize.width, fullTemplateSize.height);
    fullTempl = frame(fullRoi);

    // Return center of matched region
    return Point(matchLoc.x + offX + fullTemplateSize.width/2, matchLoc.y + offY + fullTemplateSize.height/2);
}

void TemplateTrackingModule::setTrackPoint(cv::Mat frame, Point point)
{
    sanityCheck.checkFrameNotEmpty(frame);

    fullImageSize = frame.size();
    fullTemplateSize = (Point(frame.size().width, frame.size().width)*templateSizeRatio).asCVIntPoint();
    if (point.X() < 0 || point.X() >= fullImageSize.width ||
        point.Y() < 0 || point.Y() >= fullImageSize.height)
    {
        return;
    }

    cv::Mat smallFrame = workingFrame(frame);
    imageSize = smallFrame.size();
    // scaleFactor is only set now
    Point scaledPoint = point * scaleFactor;

    // Get positions of the top-left corner of the region of interest (template) centered in (x,y) on the scaled image
    cv::Point roiOrigin = adjustPoint(scaledPoint.asCVIntPoint() - cv::Point(templateSize.width/2, templateSize.height/2), imageSize - templateSize);
    cv::Rect roi(roiOrigin.x, roiOrigin.y, templateSize.width, templateSize.height);
    templ = smallFrame(roi);

    // Get positions of the top-left corner of the region of interest (template) centered in (x,y) on the full image
    cv::Point fullRoiOrigin = adjustPoint(point.asCVIntPoint() - cv::Point(fullTemplateSize.width/2, fullTemplateSize.height/2), fullImageSize - fullTemplateSize);
    cv::Rect fullRoi(fullRoiOrigin.x, fullRoiOrigin.y, fullTemplateSize.width, fullTemplateSize.height);
    fullTempl = frame(fullRoi);

    // Create the result matrix for the scaled image
    int resultCols =  smallFrame.cols - templ.cols + 1;
    int resultRows = smallFrame.rows - templ.rows + 1;
    result.create(resultRows, resultCols, CV_32FC1);

    // Create the result matrix for the full image (only around the matched region for the scaled image)
    int fullResultCols =  (int) 3/scaleFactor + 1;
    int fullResultRows = (int) 3/scaleFactor + 1;
    fullResult.create(fullResultRows, fullResultCols, CV_32FC1);

    initialized = true;
}

cv::Size TemplateTrackingModule::getImageSize()
{
    return fullImageSize;
}

bool TemplateTrackingModule::isInitialized()
{
    return initialized;
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

