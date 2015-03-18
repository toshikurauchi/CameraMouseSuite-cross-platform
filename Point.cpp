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

#include "Point.h"

namespace CMS {

Point::Point() : isEmpty(true)
{
}

Point::Point(double x, double y) : isEmpty(false), x(x), y(y)
{
}

Point::Point(cv::Point2f &cvPoint) : isEmpty(false), x(cvPoint.x), y(cvPoint.y)
{
}

bool Point::empty()
{
    return isEmpty;
}

double Point::X()
{
    return x;
}

double Point::Y()
{
    return y;
}

cv::Point2f Point::asCVPoint()
{
    return cv::Point2f((float)x, (float)y);
}

} // namespace CMS