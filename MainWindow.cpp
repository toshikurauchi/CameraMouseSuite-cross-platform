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

#include <QCameraInfo>
#include <QMessageBox>

#include "MainWindow.h"
#include "ui_mainWindow.h"
#include "VideoManagerSurface.h"

Q_DECLARE_METATYPE(QCameraInfo)

namespace CMS {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("CameraMouseSuite"));
    setupCameraWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCameraWidgets()
{
    // Create video manager
    videoManagerSurface = new VideoManagerSurface(ui->frameLabel, this);

    // Create device selection menu
    QActionGroup *cameraGroup = new QActionGroup(this);
    cameraGroup->setExclusive(true);
    for (const QCameraInfo &cameraInfo: QCameraInfo::availableCameras()) {
        QAction *cameraAction = new QAction(cameraInfo.description(), cameraGroup);
        cameraAction->setCheckable(true);
        cameraAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera())
            cameraAction->setChecked(true);

        ui->menuDevices->addAction(cameraAction);
    }

    connect(cameraGroup, SIGNAL(triggered(QAction*)), SLOT(updateSelectedCamera(QAction*)));

    setCamera(QCameraInfo::defaultCamera());
}

void MainWindow::updateSelectedCamera(QAction *action)
{
    setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

void MainWindow::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera error"), camera->errorString());
}

void MainWindow::setCamera(const QCameraInfo &cameraInfo)
{
    delete camera;

    camera = new QCamera(cameraInfo);

    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));
    camera->setViewfinder(videoManagerSurface);

    camera->start();
}

} // namespace CMS
