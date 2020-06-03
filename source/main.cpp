#include <iostream>
#include <switch.h>
#include <filesystem>

#define VERSION "0.0.1"

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
    std::cout << "\033[1;31m" << "Tinfoil Workaround v" << VERSION << " by  Acta" << "\033[0m" <<std::endl;
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

    while (appletMainLoop()) {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_A) {
            // rename files
        }

        if (kDown & KEY_B) {
            //restore files
        }

        if (kDown & KEY_PLUS)
            break;
    }

    exitServices();
    return 0;
}