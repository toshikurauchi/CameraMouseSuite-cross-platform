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

MouseControlModule::MouseControlModule(Settings &settings) :
    settings(settings),
    mouse(MouseFactory::newMouse()),
    keyboard(KeyboardFactory::newKeyboard()),
    initialized(false),
    screenReference(settings.getScreenResolution()/2),
    resetReference(true),
    controlling(false),
    prevLoopClicked(false)
{
}

MouseControlModule::~MouseControlModule()
{
    delete mouse;
    delete keyboard;
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
    while (keyboard->hasNextEvent())
    {
        KeyEvent event = keyboard->nextEvent();
        if (event.getKey() == KEY_CONTROL && event.getState() == KEY_STATE_DOWN)
        {
            controlling = !controlling;
            if (controlling)
            {
                resetReference = true;
            }
        }
    }

    if (controlling && resetReference)
    {
        setFeatureReference(featurePosition);
        resetReference = false;
    }

    if (!initialized || !controlling)
    {
        time.start();
        return;
    }

    // Move mouse
    Point displacement = (featurePosition - featureReference).elMult(settings.getGain());
    if (settings.getReverseHorizontal())
    {
        displacement.setX(-displacement.X());
    }
    Point pointerPos = screenReference + displacement;
    double damping = settings.getDamping();
    if (!prevPointer.empty())
    {
        pointerPos = pointerPos * damping + prevPointer * (1 - damping);
    }
    prevPointer = pointerPos;
    mouse->move(pointerPos);

    // Check if should click
    // TODO Should it be in another thread?
    if (settings.isClickingEnabled())
    {
        int elapsedTime = time.elapsed();
        int dwellTime = settings.getDwellTimeMillis();

        if (!withinRadius(dwellReference, pointerPos, settings.getDwellRadius()))
        {
            dwellReference = pointerPos;
            time.restart();
            prevLoopClicked = false;
        }

        if (elapsedTime >= dwellTime && !prevLoopClicked)
        {
            mouse->click();
            // TODO play sound
            prevLoopClicked = true;
        }
        else if (elapsedTime >= dwellTime + 1000 && prevLoopClicked)
        {
            time.restart();
            prevLoopClicked = false;
        }
    }
    else
    {
        time.start();
    }
}

void MouseControlModule::restart()
{
    resetReference = true;
}

bool MouseControlModule::withinRadius(Point center, Point point, double radius)
{
    return (point - center) * (point - center) <= radius * radius;
}

} // namespace CMS
