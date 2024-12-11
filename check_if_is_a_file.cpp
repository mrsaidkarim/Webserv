#include <sys/stat.h>
#include <stdio.h>

void checkFileType(const char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) == 0) {
		if (S_ISREG(statbuf.st_mode)) {
			printf("%s is a regular file\n", path);
		} else if (S_ISDIR(statbuf.st_mode)) {
			printf("%s is a directory\n", path);
	} else {
		perror("stat");
	}
}

int main() {
	checkFileType("/Users/zelabbas/Desktop/WebServer/Test/test.cpp");
	checkFileType("/Users/zelabbas/Desktop/WebServer/Test");
	return 0;
}
