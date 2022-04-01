#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/namei.h>			/* For kern_path function */
#include <linux/sched.h>			/* For current definition */
#include <linux/types.h>			/* For mode_t, uid_t, gid_t*/
#include <linux/dcache.h>			/* For struct dentry */
#include <linux/fs.h>
#include <linux/path.h>				/* For struct path */
#include <linux/unistd.h>
#include <linux/time.h>				/* For time_t */
#include <linux/string.h>			/* For strcpy function */
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/list.h>				/* For list_for_each_entry */
#include <linux/stat.h>				/* For S_ISDIR definition */
#include <linux/fs_struct.h>		/* For get_fs_pwd function */

struct folder_info {
    char name[128];
    mode_t permission;
    uid_t uid;
    gid_t gid;
    long long size;
    time_t atime;
};

struct folder_stat {
    long studentID;
    struct folder_info folder;
    struct folder_info parent_folder;
    struct folder_info last_access_child_folder;
};

long long getMegabytes(long long kBytes) {
	// declaration
    long long temp = kBytes / 1000000;
    long long afterPoint = kBytes % 1000000;
    int count = 0;
    long long stack = 0;
    long long returnVal = 0;
    int i = 0;

    if (kBytes <= 0) return 0;
    if (kBytes < 1000000) {
		// if result < 1
        afterPoint = afterPoint << 43;
        afterPoint = afterPoint / 1000000;
        while(afterPoint != 0) {
            stack = stack << 1;
            stack = stack + (afterPoint & 1);
            afterPoint = afterPoint >> 1;
            count = count + 1;
        }
        stack = stack >> 1;
        returnVal = (long long)(1023 - 44 + count);
        for (i = 0; i < 52; i++) {
            returnVal = returnVal << 1;
            returnVal = returnVal + (stack & 1);
            stack = stack >> 1;
        }
    }
    else {
		// if result >= 1
        afterPoint = afterPoint << 43;
        afterPoint = afterPoint / 1000000;
        for (i = 0; i < 43; i++) {
            stack = stack << 1;
            stack = stack + (afterPoint & 1);
            afterPoint = afterPoint >> 1;
        }
        while (temp != 0) {
            stack = stack << 1;
            stack = stack + (1 & temp);
            temp = temp >> 1;
            count = count + 1;
        }
        stack = stack >> 1;
        returnVal = (long long)(count - 1 + 1023);
        for (i = 0; i < 52; i++) {
            returnVal = returnVal << 1;
            returnVal = returnVal + (stack & 1);
            stack = stack >> 1;
        }
    }
    return returnVal;
}

long long getSize(struct dentry* dir) {
	struct dentry *tmp_dir;
	long long size = 0;
	list_for_each_entry(tmp_dir, &dir->d_subdirs, d_child) {
		if(tmp_dir->d_inode != NULL) {
			if(S_ISDIR(tmp_dir->d_inode->i_mode)) {
				long long more = 0;
				more = getSize(tmp_dir);
				size = size + tmp_dir->d_inode->i_size + more;	
			}
			else {
				size = size + tmp_dir->d_inode->i_size;
			}
		}
	}
	return size;
}

SYSCALL_DEFINE2 (get_folder_stat, char *, path, struct folder_stat*, stat) {
	stat->studentID = 1910351;
	// Declare real_path for folder_info folder
	struct path real_path;
	/* 
	if path is NULL, real_path is asigned with current working folder 
	else real_path is asigned with the folder get from the given path
	*/
	int r = 0;	
	if(path != NULL) {
		// r = 0 show that the given path is valid
		r = kern_path(path, LOOKUP_FOLLOW, &real_path);
	}
	else {
		// get current working folder
		get_fs_pwd(current->fs, &real_path);	
	}
	if((int)r == 0) {
		// get the folder attribute.
		struct dentry *dir;
		dir = real_path.dentry;
		strcpy(stat->folder.name, (char*)dir->d_name.name);
		stat->folder.permission = dir->d_inode->i_mode;
		stat->folder.uid = dir->d_inode->i_uid.val;
		stat->folder.gid = dir->d_inode->i_gid.val;
		stat->folder.size = getMegabytes(getSize(dir));
		stat->folder.atime = dir->d_inode->i_atime.tv_sec;
		// get the parent_folder attribute.
		struct dentry *parent;
		parent = dir->d_parent;
		if(parent != NULL) {
			strcpy(stat->parent_folder.name, (char*)parent->d_name.name);
			stat->parent_folder.permission = parent->d_inode->i_mode;
			stat->parent_folder.uid = parent->d_inode->i_uid.val;
			stat->parent_folder.gid = parent->d_inode->i_gid.val;
			stat->parent_folder.size = getMegabytes(getSize(parent));
			stat->parent_folder.atime = parent->d_inode->i_atime.tv_sec;
		}
		else stat->parent_folder.atime = -1;
		// find the last access child folder
		struct dentry *tmp_dir;
		struct dentry *last_access_child_dir = NULL;
		list_for_each_entry(tmp_dir, &dir->d_subdirs, d_child) {
			if(tmp_dir->d_inode != NULL) {
				if(S_ISDIR(tmp_dir->d_inode->i_mode)) {
					if(last_access_child_dir == NULL)
						last_access_child_dir = tmp_dir;
					else {
						if((long)last_access_child_dir->d_inode->i_atime.tv_sec < (long)tmp_dir->d_inode->i_atime.tv_sec) {
							last_access_child_dir = tmp_dir;
						}
					}		
				}
			}
		}
		// get the last_access_child_folder attribute.
		if(last_access_child_dir != NULL) {
			strcpy(stat->last_access_child_folder.name, (char*)last_access_child_dir->d_name.name);
			stat->last_access_child_folder.permission = last_access_child_dir->d_inode->i_mode;
			stat->last_access_child_folder.uid = last_access_child_dir->d_inode->i_uid.val;
			stat->last_access_child_folder.gid = last_access_child_dir->d_inode->i_gid.val;
			stat->last_access_child_folder.size = getMegabytes(getSize(last_access_child_dir));
			stat->last_access_child_folder.atime = last_access_child_dir->d_inode->i_atime.tv_sec;
		}
		else stat->last_access_child_folder.atime = -1;
		printk("Student ID: %ld\n", stat->studentID);
		return 0;
	}
	return EINVAL;
}









