# Cling Wrap
This app aims to provide a workaround for using Hekate and Tinfoil on the same Switch Console without having to remove Hekate.
## How It Works
Tinfoil looks through your switch's files for specific folders that are used by hekate. This app renames them temporarily so that you can start tinfoil without uninstalling Hekate.

After renaming, it is important to run the program again to rename the files so that hekate can boot them again.

Specifically, it renames the `bootloader` folder, and the `atmosphere/kips` folder.

In the event that the app crashes during use, be sure to rename the files manually by inserting your SD card into a computer.

## Credits
- KiteAffair for the name suggestion
- NX-Shell for the filesystem code

