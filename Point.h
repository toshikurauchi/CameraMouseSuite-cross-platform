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

#ifndef CMS_POINT_H
#define CMS_POINT_H

#include <opencv/cv.h>
#include <QSize>

namespace CMS {

// TODO Maybe we should call it Vector? Should we have another class Size (just as Point, but with accessors width() and height())?
class Point
{
public:
    Point();
    Point(double x, double y);
    Point(cv::Point2f &cvPoint);
    Point(QSize qSize);
    bool empty();
    double X();
    double Y();
    void setX(double x);
    void setY(double y);
    cv::Point2f asCVPoint();
    cv::Point asCVIntPoint();
    Point operator+(Point other);
    Point operator-(Point other);
    Point operator*(double scalar);
    double operator*(Point other); // dot product
    Point operator/(double scalar);
    Point elMult(Point other); // elementwise multiplication

private:
    bool isEmpty;
    double x, y;
};

} // namespace CMS

#endif // CMS_POINT_H
