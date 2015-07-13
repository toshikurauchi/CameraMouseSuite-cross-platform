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

#include <algorithm>
#include <QCoreApplication>
#include <QDir>
#include <set>
#include <QDebug>

#include "FeatureInitializationModule.h"

namespace CMS {

FeatureInitializationModule::FeatureInitializationModule()
{
    filesLoaded = true;

    QString dir = QDir::currentPath();
    qDebug() << QCoreApplication::applicationDirPath();
    qDebug() << dir;
    if (!faceCascade.load("cascades/haarcascade_frontalface_alt.xml"))
        filesLoaded = false;
    if (!leftEyeCascade.load("cascades/haarcascade_mcs_lefteye.xml"))
        filesLoaded = false;
    if (!rightEyeCascade.load("cascades/haarcascade_mcs_righteye.xml"))
        filesLoaded = false;
    if (!noseCascade.load("cascades/haarcascade_mcs_nose.xml"))
        filesLoaded = false;
    if (!mouthCascade.load("cascades/haarcascade_mcs_mouth.xml"))
        filesLoaded = false;
}

// **** Rectangle comparators ****
// Use an unnamed namespace to restrict global variables scope
namespace {
Point center;
} // namespace

bool compareRectByProximityToCenter(cv::Rect r1, cv::Rect r2)
{
    Point d1(r1.x + r1.width / 2.0 - center.X(), r1.y + r1.height / 2.0 - center.Y());
    Point d2(r2.x + r2.width / 2.0 - center.X(), r2.y + r2.height / 2.0 - center.Y());
    return d1*d1 > d2*d2;
}
// *******************************

bool compareRectByHeight(cv::Rect r1, cv::Rect r2)
{
    return r1.height < r2.height;
}

Point FeatureInitializationModule::initializeFeature(cv::Mat &frame)
{
    if (!filesLoaded)
    {
        return Point();
    }

    center = Point(frame.size().width / 2.0, frame.size().height / 2.0);

    // Minimum face size to detect
    int minFaceH = std::max<int>(50, (int)(0.075*frame.size().height));
    cv::Size minFace(minFaceH, minFaceH);

    std::vector<cv::Rect> candidateNoses;
    std::vector<cv::Rect> faces;
    cv::Mat pyrDownFrame;
    cv::pyrDown(frame, pyrDownFrame);
    faceCascade.detectMultiScale(pyrDownFrame, faces, 1.2, 2, 0, minFace);
    for (std::vector<cv::Rect>::iterator it = faces.begin(); it != faces.end(); it++)
    {
        cv::Mat face;
        it->x *= 2;
        it->y *= 2;
        it->width *= 2;
        it->height *= 2;
        face = frame(*it);

        cv::Rect nose = detectNose(face);
        if (nose.width > 0 && nose.height > 0) // Found nose!
        {
            nose.x += it->x;
            nose.y += it->y;
            candidateNoses.push_back(nose);
        }
    }
    if (candidateNoses.size() == 0) return Point();
    cv::Rect detectedNose = *std::max_element(candidateNoses.begin(), candidateNoses.end(), compareRectByProximityToCenter);
    return Point(detectedNose.x + detectedNose.width / 2, detectedNose.y + detectedNose.height / 2);
}

cv::Rect FeatureInitializationModule::detectNose(cv::Mat &face)
{
    // Minimum face feature size to detect
    int minFaceFeatureH = std::max<int>(50, (int)(0.075*face.size().height));
    cv::Size minFaceFeature(minFaceFeatureH, minFaceFeatureH);

    std::vector<cv::Rect> leftEyes;
    std::vector<cv::Rect> rightEyes;
    std::vector<cv::Rect> noses;
    std::vector<cv::Rect> mouths;
    leftEyeCascade.detectMultiScale(face, leftEyes, 1.2, 2, 0, minFaceFeature);
    rightEyeCascade.detectMultiScale(face, rightEyes, 1.2, 2, 0, minFaceFeature);
    noseCascade.detectMultiScale(face, noses, 1.2, 2, 0, minFaceFeature);
    mouthCascade.detectMultiScale(face, mouths, 1.2, 2, 0, minFaceFeature);

    applyGeometricConstraints(leftEyes, rightEyes, noses, mouths);

    cv::Rect nose(0, 0, 0, 0);
    if (leftEyes.size() && rightEyes.size() && noses.size() && mouths.size()) // Found all features!
    {
        nose = *std::max_element(noses.begin(), noses.end(), compareRectByHeight);
    }

    return nose;
}

struct CompareRect
{
    bool operator() (const cv::Rect &r1, const cv::Rect &r2) {
        if (r1.x == r2.x)
        {
            if (r1.y == r2.y)
            {
                if (r1.width == r2.width)
                {
                    if (r1.height == r2.height)
                        return false;
                    return r1.height < r2.height;
                }
                return r1.width < r2.width;
            }
            return r1.y < r2.y;
        }
        return r1.x < r2.x;
    }
};

void FeatureInitializationModule::applyGeometricConstraints(std::vector<cv::Rect> &leftEyes,
                                                            std::vector<cv::Rect> &rightEyes,
                                                            std::vector<cv::Rect> &noses,
                                                            std::vector<cv::Rect> &mouths)
{
    std::set<cv::Rect, CompareRect> filteredLeftEyes;
    std::set<cv::Rect, CompareRect> filteredRightEyes;
    std::set<cv::Rect, CompareRect> filteredNoses;
    std::set<cv::Rect, CompareRect> filteredMouths;

    for (std::vector<cv::Rect>::iterator lEyeIt = leftEyes.begin(); lEyeIt != leftEyes.end(); lEyeIt++)
    {
        Point rightEye = centerOfRect(*lEyeIt);
        for (std::vector<cv::Rect>::iterator rEyeIt = rightEyes.begin(); rEyeIt != rightEyes.end(); rEyeIt++)
        {
            Point leftEye = centerOfRect(*rEyeIt);
            if (leftEye.X() < rightEye.X())
            {
                for (std::vector<cv::Rect>::iterator noseIt = noses.begin(); noseIt != noses.end(); noseIt++)
                {
                    Point nose = centerOfRect(*noseIt);
                    if (nose.X() > leftEye.X() && nose.X() < rightEye.X() && nose.Y() > leftEye.Y() && nose.Y() > rightEye.Y())
                    {
                        for (std::vector<cv::Rect>::iterator mouthIt = mouths.begin(); mouthIt != mouths.end(); mouthIt++)
                        {
                            Point mouth = centerOfRect(*mouthIt);
                            if (mouth.X() > leftEye.X() && mouth.X() < rightEye.X() && mouth.Y() > nose.Y())
                            {
                                filteredLeftEyes.insert(*lEyeIt);
                                filteredRightEyes.insert(*rEyeIt);
                                filteredNoses.insert(*noseIt);
                                filteredMouths.insert(*mouthIt);
                            }
                        }
                    }
                }
            }
        }
    }
    leftEyes.clear();
    leftEyes.insert(leftEyes.begin(), filteredLeftEyes.begin(), filteredLeftEyes.end());
    rightEyes.clear();
    rightEyes.insert(rightEyes.begin(), filteredRightEyes.begin(), filteredRightEyes.end());
    noses.clear();
    noses.insert(noses.begin(), filteredNoses.begin(), filteredNoses.end());
    mouths.clear();
    mouths.insert(mouths.begin(), filteredMouths.begin(), filteredMouths.end());
}

// TODO Move this to somewhere that makes more sense (and I think it could be used in other places as well)
Point FeatureInitializationModule::centerOfRect(cv::Rect rect)
{
    return Point(rect.x + rect.width / 2, rect.y + rect.height);
}

} // namespace CMS
