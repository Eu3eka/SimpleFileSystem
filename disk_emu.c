#include "disk_emu.h"
#include "sfs_header.h"

int ds_init_fresh(char *filename, int block_size, int num_blocks) {
    printf("Creating disk file...\n");
    disk_file = fopen(filename, "wb+");
    if(disk_file == NULL){
        printf("Error: Failed to create disk\n");
        return -1;
    }

    char *block = (char*) calloc(num_blocks, block_size);
    for(int i=0;i<num_blocks;i++){
        fwrite(block, block_size, 1, disk_file);
    }
    printf("Successfully created disk file.\n");
    
    return 0;
}

int ds_init(char *filename, int block_size, int num_blocks) {
     //打开现有磁盘文件
    disk_file = fopen(filename, "rb+");
    if (disk_file == NULL) {
        printf("Error: Failed to open disk file\n");
        return -1;
    }
    
     //检查磁盘文件是否有效
    if (block_size != BLOCK_SIZE || num_blocks != NUM_BLOCKS) {
        printf("Error: Invalid disk file\n");
        fclose(disk_file);
        return -1;
    }
    return 0;

}

int ds_read_blocks(int start_node_index, int nblocks, void *buffer) {
     //检查读取的块是否超出磁盘块的范围
    if (start_node_index + nblocks > NUM_BLOCKS) {
        return -1;
    }

    if(disk_file == NULL) return -1;
    //临时缓冲区
    void* tmpbuff = (void*) malloc(BLOCK_SIZE);

    //移动指针
    fseek(disk_file, start_node_index * BLOCK_SIZE, SEEK_SET);

    for (int i=0; i<nblocks; i++){   
        fread(tmpbuff, BLOCK_SIZE, 1, disk_file);
        for (int j=0; j<BLOCK_SIZE; j++){
            memcpy(buffer+(i*BLOCK_SIZE), tmpbuff, BLOCK_SIZE);  
        }
    }
    free(tmpbuff);
    return nblocks;
}

int ds_write_blocks(int start_node_index, int nblocks, void *buffer) {
    //写入的块是否超出磁盘块的范围
    if (start_node_index + nblocks > NUM_BLOCKS) {
        return -1;
    }

    void* tmpbuff = (void*) malloc(BLOCK_SIZE);
	
    fseek(disk_file, start_node_index * BLOCK_SIZE, SEEK_SET);
   
    for (int i=0;i<nblocks;i++){
        memcpy(tmpbuff, buffer+(i*BLOCK_SIZE), BLOCK_SIZE);
        fwrite(tmpbuff, BLOCK_SIZE, 1, disk_file);
        fflush(disk_file);
    }
    free(tmpbuff);
    return nblocks;
}

int ds_close() {
     if(disk_file != NULL){
        fclose(disk_file);
        disk_file = NULL;
     }
     return 0;
}

