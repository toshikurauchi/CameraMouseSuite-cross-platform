#-------------------------------------------------
#                         Camera Mouse Suite
#  Copyright (C) 2015, Andrew Kurauchi
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CameraMouseSuite
TEMPLATE = app

unix {
    QT_CONFIG -= no-pkg-config
    CONFIG += c++11 link_pkgconfig

    mac {
      PKG_CONFIG = /usr/local/bin/pkg-config
    }

    PKGCONFIG += opencv
}

win32 {
    INCLUDEPATH += $$(OPENCV_INCLUDE)
    LIBS += $$(OPENCV_DIR)/lib/*.lib
}

SOURCES += *.cpp

HEADERS  += *.h \
    asmOpenCV.h

FORMS    += mainWindow.ui

OTHER_FILES += \
    README.md
