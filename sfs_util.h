#ifndef SFS_UTIL_H
#define SFS_UTIL_H

#include <string.h>
#include "sfs_header.h"
// FAT 初始化
void FAT_init(FileAllocationTable *fat);
// 在FAT中查找一个空闲块
int FAT_getFreeNode(FileAllocationTable *fat);
// 文件目录初始化
void DirectoryDescriptor_init(DirectoryDescriptor *root);
// 根据文件名在文件目录中查找一个文件
int getIndexOfFileInDirectory(char *name, DirectoryDescriptor *rootDir);
// 根据文件名创建一个文件
void FileDescriptor_createFile(char *name, FileDescriptor *file, DirectoryDescriptor *rootDir, FileAllocationTable *fat);
void prtDir(DirectoryDescriptor* rootDir, int line, char* file);
void prtFAT(FileAllocationTable* fat, int line, char* file);

#endif
