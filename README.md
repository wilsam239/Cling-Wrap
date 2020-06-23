# Cling Wrap
<p align="center">
<img src = "https://user-images.githubusercontent.com/11286118/84357080-6e643c00-ac08-11ea-9435-19f116d0477e.png"\><br>
<img alt="GitHub All Releases" src="https://img.shields.io/github/downloads/wilsam239/Cling-Wrap/total">
</p>
A Nintendo Switch homebrew app to enable Tinfoil to be run on devices with hekate (without the custom fork) by renaming folders that Tinfoil looks for on your SD card so that it can be run.

**It is important to run the software before and after using Tinfoil, as not running it after using Tinfoil will result in Hekate failing to launch.**

## How It Works
Tinfoil looks through your switch's files for specific folders that are used by hekate. This app renames them temporarily so that you can start tinfoil without uninstalling Hekate.

After renaming, it is important to run the program again to rename the files so that hekate can boot them again.

Specifically, it renames the `bootloader` and `atmosphere/kips/` folder if they are present.

In the event that the app crashes during use, be sure to rename the files manually by inserting your SD card into a computer or by booting fusee primary and using goldleaf. Also be sure to upload your log.txt file (found in the Cling Wrap nro folder) as an issue on GitHub.

## How to install
Copy the `Cling-Wrap/` directory to `/switch/` in your sdcard

## Screenshots
![Ready](https://user-images.githubusercontent.com/11286118/84357078-6e643c00-ac08-11ea-8b82-67ccafbb5d8f.jpg)

![Not Ready](https://user-images.githubusercontent.com/11286118/84357073-6d330f00-ac08-11ea-9f66-f910660eef05.jpg)

## Thanks
- KiteAffair for the name suggestion
- NX-Shell for their filesystem code

