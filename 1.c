#include <get_folder_stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

void print_folder_info(struct folder_info folder) {
	printf("+ Permission: %o\n+ Uid: %ld\n+ Gid: %ld\n+ Size: %f\n+ Access time: %ld\n", 
		folder.permission, (long)folder.uid, (long)folder.gid, (double)folder.size, (long)folder.atime);
}

int main() {
	struct folder_stat stat;
	char* path = "/home/nam/Desktop";
	if(get_folder_stat(path, &stat) == 0) {
		printf("Folder name: %s\n", stat.folder.name);
		print_folder_info(stat.folder);
		printf("Parent name: %s\n", stat.parent_folder.name);
		print_folder_info(stat.parent_folder);
		if(stat.last_access_child_folder.atime >= 0) {
			printf("Last Access Child name: %s\n", stat.last_access_child_folder.name);	
			print_folder_info(stat.last_access_child_folder);
		}
		else printf("No child folder has been found!\n");
	}
	else printf("There is no directory from given path: %s\n", path);
}
