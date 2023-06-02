#include "sfs_header.h"
#include "disk_emu.h"
#include "sfs_util.h"

void FAT_init(FileAllocationTable *fat) {
     for(int i=0;i<NUM_BLOCKS;i++){
        fat->table[i] = -1;
     }
		 fat->table[0] = 1;
		 fat->table[1] = 1;
     fat->count = 2;
		  // 将 FAT 表写入磁盘的第 1 块
    ds_write_blocks(1, 1, fat);
}

int FAT_getFreeNode(FileAllocationTable *fat) {
		//块数不足	
		if(fat->count == NUM_BLOCKS){
			printf("Blocks inadequate!\n");
			return -1;
		}

		for(int i=0;i<NUM_BLOCKS;i++){//首次适应
			if(fat->table[i] == -1){//空闲块
				fat->table[i] = 0;
				fat->count++;
				ds_write_blocks(1,1,fat);
				printf("Block %d is empty, allocated\n", i);
				return i;//返回分配的块号
			}
		}
		//没有空闲块，返回-1
		printf("Blocks inadequate!\n");
		ds_write_blocks(1,1,fat);
		return -1;
}

void DirectoryDescriptor_init(DirectoryDescriptor *root) {
     //文件个数设置为 0
    root->count = 0;

    //元数据初始化
    for (int i = 0; i < FILE_LIST_SIZE; i++) {
        FileDescriptor *fd = &(root->table[i]);
        fd->filename[0] = '\0';
        fd->fat_index = -1;
        fd->timestamp = 0;
        fd->size = 0;
        fd->fas = FILE_CLOSED;//关闭状态
    }
		// 将文件目录写入磁盘的第 0 块
    ds_write_blocks(0, 1, root);
}

int getIndexOfFileInDirectory(char *name, DirectoryDescriptor *rootDir) {
     for(int i=0;i<rootDir->count;i++){
        if(strcmp(name ,rootDir->table[i].filename) == 0){
          return i;
        }
     }
     return -1;//没有找到
}

void FileDescriptor_createFile(char *name, FileDescriptor *file, DirectoryDescriptor *rootDir, FileAllocationTable *fat) {
  //查找有没有同名文件
	// prtDir(rootDir,__LINE__,__FILE__);
	// prtFAT(fat,__LINE__,__FILE__);
	int file_index = getIndexOfFileInDirectory(name,rootDir);
	if(file_index == -1){//没找到
		if(rootDir->count < FILE_LIST_SIZE){
			strcpy(file->filename, name);
			int freenode = FAT_getFreeNode(fat);
			printf("Now using block %d\n", freenode);
			fat->table[freenode] = 1;
			file->fat_index = freenode;//分配的块
			file->timestamp = time(NULL);
			file->size = 0;
			file->fas = FILE_CLOSED;
			rootDir->count++;
			rootDir->table[rootDir->count] = *file;
			ds_write_blocks(0,1,rootDir);
		} else {
			printf("Directory is full, delete any file before creating.\n");
			return;
		}
	} else {//实现检测重名文件和重命名功能
		printf("File already exists!\n");
		char cmd_buf[10];
		printf("Enter command to decide whether to overwrite or rename(O/R): ");
		scanf("%s", cmd_buf);
		char new_name[FILENAME_SIZE];//文件名最长32个字符
		if(cmd_buf[0] == 'R' || cmd_buf[0] == 'r'){
			again:
			printf("Please input new name:");
			scanf("%s", new_name);
			int new_name_index = getIndexOfFileInDirectory(new_name, rootDir);
			if(new_name_index == -1){//不存在重名文件
				strcpy(rootDir->table[file_index].filename, new_name);
				strcpy(file->filename, new_name);
			} else {
				printf("File name already exists in directory, please enter again.");
				goto again;
				return;
			}
		} else if(cmd_buf[0] == 'O' || cmd_buf[0] == 'o'){
			file->fat_index = file_index;
			strcpy(file->filename, rootDir->table[file_index].filename);//不改变原来文件的文件名
			rootDir->table[file_index] = *file;
		} else {
			printf("Invalid command.\n");
			return ;
		}
		ds_write_blocks(0,1,rootDir);
	}
}

void prtDir(DirectoryDescriptor* rootDir, int line, char* file){
	printf("\nCall at file %s, line %d\n", file, line);
	if(rootDir == NULL) printf("FileDirectory is empty!");	
	else {
		printf("table[0] is:filename - %s\nfat_index - %d\ntimestamp - %ld\nsize - %d\n", 
		rootDir->table[0].filename, rootDir->table[0].fat_index, rootDir->table[0].timestamp, rootDir->table[0].size);
		if(rootDir->table[0].fas == FILE_OPEN) printf("fas - FILE_OPEN\n");
		else if(rootDir->table[0].fas == FILE_CLOSED) printf("fas - FILE_CLOSED\n");
		else printf("fas undefined\n");
		printf("Directory size is:%d\n", rootDir->count);
	} 
}

void prtFAT(FileAllocationTable* fat, int line, char* file){
	printf("\nCall at file %s, line %d\n", file, line);
  if(fat == NULL) printf("FAT is empty!");	
	else{
		for(int i=0;i<fat->count;i++){
			printf("fat table[%d] is:%d", i, fat->table[i]);
		}
		printf("fat count is:%d\n", fat->count);
	}
}
