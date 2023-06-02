#ifndef SFS_API_H
#define SFS_API_H

// 初始化文件系统
void sfs_system_init();
void sfs_system_close();
void sfs_system_info();

// 目录操作
void sfs_ls();

// 文件操作，返回-1表示文件不存在
int sfs_open(char *name);
int sfs_close(char* name);

int sfs_write(char* name, char *buf, int length);
int sfs_read(char* name, char *buf, int length);

int min(int a, int b);
#endif
