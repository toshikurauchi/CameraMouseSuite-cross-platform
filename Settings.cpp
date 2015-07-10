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

#include "Settings.h"
#include "Monitor.h"

namespace CMS {

Settings::Settings(QObject *parent) :
    QObject(parent),
    enableClicking(false),
    radiusRel(0.05),
    screenResolution(MonitorFactory::newMonitor()->getResolution()),
    reverseHorizontal(false),
    autoDetectNose(true)
{
}

bool Settings::isClickingEnabled()
{
    return enableClicking;
}

double Settings::getDwellTime()
{
    return dwellTime;
}

int Settings::getDwellTimeMillis()
{
    return (int) (dwellTime * 1000);
}

Point Settings::getGain()
{
    return Point(horizontalGain, verticalGain);
}

bool Settings::getReverseHorizontal()
{
    return reverseHorizontal;
}

double Settings::getDamping()
{
    double dp = 100;
    if (enableSmoothing)
        dp = damping;
    return dp;
}

double Settings::getResetFeatureDistThreshSq()
{
    if (frameSize.empty()) return 0;
    Point threshReg = frameSize * 0.02;
    return threshReg * threshReg;
}

Point Settings::getFrameSize()
{
    return frameSize;
}

bool Settings::isAutoDetectNoseEnabled()
{
    return autoDetectNose;
}

void Settings::setEnableClicking(bool enableClicking)
{
    this->enableClicking = enableClicking;
}

void Settings::setDwellTime(double dwellTime)
{
    this->dwellTime = dwellTime;
}

Point Settings::getScreenResolution()
{
    return screenResolution;
}

double Settings::getDwellRadius()
{
    return radiusRel * screenResolution.X();
}

void Settings::setHorizontalGain(int horizontalGain)
{
    this->horizontalGain = horizontalGain;
}

void Settings::setVerticalGain(int verticalGain)
{
    this->verticalGain = verticalGain;
}

void Settings::setReverseHorizontal(bool reverseHorizontal)
{
    this->reverseHorizontal = reverseHorizontal;
}

void Settings::setEnableSmoothing(bool enableSmoothing)
{
    this->enableSmoothing = enableSmoothing;
}

void Settings::setDampingPercent(int damping)
{
    this->damping = damping / 100.0;
}

void Settings::setFrameSize(Point frameSize)
{
    this->frameSize = frameSize;
}

void Settings::setAutoDetectNose(bool autoDetectNose)
{
    this->autoDetectNose = autoDetectNose;
}

} // namespace CMS

