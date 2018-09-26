#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <unistd.h>

int main(int argc, char** argv)
{
	char config_file_name[2048];
	if(argc < 2)
	{
		printf("No config file provided\n");
		strcpy(config_file_name, "push_config.cfg");
	}
	else
	{
		strcpy(config_file_name, argv[1]);
	}
	
	
	FILE* config_file = fopen(config_file_name, "r");
	if(config_file == NULL)
	{
		fprintf(stderr, "Failed to open \"%s\"\n", config_file_name);
		return -1;
	}
	int read_amount = 0;
	size_t file_in_size = 2048;
	char *mount_point = new char[file_in_size];
	read_amount = getline(&mount_point, &file_in_size,config_file);
	mount_point[strlen(mount_point) - 1] = 0;
	if(read_amount == -1)
	{
		fprintf(stderr, "config_file is incomplete\n");
		return -1;
	}
	char mount_dest[256];
	if(fgets(mount_dest, 256, config_file) == NULL)
	{
		fprintf(stderr, "config_file is incomplete\n");
		return -1;
	}
	
	char *rem_pos;
	if((rem_pos=strchr(mount_dest, '\n')) != NULL)
	{
		*rem_pos = 0;
	}
	
	
	printf("Mount point is \"%s\"\n", mount_point);
	printf("File destination is \"%s\"\n", mount_dest);
	char mount_check_str[256];
	sprintf(mount_check_str, "mountpoint %s -q; printf \"$?\n\"", mount_point);
	FILE* mount_check = popen(mount_check_str, "r");
	char m_check_read[16];
	fgets(m_check_read, 16,  mount_check);
	m_check_read[1] = 0;
	
	
	if(atoi(m_check_read) == 1)//mount
	{
		printf("Destination not mounted, attempting mount from \"%s\"\n", mount_dest);
		char mount_command[1024];
		sprintf(mount_command, "sudo sshfs -o allow_other %s %s",mount_dest, mount_point);
		
		system(mount_command); 
	}
	
	//copy files
	char file_to_copy[1024];
	while(fgets(file_to_copy, 1024, config_file) != NULL)
	{
		if((rem_pos=strchr(file_to_copy, '\n')) != NULL)
		{
			*rem_pos = 0;
		}
		char copy_command[2048];
		sprintf(copy_command, "cp %s %s", file_to_copy, mount_point);
		system(copy_command);
	}
	
	return 0;
}


