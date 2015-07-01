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

#ifndef MOUSECONTROLMODULE_H
#define MOUSECONTROLMODULE_H

#include "Point.h"
#include "Mouse.h"

namespace CMS {

class MouseControlModule
{
public:
    MouseControlModule();
    ~MouseControlModule();
    void setFeatureReference(Point featureReference);
    bool isInitialized();
    void update(Point featurePosition);

private:
    bool initialized;
    Point screenReference;
    Point featureReference;
    Point gain;
    IMouse *mouse;
};

} // namespace CMS

#endif // MOUSECONTROLMODULE_H