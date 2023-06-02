#include "sfs_api.h"
#include "sfs_util.h"
#include "disk_emu.h"
#include "sfs_header.h"
// 初始化文件系统
void sfs_system_info(){
    FileAllocationTable *fat = (FileAllocationTable*)malloc(sizeof(FileAllocationTable));
    DirectoryDescriptor* root = (DirectoryDescriptor*)malloc(sizeof(DirectoryDescriptor));
    ds_read_blocks(1,1,fat);
    printf("\n ==== File System Info ==== \n");
    printf("\nDisk Size %d Bytes\n", NUM_BLOCKS * BLOCK_SIZE);
    printf("\nAlready Used %d Blocks, (%.2f)%% \n",  root->count);
    printf("\nFile Num %d \n", root->count);
}
void sfs_system_init(){
    FileAllocationTable* fat = (FileAllocationTable*)malloc(sizeof(FileAllocationTable));
    DirectoryDescriptor* rootDir = ( DirectoryDescriptor*)malloc(sizeof(DirectoryDescriptor));
    if(ds_init(DISK_NAME, BLOCK_SIZE, NUM_BLOCKS) == -1){
        ds_init_fresh(DISK_NAME, BLOCK_SIZE, NUM_BLOCKS);
    } 
    FAT_init(fat);
    DirectoryDescriptor_init(rootDir);
}
void sfs_system_close(){
    ds_close();
}

// 目录操作
void sfs_ls(){
    DirectoryDescriptor *rootDir = (DirectoryDescriptor*)malloc(sizeof(DirectoryDescriptor));
    int res = ds_read_blocks(0, 1, rootDir);//从磁盘中读取文件目录
    printf("%d block(s) read.\n", res);
    if(rootDir->table[0].size == 0){
        printf("System empty.\n");   
    } else {
        printf("%-15s | %-10s | %-10s\n", "File name", "FAT_index", "Size");
        for(int i=0;i<rootDir->count;i++){//遍历文件目录
        printf("%-15s | %-10d | %-10d\n", rootDir->table[i].filename, rootDir->table[i].fat_index, rootDir->table[i].size);
        } 
    }
    free(rootDir);
}

// 文件操作
int sfs_open(char *name){
    DirectoryDescriptor* rootDir = (DirectoryDescriptor*)malloc(sizeof(DirectoryDescriptor));
    ds_read_blocks(0,1,rootDir);
    // prtDir(rootDir,__LINE__, __FILE__);
    for(int i=0;i<rootDir->count;i++){
        if(strcmp(name, rootDir->table[i].filename) == 0){
            if(rootDir->table[i].fas == FILE_OPEN){
                printf("File already opened.\n");
            } else rootDir->table[i].fas = FILE_OPEN;
            ds_write_blocks(0,1,rootDir);
            return 0;
        }
    }
    FileDescriptor* file = (FileDescriptor*)malloc(sizeof(FileDescriptor));
    FileAllocationTable* fat = (FileAllocationTable*)malloc(sizeof(FileAllocationTable));
    FileDescriptor_createFile(name, file, rootDir, fat);
    ds_read_blocks(1,1,fat);
    ds_read_blocks(0,1,rootDir);
    // prtDir(rootDir,__LINE__,__FILE__);
    // prtFAT(fat, __LINE__, __FILE__);
    return 1;//表示创建了原来不存在的文件
}

int sfs_close(char* name){
    DirectoryDescriptor* root = (DirectoryDescriptor*)malloc(sizeof(DirectoryDescriptor));
    ds_read_blocks(0,1,root);
    // prtDir(root, __LINE__, __FILE__);
    FileDescriptor ff = root->table[0];
    for(int i=0;i<root->count;i++){
        if(strcmp(name, root->table[i].filename) == 0){
            if(root->table[i].fas == FILE_CLOSED){
                return -1;
            } else root->table[i].fas = FILE_CLOSED;
            ds_write_blocks(0,1,root);
            return 0;
        }
    }
    return -2;
}

int sfs_write(char* name, char *buf, int length) {
    int i=0;
    // printf("Now at %s %d line", __FILE__, __LINE__);
    FileAccessStatus* fas = (FileAccessStatus*)malloc(sizeof(FileAccessStatus));
	DirectoryDescriptor* root = (DirectoryDescriptor*)malloc(sizeof(DirectoryDescriptor));
	FileDescriptor file;
    ds_read_blocks(0,1,root);
    // prtDir(root, __LINE__, __FILE__);
    for(i=0;i<root->count;i++){
        file = root->table[i];
        if(strcmp(file.filename, name) == 0){
            if(file.fas == FILE_CLOSED) return -1;
            else {
              int offset = file.fat_index*BLOCK_SIZE + file.size;
              fseek(disk_file, offset, SEEK_SET);
              fwrite(buf, sizeof(char), length, disk_file);
              root->table[i].size += length;
              ds_write_blocks(0,1,root);
              break;
            }
        }
    }   
    if(i == root->count) return -2;
    else return 0;
}

int sfs_read(char* name, char *buf, int length) {
    int i=0;
    FileAccessStatus* fas = (FileAccessStatus*)malloc(sizeof(FileAccessStatus));
	DirectoryDescriptor* root = (DirectoryDescriptor*)malloc(sizeof(DirectoryDescriptor));
    ds_read_blocks(0,1,root);
    // prtDir(root, __LINE__, __FILE__);
    for(i=0;i<root->count;i++){
        if(strcmp(root->table[i].filename, name) == 0){
            if(root->table[i].fas == FILE_CLOSED) return -1;
            else {
              int offset = root->table[i].fat_index*BLOCK_SIZE;
            //   fseek(disk_file, offset, SEEK_SET);
              length = min(length, root->table[i].size);
              fread(buf, sizeof(char), length, disk_file);
              break;
            }
        }        
    }
    if(i == root->count) return -2;
    else return 0;
}

int min(int a,int b){
    return a>b?b:a;
}
