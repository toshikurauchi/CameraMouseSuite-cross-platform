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

#ifndef CMS_SETTINGS_H
#define CMS_SETTINGS_H

#include <QObject>

#include "Point.h"

namespace CMS {

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);

    bool isClickingEnabled();
    double getDwellTime();
    int getDwellTimeMillis();
    Point getGain();
    bool getReverseHorizontal();
    double getDamping();
    double getResetFeatureDistThreshSq();
    Point getFrameSize();
    bool isAutoDetectNoseEnabled();

signals:

public slots:
    void setEnableClicking(bool enableClicking);
    void setDwellTime(double dwellTime);
    Point getScreenResolution();
    double getDwellRadius();
    void setHorizontalGain(int horizontalGain);
    void setVerticalGain(int verticalGain);
    void setReverseHorizontal(bool reverseHorizontal);
    void setEnableSmoothing(bool enableSmoothing);
    void setDampingPercent(int damping);
    void setFrameSize(Point frameSize);
    void setAutoDetectNose(bool autoDetectNose);

private:
    bool enableClicking;
    double dwellTime;
    double radiusRel;
    Point screenResolution;
    int horizontalGain;
    int verticalGain;
    bool reverseHorizontal;
    bool enableSmoothing;
    double damping;
    Point frameSize;
    bool autoDetectNose;
};

} // namespace CMS

#endif // CMS_SETTINGS_H
