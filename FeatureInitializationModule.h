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

#ifndef CMS_FEATUREINITIALIZATIONMODULE_H
#define CMS_FEATUREINITIALIZATIONMODULE_H

#include <QObject>
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
#include <opencv2/opencv.hpp>
#endif

#include <cv.h>
#include <vector>

#include "Point.h"

namespace CMS {

class FeatureInitializationModule
{
public:
    FeatureInitializationModule();
    bool allFilesLoaded();
    Point initializeFeature(cv::Mat &frame);
private:
    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier leftEyeCascade;
    cv::CascadeClassifier rightEyeCascade;
    cv::CascadeClassifier noseCascade;
    cv::CascadeClassifier mouthCascade;
    bool filesLoaded;

    cv::Rect detectNose(cv::Mat &face);
    void applyGeometricConstraints(std::vector<cv::Rect> &leftEyes,
                                   std::vector<cv::Rect> &rightEyes,
                                   std::vector<cv::Rect> &noses,
                                   std::vector<cv::Rect> &mouths);
    Point centerOfRect(cv::Rect rect);
};

} // namespace CMS

#endif // CMS_FEATUREINITIALIZATIONMODULE_H
