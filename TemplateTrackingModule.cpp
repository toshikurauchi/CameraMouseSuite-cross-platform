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

#include <QObject>
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
#include <opencv2/imgproc.hpp>
#endif

#include "TemplateTrackingModule.h"
#include "ImageProcessing.h"

namespace CMS {

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

    // Match template (only look in a region around the previous position)
    cv::Size searchSize(smallFrame.size().width / 3, smallFrame.size().height / 3);
    cv::Point matchLoc = match(smallFrame, templ, imageSize - templateSize, (prevLoc * scaleFactor).asCVIntPoint(), searchSize);
    // Update template for scaled image
    cv::Rect roi(matchLoc.x, matchLoc.y, templateSize.width, templateSize.height);
    templ = smallFrame(roi);

    // Match template for full image (only look a few pixels around the matched region
    cv::Point searchCenter((int) (matchLoc.x / scaleFactor + fullTemplateSize.width / 2),
                           (int) (matchLoc.y / scaleFactor + fullTemplateSize.height / 2));
    searchSize = cv::Size(fullTemplateSize.width + (int) 10 / scaleFactor,
                          fullTemplateSize.height + (int) 10 / scaleFactor);
    matchLoc = match(frame, fullTempl, fullImageSize - fullTemplateSize, searchCenter, searchSize);
    // Update template for full image
    cv::Rect fullRoi(matchLoc.x, matchLoc.y, fullTemplateSize.width, fullTemplateSize.height);
    fullTempl = frame(fullRoi);

    // Return center of matched region
    prevLoc = Point(matchLoc.x + fullTemplateSize.width/2, matchLoc.y + fullTemplateSize.height/2);
    return prevLoc;
}

void TemplateTrackingModule::setTrackPoint(cv::Mat &frame, Point point)
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

    prevLoc = point;
    initialized = true;
}

void TemplateTrackingModule::drawOnFrame(cv::Mat &frame, Point point)
{
#ifdef DEBUGING
    int width = fullTemplateSize.width;
    int height = fullTemplateSize.height;
    cv::Rect rectangle(point.X() - width / 2, point.Y() - height / 2, width, height);
    ImageProcessing::drawGreenRectangle(frame, rectangle);
#else
    ITrackingModule::drawOnFrame(frame, point);
#endif
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

cv::Point TemplateTrackingModule::match(cv::Mat &frame, cv::Mat &tmpl, cv::Size limits, cv::Point searchCenter, cv::Size searchSize)
{
    // Adjust search region
    int offX = (2 * searchCenter.x - searchSize.width) / 2; // Subtracting first may change the result
    int offY = (2 * searchCenter.y - searchSize.height) / 2; // Subtracting first may change the result
    if (offX < 0) offX = 0;
    if (offY < 0) offY = 0;
    int searchWidth = searchSize.width;
    int searchHeight = searchSize.height;
    if (offX + searchWidth > frame.size().width) searchWidth = frame.size().width - offX;
    if (offY + searchHeight > frame.size().height) searchHeight = frame.size().height - offY;
    if (searchWidth < tmpl.size().width || searchHeight < tmpl.size().height) // Search area is smaller than template
    {
        return searchCenter;
    }
    cv::Mat searchRegion(frame(cv::Rect(offX, offY, searchWidth, searchHeight)));

    int resultCols =  searchRegion.cols - tmpl.cols + 1;
    int resultRows = searchRegion.rows - tmpl.rows + 1;
    cv::Mat result(resultRows, resultCols, CV_32FC1);

    // Do the Matching and Normalize
    int match_method = CV_TM_SQDIFF;
    cv::matchTemplate(searchRegion, tmpl, result, match_method);
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
    return adjustPoint(matchLoc + cv::Point(offX, offY), limits);
}

} // namespace CMS

