#include <stdlib.h>
#include <string.h>
#include "sfs_header.h"
#include "disk_emu.h"
#include "sfs_util.h"
#include "sfs_api.h"

int main(){
  //文件系统提供初始化、打开系统、关闭系统、显示目录、创建文件、删除文件、写文件、读文件的基本操作
	sfs_system_init();
	fseek(disk_file, 0, SEEK_SET);
  char op[100];
	char command[10];
	char filename[90];
	int bytes;
	char buf[1024];
	// FileAccessStatus* fas;
	// DirectoryDescriptor* root;
	// FileDescriptor file;
	printf("Welcome to File management system, enter 'help' for command help:\n");
	while(1){
		printf("Enter your command:\n");
		memset(op, 0, sizeof(op)); 
		if(fgets(op, sizeof(op), stdin) == NULL) {
			printf("Failed to read command\n");
			continue;
		}
		op[strcspn(op, "\n")] = '\0'; 
		if(strcmp(op, "help") == 0){
			printf("Commands below:\n");
			printf("help ---- show system commands\n");
			printf("info ---- show system info");
			printf("ls ---- show file directory\n");
			printf("exit ---- exit system\n");
			printf("op [filename] ---- open the specified file\n");
			printf("cl [filename] ---- close the specified file\n");
			printf("rd [filename] [bytes] ---- read the specified number of bytes from the specified file.\nif file length is less than the bytes you put, system will put all of the file content on the screen.\n");
			printf("wt [filename] [bytes] ---- write specified number of bytes to the specified file\n");
		} else if(strcmp(op, "info") == 0){
			sfs_system_info();
		} else if(strcmp(op, "ls") == 0){
			sfs_ls();
		} else if(strcmp(op, "exit") == 0){
			sfs_system_close();
			printf("System closed\n");
			break;
		} else if(op[0] == 'o' && op[1] == 'p'){//open
			if (sscanf(op, "%9s %49s", command, filename) != 2) {
				printf("Wrong format of command\n");
				continue;
			} else if (filename[0] == '\0') {
					printf("File name cannot be empty\n");
					continue;
			} else {
				int res = sfs_open(filename);
				if(res == 1){
					printf("File does not exist, newly created for you, its status is opened now.\n");
				}	
			} 
			printf("File opened.\n");
		} else if(op[0] == 'c' && op[1] == 'l'){//close
			if (sscanf(op, "%9s %49s", command, filename) != 2) {
				printf("Wrong format of command\n");
				continue;
			} else if (filename[0] == '\0') {
					printf("File name cannot be empty\n");
			} else {
				int res = sfs_close(filename);
				if(res == -2){
					printf("File does not exist\n");
				} else if(res == -1){
					printf("File is already closed\n");
				} else printf("File '%s' is closed", filename);
			}
		} else if(op[0] == 'r' && op[1] == 'd'){//read
			if (sscanf(op, "%9s %49s %d", command, filename, &bytes) != 3) {
				printf("Wrong format of command\n");
			} else if(filename[0] == '\0'){
				printf("File name cannot be empty\n");
			} else if (bytes <= 0) {
				printf("The number of bytes must be positive\n");
			} else {
				int res = sfs_read(filename, buf, bytes);
				if(res== -2){
					printf("No such file, please create before writing content to it.\n"); 
				} else if(res == -1){
					printf("File is closed.\n");
				} else if(res == 0) printf("Successfully read, file content is:\n%s\n", buf);
			}
		} else if(op[0] == 'w' && op[1] == 't'){//write
			if (sscanf(op, "%9s %49s %d", command, filename, &bytes) != 3) {
				printf("Wrong format of command\n");
			} else if(filename[0] == '\0'){
				printf("File name cannot be empty\n");
			} else if (bytes <= 0) {
				printf("The number of bytes must be positive\n");
			} else {
				printf("Enter the content you want to write:\n");
				scanf("%s", buf);
				getchar();
				if(buf[0] == '\0'){
					printf("Content cannot be empty\n");
				}
				int res = sfs_write(filename, buf, bytes);
				if(res== -2){
					printf("No such file, please create before writing content to it.\n"); 
				} else if(res == -1){
					printf("File is closed.\n");
				} else if(res == 0) printf("Successfully wrote\n");
			}
		} else printf("No such commands, enter again\n");
	}
	return 0;
}
