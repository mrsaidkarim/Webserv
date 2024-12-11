#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


// #define std::cout cout
bool	deleteFile(const char *filepath) {
	if (unlink(filepath) == 0) {
		std::cout << "File deleted successfully: " << filepath << "\n";
		return (true);
	} else {
		std::cerr << "Error deleting file: " << "Permission denied!" << "\n";
		return (false);
	}
}

void checkFileType(const char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) == 0) {
		if (S_ISREG(statbuf.st_mode)) {
			std::cout << path << " is a file\n";
			if (access(path, W_OK) != 0)
				std::cout << "You don't have the write permission for: " << path << "\n";
			else {
				if (!deleteFile(path))
					std::cout << "error : can't delete the file!\n";
			}
		} else if (S_ISDIR(statbuf.st_mode)) {
			std::cout << path << " is a directory\n";
		} else {
			std::cout << path << " is neither a file nor a directory\n";
		}
	} else {
		std::cout << "error the file does not exist\n";
	}
}

// check the loction allwe delete 

int main() {
	checkFileType("/Users/zelabbas/WebServ/dir/test_file");
	checkFileType("/Users/zelabbas/Desktop/WebServer/Test");
	return 0;
}
