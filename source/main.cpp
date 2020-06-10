#include <iostream>
#include <switch.h>
#include <fstream>
#include <filesystem>

#include "fs.h"

#define VERSION "0.0.1"

#define BOOTLOADERPATH "bootloader"
#define ALTBOOTLOADERPATH "_bootlader"

#define KIPSPATH "atmosphere/kips"   
#define ALTKIPSPATH "_kips"

//extern FsFileSystem *fs;

namespace fs = std::filesystem;

void initServices(){
    consoleInit(NULL);
    //ncmInitialize();
    nsInitialize();
    //socketInitializeDefault();
}

void exitServices(){
    //socketExit();
    nsExit();
    //ncmExit();
    consoleExit(NULL);
}

void viewHeader() {
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << "\033[1;31m" << "Cling Wrap v" << VERSION << " by  Acta" << "\033[0m" <<std::endl;
    std::cout << "\033[31m" << "================================================================================" << "\033[0m" << std::endl;
    std::cout << std::endl;
}

void viewMain() {
    viewHeader();

    std::cout << "Press [A] to rename files to be able to launch tinfoil" << std::endl;
    std::cout << "Press [B] to rename files to be able to launch hekate" << std::endl;

    std::cout << "Press [+] to quit" << std::endl << std::endl;

    consoleUpdate(NULL);
}

int main(int argc, char* argv[]) {
    initServices();
    viewMain();
    bool renameBootloader = false;
    bool renameKips = false;

    while (appletMainLoop()) {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        fs::path p = "switch/Tinfoil-Workaround";

        if (kDown & KEY_A) {
            if(!fs::exists(p/BOOTLOADERPATH) && !fs::exists(p/ALTBOOTLOADERPATH)) {
                std::cout << "bootloader path not found" << std::endl;
                renameBootloader = false;
                fs::create_directories(p/BOOTLOADERPATH);
            }

            if(!fs::exists(p/KIPSPATH) && !fs::exists(p/ALTKIPSPATH)) {
                std::cout << "kips path not found" << std::endl;
                renameKips = false;
                fs::create_directories(p/KIPSPATH);
            }

            if(fs::exists(p/ALTBOOTLOADERPATH) && !fs::exists(p/BOOTLOADERPATH)) {
                std::cout << "bootloader already renamed for tinfoil" << std::endl;
                renameBootloader = false;
            }

            if(!fs::exists(p/ALTBOOTLOADERPATH) && fs::exists(p/BOOTLOADERPATH)) {
                std::cout << "bootloader not yet renamed for tinfoil" << std::endl;
                renameBootloader = true;
            }

            if(fs::exists(p/ALTKIPSPATH) && !fs::exists(p/KIPSPATH)) {
                std::cout << "kips already renamed for tinfoil" << std::endl;
                renameKips = false;
            }

            if(!fs::exists(p/ALTKIPSPATH) && fs::exists(p/KIPSPATH)) {
                std::cout << "kips not yet renamed for tinfoil" << std::endl;
                renameKips = true;
            }

            if(renameBootloader) {
                fs::rename(p/BOOTLOADERPATH, p/ALTBOOTLOADERPATH);
                std::cout << "Successfully renamed bootloader directory." << std::endl;
            }

            if(renameKips) {
                fs::rename(p/KIPSPATH, p/ALTKIPSPATH);
                std::cout << "Successfully renamed kips directory." << std::endl;
            }
            
            if(!renameKips && !renameBootloader) {
                std::cout << "No directories need to be renamed for tinfoil" << std::endl;
            }
        }

        if (kDown & KEY_B) {
            if(!fs::exists(p/BOOTLOADERPATH) && !fs::exists(p/ALTBOOTLOADERPATH)) {
                std::cout << "bootloader path not found" << std::endl;
                renameBootloader = false;
                fs::create_directories(p/BOOTLOADERPATH);
            }

            if(!fs::exists(p/KIPSPATH) && !fs::exists(p/ALTKIPSPATH)) {
                std::cout << "kips path not found" << std::endl;
                renameKips = false;
                fs::create_directories(p/KIPSPATH);
            }

            if(!fs::exists(p/ALTBOOTLOADERPATH) && fs::exists(p/BOOTLOADERPATH)) {
                std::cout << "bootloader already renamed for hekate" << std::endl;
                renameBootloader = false;
            }

            if(fs::exists(p/ALTBOOTLOADERPATH) && !fs::exists(p/BOOTLOADERPATH)) {
                std::cout << "bootloader not yet renamed for hekate" << std::endl;
                renameBootloader = true;
            }

            if(!fs::exists(p/ALTKIPSPATH) && fs::exists(p/KIPSPATH)) {
                std::cout << "kips already renamed for hekate" << std::endl;
                renameKips = false;
            }

            if(fs::exists(p/ALTKIPSPATH) && !fs::exists(p/KIPSPATH)) {
                std::cout << "kips not yet renamed for hekate" << std::endl;
                renameKips = true;
            }

            if(renameBootloader) {
                fs::rename(p/ALTBOOTLOADERPATH, p/BOOTLOADERPATH);
                std::cout << "Successfully renamed bootloader directory." << std::endl;
            }

            if(renameKips) {
                fs::rename(p/ALTKIPSPATH, p/KIPSPATH);
                std::cout << "Successfully renamed kips directory." << std::endl;
            }
            
            if(!renameKips && !renameBootloader) {
                std::cout << "No directories need to be renamed for hekate" << std::endl;
            }
        }

        if (kDown & KEY_PLUS)
            break;
    }

    exitServices();
    return 0;
}