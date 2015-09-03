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
#ifdef Q_OS_LINUX
#include <opencv2/imgproc.hpp>
#endif

#include "CameraMouseController.h"

namespace CMS {

CameraMouseController::CameraMouseController(Settings &settings, ITrackingModule *trackingModule, MouseControlModule *controlModule) :
    settings(settings), trackingModule(trackingModule), controlModule(controlModule)
{
    featureCheckTimer.start();
}

CameraMouseController::~CameraMouseController()
{
    delete trackingModule;
    delete controlModule;
}

void CameraMouseController::processFrame(cv::Mat &frame)
{
    prevFrame = frame;

    if (trackingModule->isInitialized())
    {
        Point featurePosition = trackingModule->track(frame);
        if (!featurePosition.empty())
        {
            if (settings.isAutoDetectNoseEnabled() && featureCheckTimer.elapsed() > 1000)
            {
                Point autoFeaturePosition = initializationModule.initializeFeature(frame);
                if (!autoFeaturePosition.empty())
                {
                    double distThreshSq = settings.getResetFeatureDistThreshSq();
                    Point disp = autoFeaturePosition - featurePosition;
                    if (disp * disp > distThreshSq)
                    {
                        trackingModule->setTrackPoint(frame, autoFeaturePosition);
                        controlModule->setScreenReference(controlModule->getPrevPos());
                        controlModule->restart();
                        featurePosition = autoFeaturePosition;
                    }
                    featureCheckTimer.restart();
                }
            }
            trackingModule->drawOnFrame(frame, featurePosition);
            controlModule->update(featurePosition);
        }
    }
    else if (settings.isAutoDetectNoseEnabled())
    {
        Point initialFeaturePosition = initializationModule.initializeFeature(frame);
        if (!initialFeaturePosition.empty())
        {
            trackingModule->setTrackPoint(frame, initialFeaturePosition);
            controlModule->setScreenReference(settings.getScreenResolution()/2);
            controlModule->restart();
        }
    }
}

void CameraMouseController::processClick(Point position)
{
    if (!prevFrame.empty())
    {
        trackingModule->setTrackPoint(prevFrame, position);
        controlModule->restart();
    }
}

bool CameraMouseController::isAutoDetectWorking()
{
    return initializationModule.allFilesLoaded();
}

} // namespace CMS

