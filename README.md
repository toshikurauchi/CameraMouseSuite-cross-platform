# CameraMouseSuite - cross-platform version

## Configuring the environment

### Mac OS X

1. If [Xcode](https://developer.apple.com/xcode/) is not already installed, install it
1. [Install Qt Creator](https://www1.qt.io/download/) (with Qt 5.X). You can select a free, open source version. When installing it, select at least 1 version that is 5.1 or higher.
1. Install OpenCV (with Homebrew, for example: `brew install opencv`)
1. Install pkg-config (with Homebrew: `brew install pkg-config`)
1. Open QT Creator, then open the `.pro` file, `CameraMouseSuite-cross-platform.pro`.
1. Run the program by pressing cmd-R, or click on the `Build` menu, then click `Run`.

#### Troubleshooting

##### Qt Creator says you must install Xcode

Make sure you have Xcode installed first. If you don't, Qt Creator may go into a loop during install and the only way out is to kill the install process and begin again. If you've installed Xcode and are still getting the error, according to these two sources [one](https://stackoverflow.com/a/38678822/2555516) and [two](https://www.codeproject.com/Tips/740041/Installing-Qt-on-Mac-OS-X), open Xcode, go to Preferences, and change the Command Line Tools to point at the most recent version (`Preferences -> Locations`, or `Preferences -> Downloads -> Components`, depending on your version).

##### Permission issue when installing opencv or pkg-config with brew

If you get a permission error regarding `/usr/local/Frameworks`, copy and paste the steps into your terminal, as explained [here](https://github.com/Homebrew/homebrew-core/issues/19286):
1. `sudo mkdir /usr/local/Frameworks`
1. `sudo chown $(whoami):admin /usr/local/Frameworks`


### Windows

1. Install Qt Creator (with Qt 5.X)
1. Install OpenCV (the easiest way is to [use the pre-built libraries](http://docs.opencv.org/doc/tutorials/introduction/windows_install/windows_install.html)
1. Set the environment variable `OPENCV_DIR` as described [here](http://docs.opencv.org/doc/tutorials/introduction/windows_install/windows_install.html#windowssetpathandenviromentvariable)
1. Set the environment variable `OPENCV_INCLUDE` to the OpenCV `include` directory
