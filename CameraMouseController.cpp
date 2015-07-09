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

#include "CameraMouseController.h"

namespace CMS {

CameraMouseController::CameraMouseController(ITrackingModule *trackingModule, MouseControlModule *controlModule) :
    trackingModule(trackingModule), controlModule(controlModule)
{
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
            cv::circle(frame, featurePosition.asCVPoint(), 10, cv::Scalar(255, 255, 0));
            controlModule->update(featurePosition);
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

} // namespace CMS

