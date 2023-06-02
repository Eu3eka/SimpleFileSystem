#ifndef SFS_HEADER_H
#define SFS_HEADER_H

#include <time.h>
#include <stdint.h>
#include <stdio.h>

#define FILE_LIST_SIZE 1024 //文件列表最大大小
#define FILENAME_SIZE 32//文件名最长长度
#define BLOCK_SIZE 4096//磁盘块大小，单位字节
#define NUM_BLOCKS 1024//初始时未分配的磁盘块数量
#define DISK_NAME "disk.txt"

static FILE* disk_file = NULL;

typedef enum {
    FILE_OPEN,   // 读取状态
    FILE_CLOSED     // 关闭状态
} FileAccessStatus;

typedef struct FileDescriptor {         // FCB
  char                filename[FILENAME_SIZE];   // 文件名
  int                 fat_index;      // 在fat 上的index
  time_t             timestamp;      // 创建时间
  int                 size;           // 文件大小
  FileAccessStatus    fas;            // 文件状态
} FileDescriptor;//文件元数据

typedef struct DirectoryDescriptor {    // 描述目录信息
  FileDescriptor table[FILE_LIST_SIZE];    //各个文件的元数据
  int count;                          // 文件个数
} DirectoryDescriptor;

typedef struct FileAllocationTable {
  int16_t table[NUM_BLOCKS];           //所有的块的分配情况，初始的时候为全-1，代表没有被分配,分配后置1
  int16_t count;                          // 已经分配的块的个数
} FileAllocationTable;

#endif 
