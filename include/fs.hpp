#pragma once

#include <filesystem>
#include <switch.h>

bool FS_DirExists(FsFileSystem *fs, const char *path);
Result FS_RenameDir(FsFileSystem *fs, const char *old_dirname, const char *new_dirname);