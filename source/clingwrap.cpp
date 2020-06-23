#include "clingwrap.hpp"

void viewHeader() {
    // Header output
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "\033[1;31m" << "Cling Wrap v" << VERSION << " by  Acta" << "\033[0m" <<std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "Wrapped = Files are ready for Tinfoil" << std::endl;
    std::cout << "Unwrapped = Files are ready for Hekate/Kosmos" << std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
}

void viewMain() {
    // Main output
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
    // Write a string to a log file
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
 * tinfoilReady
 * Checks if the device is tinfoil ready by checking the folders tinfoil searches for
**/
status tinfoilReady() {
    // Array of status'
    status readyPaths[directoryItems.size()];
    // Log message
    std::stringstream logMsg;
    // Counter0
    int i = 0;

    // Loop through the list of found paths
    if(directoryItems.size() == 0) {
        return status::neitherPresent;
    } else {
        for (const auto &dir : directoryItems) {
            logMsg << "Checking " << dir.dirName;
            writeToLog(logMsg.str());
            // Check the status of the current directory (wrapped, unwrapped, both, neither)
            readyPaths[i] = getStatus(dir);
            i++;
        }
    }
    
    // Loop through the status'
    for(int j = 0; j < (int) (sizeof(readyPaths)/sizeof(readyPaths[0])); j++) {
        if(readyPaths[j] == status::wrapped) {
            // If wrapped, do nothing
        } else {
            // If not wrapped, return the status
            return readyPaths[j];
        }
    }

    // If the end of the loop is reached, it must be entirely wrapped
    return status::wrapped;
}

status getStatus(const directory &dir) {
    // Get the main path
    const char *path = dir.dirPath.c_str();
    // Get the alt path
    const char *alt = dir.altPath.c_str();

    // Check for main and/or alts
    if(FS_DirExists(fs, path) && FS_DirExists(fs, alt)) {
        // Both directories are found
        return status::bothPresent;
    } else {
        if(FS_DirExists(fs, path)) {
            // Main is found
            return status::unwrapped;
        } else if(FS_DirExists(fs, alt)) {
            // Alt is found
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

// Rename a directory
void rename(const int pathIndex) {
    
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