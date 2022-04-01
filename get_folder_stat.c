#include <get_folder_stat.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

long get_folder_stat(char* path, struct folder_stat *stat) {
	long sysvalue;
	sysvalue = syscall(548, path, stat);
	if(sysvalue) return sysvalue;
	printf("Student ID: %lu\n", stat->studentID); 
	return sysvalue;
}
