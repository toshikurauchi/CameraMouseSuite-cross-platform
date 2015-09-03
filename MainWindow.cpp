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
#include "CameraMouseController.h"
#include "TemplateTrackingModule.h"
#include "MouseControlModule.h"

Q_DECLARE_METATYPE(QCameraInfo)

namespace CMS {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    camera(0),
    settings(this)
{
    ui->setupUi(this);
    setWindowTitle(tr("CameraMouseSuite"));
    setupCameraWidgets();
    setupSettingsWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCameraWidgets()
{
    // Create video manager
    ITrackingModule *trackingModule = new TemplateTrackingModule(0.08); // TODO magic constants are not nice :(
    MouseControlModule *controlModule = new MouseControlModule(settings);
    CameraMouseController *controller = new CameraMouseController(settings, trackingModule, controlModule);
    videoManagerSurface = new VideoManagerSurface(settings, controller, ui->frameLabel, this);

    // Create device selection menu
    QActionGroup *cameraGroup = new QActionGroup(this);
    cameraGroup->setExclusive(true);
    foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
        QAction *cameraAction = new QAction(cameraInfo.description(), cameraGroup);
        cameraAction->setCheckable(true);
        cameraAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera())
            cameraAction->setChecked(true);

        ui->menuDevices->addAction(cameraAction);
    }

    connect(cameraGroup, SIGNAL(triggered(QAction*)), SLOT(updateSelectedCamera(QAction*)));

    setCamera(QCameraInfo::defaultCamera());
    if (!controller->isAutoDetectWorking()) ui->autoDetectNoseCheckBox->setVisible(false);
}

void MainWindow::setupSettingsWidgets()
{
    // Clicking
    connect(ui->enableClickingCheckBox, SIGNAL(toggled(bool)), ui->dwellSlider, SLOT(setEnabled(bool)));
    connect(ui->enableClickingCheckBox, SIGNAL(toggled(bool)), ui->dwellSpinBox, SLOT(setEnabled(bool)));
    connect(ui->enableClickingCheckBox, SIGNAL(toggled(bool)), &settings, SLOT(setEnableClicking(bool)));
    bool clickingEnabled = settings.isClickingEnabled();
    ui->enableClickingCheckBox->setChecked(clickingEnabled);
    ui->dwellSlider->setEnabled(clickingEnabled);
    ui->dwellSpinBox->setEnabled(clickingEnabled);

    // Dwell Time
    connect(ui->dwellSpinBox, SIGNAL(valueChanged(double)), &settings, SLOT(setDwellTime(double)));
    connect(ui->dwellSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateDwellSlider(double)));
    connect(ui->dwellSlider, SIGNAL(valueChanged(int)), this, SLOT(updateDwellSpinBox(int)));
    ui->dwellSpinBox->setValue(1.0);

    // Mouse Movement
    connect(ui->reverseHorizontalCheckBox, SIGNAL(toggled(bool)), &settings, SLOT(setReverseHorizontal(bool)));
    ui->reverseHorizontalCheckBox->setChecked(settings.getReverseHorizontal());

    // Gain
    connect(ui->horizontalGainSlider, SIGNAL(valueChanged(int)), &settings, SLOT(setHorizontalGain(int)));
    connect(ui->horizontalGainSlider, SIGNAL(valueChanged(int)), this, SLOT(horizontalGainChanged(int)));
    connect(ui->verticalGainSlider, SIGNAL(valueChanged(int)), &settings, SLOT(setVerticalGain(int)));
    connect(ui->verticalGainSlider, SIGNAL(valueChanged(int)), this, SLOT(verticalGainChanged(int)));
    connect(ui->lockGainButton, SIGNAL(toggled(bool)), this, SLOT(lockGainClicked(bool)));
    ui->lockGainButton->setChecked(true);
    settings.setHorizontalGain(ui->horizontalGainSlider->value());
    settings.setVerticalGain(ui->verticalGainSlider->value());

    // Smoothing
    connect(ui->smoothingCheckBox, SIGNAL(toggled(bool)), ui->smoothingSlider, SLOT(setEnabled(bool)));
    connect(ui->smoothingCheckBox, SIGNAL(toggled(bool)), &settings, SLOT(setEnableSmoothing(bool)));
    connect(ui->smoothingSlider, SIGNAL(valueChanged(int)), &settings, SLOT(setDampingPercent(int)));
    settings.setDampingPercent(ui->smoothingSlider->value());
    ui->smoothingCheckBox->setChecked(true);

    // Auto Detect Nose
    connect(ui->autoDetectNoseCheckBox, SIGNAL(toggled(bool)), &settings, SLOT(setAutoDetectNose(bool)));
    ui->autoDetectNoseCheckBox->setChecked(settings.isAutoDetectNoseEnabled());
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
    if (camera)
        delete camera;

    camera = new QCamera(cameraInfo);

    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));
    camera->setViewfinder(videoManagerSurface);

    camera->start();

    camera->setCaptureMode(QCamera::CaptureViewfinder);
    camera->searchAndLock();
}

void MainWindow::updateDwellSpinBox(int dwellMillis)
{
    double dwellTime = dwellMillis / 1000.0;
    ui->dwellSpinBox->setValue(dwellTime);
}

void MainWindow::updateDwellSlider(double dwellSecs)
{
    int dwellMillis = (int) (dwellSecs * 1000);
    ui->dwellSlider->setValue(dwellMillis);
}

void MainWindow::horizontalGainChanged(int horizontalGain)
{
    if (ui->lockGainButton->isChecked())
        ui->verticalGainSlider->setValue(horizontalGain);
}

void MainWindow::verticalGainChanged(int verticalGain)
{
    if (ui->lockGainButton->isChecked())
        ui->horizontalGainSlider->setValue(verticalGain);
}

void MainWindow::lockGainClicked(bool lock)
{
    if (lock)
        ui->verticalGainSlider->setValue(ui->horizontalGainSlider->value());
}

} // namespace CMS
