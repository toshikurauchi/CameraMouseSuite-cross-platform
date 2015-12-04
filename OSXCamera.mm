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

#include "Camera.h"

#ifdef Q_OS_MAC

#import <AVFoundation/AVFoundation.h>

namespace CMS {

std::vector<Camera*> OSXCamera::getCameraList()
{
    int idx = 0;
    std::vector<Camera*> cameras;
    // list available devices
    NSArray *devices = [AVCaptureDevice devices];
    for (AVCaptureDevice *device in devices) {
        if ([device hasMediaType:AVMediaTypeVideo]) {
            cameras.push_back(new Camera([[device localizedName] UTF8String], idx));
            idx++;
        }
    }
    return cameras;
}

} // namespace CMS

#endif
