#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void read_file_to_buffer(const char *filename, char **buffer) {
	printf("read file: %s\n", filename);
	struct stat path_stat;
	stat(filename, &path_stat);
	if (!S_ISREG(path_stat.st_mode)) {
		return;
	}
	long length;
	FILE *f = fopen(filename, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		*buffer = malloc(length + 1);
		if (*buffer) {
			fread(*buffer, 1, length, f);
		}
		(*buffer)[length] = '\0';
		fclose(f);
	}
}

void save_current_error_to_dir(char *filename, char *content) {
	FILE *f = fopen(filename, "w+");
	fputs(content, f);
	fclose(f);
}

int main(int argc, char const *argv[]) {
	if (argc < 3) {
		printf("Usage: fuzz_test_remove_duplicate dir log\n");
		return 0;
	}
	const char *dir = argv[1];
	const char *filename = argv[2];

	char *current_error_file_content;
	read_file_to_buffer(filename, &current_error_file_content);
	DIR *dr = opendir(dir);
	struct dirent *de;
	int file_index = 0;
	int found = 0;
	while ((de = readdir(dr)) != NULL) {
		char unique_error_file_name[1000];
		printf("%s\n", de->d_name);
		sprintf(unique_error_file_name, "%s/%s", dir, de->d_name);
		char *unique_error_file_content = NULL;
		read_file_to_buffer(unique_error_file_name, &unique_error_file_content);
		if (unique_error_file_content != NULL) {
			if (strcmp(current_error_file_content, unique_error_file_content) == 0) {
				printf("error duplicated!\n");
				exit(-1);
			}
			file_index++;
		}
	}

	char save_filename[100];
	sprintf(save_filename, "%s/%d", dir, file_index);
	save_current_error_to_dir(save_filename, current_error_file_content);

	return 0;
}
