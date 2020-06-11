#include <iostream>
#include <switch.h>
#include <fstream>

#include <string.h>
#include <chrono>
#include <ctime>
#include <sstream>

//#include <fs.h>
//#include <common.h>

#define VERSION "1.0.0"

#define LOGFILE "/switch/Cling-Wrap/log.txt"

#define BOOTLOADERPATH "/bootloader"
#define ALTBOOTLOADERPATH "/_bootloader"

#define KIPSPATH "/atmosphere/kips"   
#define ALTKIPSPATH "/atmosphere/_kips"

//namespace fs = std::filesystem;

FsFileSystem *fs;
FsFileSystem devices[4];

void initService();
void exitServices();
void viewHeader();
void viewMain();
void writeToLog(std::string msg);
bool FS_DirExists(FsFileSystem *fs, const char *path);
Result FS_RenameDir(FsFileSystem *fs, const char *old_dirname, const char *new_dirname);
bool tinfoilReady();
void clearConsole();


void initServices(){
    consoleInit(NULL);
    nsInitialize();
    writeToLog("ns Initialised");
    devices[0] = *fsdevGetDeviceFileSystem("sdmc");
    writeToLog("devices[0] set to sdmc");
	fs = &devices[0];
    writeToLog("fs set to devices[0]");
}

void exitServices(){
    nsExit();
    consoleExit(NULL);
}

void viewHeader() {
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "\033[1;31m" << "Cling Wrap v" << VERSION << " by  Acta" << "\033[0m" <<std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << std::endl;
    writeToLog("Header output");
}

void viewMain() {
    viewHeader();

    if(tinfoilReady()) {
        std::cout << "Current status: " << "\033[0;32m" << "Tinfoil Ready" << "\033[0m" << std::endl;
    } else {
        std::cout << "Current status: " << "\033[31m" << "Not Tinfoil Ready" << "\033[0m" << std::endl;
    }
    
    std::cout << "Press [A] to rename files" << std::endl;

    std::cout << "Press [+] to quit" << std::endl << std::endl;

    consoleUpdate(NULL);
    writeToLog("Body output");
}

void writeToLog(std::string msg) {
    std::ofstream logFile;
    logFile.open(LOGFILE, std::ofstream::out | std::ofstream::app);

    const auto p1 = std::chrono::system_clock::now();
    std::time_t today_time = std::chrono::system_clock::to_time_t(p1);

    if(logFile.is_open()) {
        logFile << msg << " - " << std::ctime(&today_time);
    }
    logFile.close();
}

/**
 * FS_DirExists
 * Checks if the given path exists as a directory
**/
bool FS_DirExists(FsFileSystem *fs, const char *path) {
	FsDir dir;
    writeToLog("Initialised dir");

	char temp_path[FS_MAX_PATH];
    writeToLog("Initialised temp_path");
	snprintf(temp_path, FS_MAX_PATH, path);
    writeToLog("Wrote to temp_path");

	if (R_SUCCEEDED(fsFsOpenDirectory(fs, temp_path, FsDirOpenMode_ReadDirs, &dir))) {
		fsDirClose(&dir);
		return true;
	}

	return false;
}

/**
 * FS_RenameDir
 * Renames the old_dirname to new_dirname
**/
Result FS_RenameDir(FsFileSystem *fs, const char *old_dirname, const char *new_dirname) {
	Result ret = 0;

	char temp_path_old[FS_MAX_PATH], temp_path_new[FS_MAX_PATH];
	snprintf(temp_path_old, FS_MAX_PATH, old_dirname);
	snprintf(temp_path_new, FS_MAX_PATH, new_dirname);

	if (R_FAILED(ret = fsFsRenameDirectory(fs, temp_path_old, temp_path_new))) {
        std::stringstream err;
        err << "fsFsRenameDirectory(" << temp_path_old << ", " << temp_path_new << ") failed: 0x" << ret;
		writeToLog(err.str());
        std::cout << "Renaming diretories has failed." << std::endl;
		return ret;
	}

	return 0;
}

/**
 * tinfoilReady
 * Checks if the device is tinfoil ready by checking the folders tinfoil searches for
**/
bool tinfoilReady() {
    writeToLog("Checking if /bootloader or /_bootloader exists.");
    if(FS_DirExists(fs, BOOTLOADERPATH)) {
        writeToLog("/bootloader path exists");
        return false;
    } else if (FS_DirExists(fs, ALTBOOTLOADERPATH)) {
        writeToLog("/_bootloader path exists");
        return true;
    } else {
        std::string err = "Error: No Bootloader folder or alternative folder found... It is not safe to proceed";
        std::cout << err << std::endl;
        writeToLog(err);
        exitServices();
        return 0;
    }
    return false;
}

void clearConsole() {
    consoleClear();
    consoleUpdate(NULL);
}

int main(int argc, char* argv[]) {
    // Initialise services
    initServices();

    // View the main console output
    viewMain();

    // char p[500] = "/switch/Cling-Wrap/";

    // Create char arrays for the old and new paths of files that need to be renamed
    char oldPath[500], newPath[500];

    while (appletMainLoop()) {
        hidScanInput();
        // Reset the old and new path char arrays
        memset(oldPath, 0, 500);
        memset(newPath, 0, 500);
        //strcat(oldPath, p);
        //strcat(newPath, p);
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_A) {
            
            // Set the old and new path's according to the devices tinfoilReady status
            if(tinfoilReady()) {
                writeToLog("Determined that the folder is currently /_bootloader and therefore ready for tinfoil use.");
                strcat(oldPath, ALTBOOTLOADERPATH);
                strcat(newPath, BOOTLOADERPATH);
                writeToLog("Prepared old and new path strings.");
            } else {
                writeToLog("Determined that the folder is currently /bootloader and therefore not ready for tinfoil use.");
                strcat(oldPath, BOOTLOADERPATH);
                strcat(newPath, ALTBOOTLOADERPATH);
                writeToLog("Prepared old and new path strings.");
            }
            
            // Rename the directories
            if(FS_RenameDir(fs, oldPath, newPath) == 0) {
                writeToLog("Renaming successful.");
                // Reload the console to show the new status
                clearConsole();
                viewMain();

            } else {
                std::cout << "An error occured during renaming. Check the log file for details: /switch/Cling-Wrap/log.txt" << std::endl;
            }
            consoleUpdate(NULL);
        }

        if (kDown & KEY_PLUS)
            break;
    }

    exitServices();
    return 0;
}