#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <libgen.h>
#include <unistd.h>
#include <time.h>

#define SIZE 200

struct folder_info {
    char name[128];
    mode_t permission;
    uid_t uid;
    gid_t gid;
    double size;
    time_t atime;
};

struct folder_stat {
    long studentID;
    struct folder_info folder;
    struct folder_info parent_folder;
    struct folder_info last_access_child_folder;
};

int main() {
	long r;
	long stat[200];
	char *path = "/home/nam/Desktop/asg";
	r = syscall(548, path, &stat);
	printf("Student ID: %ld\n", stat[0]);
	return 0;
}
