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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QAbstractVideoSurface>

#include "Settings.h"

namespace Ui {
class MainWindow;
}

namespace CMS {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateSelectedCamera(QAction *action);
    void displayCameraError();
    void setCamera(const QCameraInfo &cameraInfo);
    void updateDwellSpinBox(int dwellMillis);
    void updateDwellSlider(double dwellSecs);
    void horizontalGainChanged(int horizontalGain);
    void verticalGainChanged(int verticalGain);
    void lockGainClicked(bool lock);

private:
    Ui::MainWindow *ui;
    QCamera *camera;
    QAbstractVideoSurface *videoManagerSurface;
    Settings settings;

    void setupCameraWidgets();
    void setupSettingsWidgets();
};

} // namespace CMS

#endif // MAINWINDOW_H
