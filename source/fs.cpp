#include "fs.hpp"

/**
 * FS_DirExists
 * Checks if the given path exists as a directory
**/
bool FS_DirExists(FsFileSystem *fs, const char *path) {
	FsDir dir;

	char temp_path[FS_MAX_PATH];
	snprintf(temp_path, FS_MAX_PATH, path);

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
		return ret;
	}

	return 0;
}