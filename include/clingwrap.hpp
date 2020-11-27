#pragma once

#include <switch.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <list>
#include <ctime>
#include <chrono>

#include "fs.hpp"

extern FsFileSystem *fs;
extern FsFileSystem devices[4];

namespace nxfs = std::filesystem;

#define LOGFILE "/switch/Cling-Wrap/log.txt"
#define VERSION "1.0.1"

// Status Descriptions and unicode glyphs
constexpr const char *const descriptions[4] = {
    [0] = "Unwrapped",
    [1] = "Wrapped",
    [2] = "Both are Present",
    [3] = "Neither are Present",    
};

// Status enum
// These correspond to descriptions
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
        [1] = "/_b0otloader",
    },
    [1] = {
        [0] = "/atmosphere/kips",
        [1] = "/atmosphere/_k1ps"
    },
};

// Directory struct
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


extern std::list<directory> directoryItems;
void renameAll(status s);
void viewHeader();
void viewMain();
void writeToLog(std::string msg);
status tinfoilReady();
void clearConsole();
status getStatus(const directory &dir);