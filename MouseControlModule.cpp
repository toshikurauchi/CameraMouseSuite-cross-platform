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

#include <stdexcept>

#include "MouseControlModule.h"
#include "Monitor.h"

namespace CMS {

MouseControlModule::MouseControlModule() :
    initialized(false),
    screenReference(MonitorFactory::newMonitor()->getResolution()/2),
    gain(6, 6), // TODO Get value from GUI
    mouse(MouseFactory::newMouse())
{
}

MouseControlModule::~MouseControlModule()
{
    delete(mouse);
}

void MouseControlModule::setFeatureReference(Point featureReference)
{
    this->featureReference = featureReference;
    initialized = true;
}

bool MouseControlModule::isInitialized()
{
    return initialized;
}

void MouseControlModule::update(Point featurePosition)
{
    if (!initialized)
        throw std::logic_error("No reference feature position");

    Point displacement = (featurePosition - featureReference).elMult(gain);
    mouse->move(screenReference + displacement);
}

} // namespace CMS
