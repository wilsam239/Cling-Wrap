#include "clingwrap.hpp"
#include "fs.hpp"

// Global filesystem variables
FsFileSystem *fs;
FsFileSystem devices[4];

// Global list of directory items
std::list<directory> directoryItems;

// Global startup status
status startupStatus;

// Function declarations
void initService();
void exitServices();

void initServices(){
    // Reset the log file
    std::ofstream ofs;
    ofs.open(LOGFILE, std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    // Initialise switch stuff
    consoleInit(NULL);
    nsInitialize();
    writeToLog("ns Initialised");
    devices[0] = *fsdevGetDeviceFileSystem("sdmc");
    writeToLog("devices[0] set to sdmc");
	fs = &devices[0];
    writeToLog("fs set to devices[0]");

    // Loop through the list of possible paths
    for(int i = 0; i < (int) (sizeof(paths)/sizeof(paths[0])); i++) {
        // Declare the mainPath variable (the unaltered path)
        // Also declare the alt path variable
        const char *mainPath = paths[i][0];
        const char *alt = paths[i][1];
        // If either directory is present, continue
        if(FS_DirExists(fs, mainPath) || FS_DirExists(fs, alt)) {
            // Create a new directory for the current directory that is found
            directory currentDir = {
                .dirName = mainPath + 1,
                .dirPath = mainPath,
                .altPath = alt
            };

            // Add the current directory to the list of directory items
            directoryItems.push_back(std::move(currentDir));
        }
    }

    // Set the startup status (determines whether anything can be renamed to begin with)
    startupStatus = tinfoilReady();
}

void exitServices(){
    nsExit();
    consoleExit(NULL);
}

int main(int argc, char* argv[]) {
    // Initialise services
    initServices();
    status currentValue;
    std::stringstream logMsg;

    // View the main console output
    viewMain();

    while (appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // If the startup status is valid, listen for A button
        if (startupStatus == status::wrapped || startupStatus == status::unwrapped) {
            if (kDown & KEY_A) {                     
                // Get the current status   
                currentValue = tinfoilReady();
                // Log the status
                logMsg << "Current status: " << currentValue;
                writeToLog(logMsg.str());
                logMsg.str(std::string());

                if(currentValue == status::wrapped) {
                    // If wrapped
                    renameAll(status::unwrapped);
                } else if(currentValue == status::unwrapped) {
                    // If unwrapped                           
                    renameAll(status::wrapped);
                }

                // Reload the console to show the new status
                clearConsole();
                viewMain();
                consoleUpdate(NULL);
            }
        }
        
        // If the plus key is pressed
        if (kDown & KEY_PLUS)
            break;
    }

    exitServices();
    return 0;
}

