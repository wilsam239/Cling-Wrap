#include <iostream>
#include <switch.h>
#include <fstream>
#include <filesystem>
#include <list>

#include <string.h>
#include <chrono>
#include <ctime>
#include <sstream>

#define VERSION "1.0.1"

#define LOGFILE "/switch/Cling-Wrap/log.txt"

namespace nxfs = std::filesystem;

FsFileSystem *fs;
FsFileSystem devices[4];

struct directory {
    std::string dirName;
    nxfs::path dirPath;
    nxfs::path altPath;

    std::string getName() {
        return dirName;
    }
    const std::string getPath() {
        return dirPath;
    }
    std::string getAltPath() {
        return altPath;
    }
};


std::list<directory> directoryItems;

// Status Descriptions and unicode glyphs
constexpr const char *const descriptions[4] = {
    [0] = "Unwrapped",
    [1] = "Wrapped",
    [2] = "Both are Present",
    [3] = "Neither are Present",    
};

enum status {
    unwrapped = 0,
    wrapped = 1,
    bothPresent = 2,
    neitherPresent = 3
};

// Paths for files tinfoil looks for and their respective alternate paths for Cling Wrap
constexpr const char *const paths[2][2] = {
    [0] = {
        [0] = "/bootloader",
        [1] = "/_bootloader",
    },
    [1] = {
        [0] = "/atmosphere/kips",
        [1] = "/atmosphere/_kips"
    },
};

status startupStatus;

void initService();
void exitServices();
void renameAll(status s);
void viewHeader();
void viewMain();
void writeToLog(std::string msg);
bool FS_DirExists(FsFileSystem *fs, const char *path);
Result FS_RenameDir(FsFileSystem *fs, const char *old_dirname, const char *new_dirname);
status tinfoilReady();
void clearConsole();
status getStatus(const directory &dir);

void initServices(){
    // Reset the log file
    std::ofstream ofs;
    ofs.open(LOGFILE, std::ofstream::out | std::ofstream::trunc);
    ofs.close();

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

    startupStatus = tinfoilReady();
}

void exitServices(){
    nsExit();
    consoleExit(NULL);
}

void viewHeader() {
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "\033[1;31m" << "Cling Wrap v" << VERSION << " by  Acta" << "\033[0m" <<std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "Wrapped = Files are ready for Tinfoil" << std::endl;
    std::cout << "Unwrapped = Files are ready for Hekate/Kosmos" << std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;

    writeToLog("Header output");
}

void viewMain() {
    viewHeader();
    status ready = tinfoilReady();
    std::cout << "Current status: " << "\033[0;32m" << descriptions[ready] << "\033[0m" << std::endl;
        
    if (ready == status::wrapped || ready == status::unwrapped) {
        std::cout << "Press [A] to rename files" << std::endl;
    } else {
        std::cout << "It is not safe to continue..." << std::endl;
    }

    std::cout << "Press [+] to quit" << std::endl << std::endl;

    consoleUpdate(NULL);
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
status tinfoilReady() {
    
    status readyPaths[directoryItems.size()];
    std::stringstream logMsg;
    int i = 0;

    // Loop through the list of possible paths
    if(directoryItems.size() == 0) {
        return status::neitherPresent;
    } else {
        for (const auto &dir : directoryItems) {
            logMsg << "Checking " << dir.dirName;
            writeToLog(logMsg.str());                
            readyPaths[i] = getStatus(dir);
            i++;
        }
    }
    
    for(int j = 0; j < (int) (sizeof(readyPaths)/sizeof(readyPaths[0])); j++) {
        if(readyPaths[j] == status::wrapped) {
            // If wrapped, do nothing
        } else {
            return readyPaths[j];
        }
    }
    // If the end of the loop is reached, it must be entirely wrapped
    return status::wrapped;
}

status getStatus(const directory &dir) {
    const char *path = dir.dirPath.c_str();
    const char *alt = dir.altPath.c_str();

    if(FS_DirExists(fs, path) && FS_DirExists(fs, alt)) {
        // Both directories are found
        return status::bothPresent;
    } else {
        if(FS_DirExists(fs, path)) {
            return status::unwrapped;
        } else if(FS_DirExists(fs, alt)) {
            return status::wrapped;
        } else {
            // No directories are found
            return status::neitherPresent;
        }
    }    
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

        if (startupStatus == status::wrapped || startupStatus == status::unwrapped) {
            if (kDown & KEY_A) {                        
                // Set the old and new path's according to the devices tinfoilReady status
                if(tinfoilReady() == status::wrapped) {
                    writeToLog("Determined that the folders are currently wrapped and therefore ready for tinfoil use.");
                    renameAll(status::unwrapped);
                } else if(tinfoilReady() == status::unwrapped) {
                    writeToLog("Determined that the folders are currently unwrapped and therefore not ready for tinfoil use.");                            
                    renameAll(status::wrapped);
                }
                // Reload the console to show the new status
                clearConsole();
                viewMain();
                consoleUpdate(NULL);
            }
        }
        

        if (kDown & KEY_PLUS)
            break;
    }

    exitServices();
    return 0;
}

// Rename a directory
void rename(const int pathIndex) {
    // Get the path variables
    const char *path = paths[pathIndex][0];
    const char *alt = paths[pathIndex][1];
    
    if(FS_DirExists(fs, path) && FS_DirExists(fs, alt)) {
        // Both directories are found
    } else {
        if(FS_DirExists(fs, path)) {
            if(FS_RenameDir(fs, path, alt) == 0) {
                // Success
            }
        } else if(FS_DirExists(fs, alt)) {
            if(FS_RenameDir(fs, alt, path) == 0) {
                // Success
            }
        } else {
            // No directories are found
        }
    }    
}

// Rename all folders so that they are in the s status
void renameAll(status s) {
    // wrapped = alt path present
    // unwrapped = path present

    // Loop through the list of possible paths
    for (const auto &dir : directoryItems) {
        // Get the path variables
        const char *path = dir.dirPath.c_str();
        const char *alt = dir.altPath.c_str();
        if(s == status::wrapped) {
            if(FS_DirExists(fs, path) && !FS_DirExists(fs, alt)) {
                if(FS_RenameDir(fs, path, alt) == 0) {
                    // Success
                }
            }
        } else if (s == status::unwrapped) {
            if(FS_DirExists(fs, alt) && !FS_DirExists(fs, path)) {
                if(FS_RenameDir(fs, alt, path) == 0) {
                    // Success
                }
            }
        }
    }
}