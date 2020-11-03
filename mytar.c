# include <stdlib.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <string.h>
# include <limits.h>
# include <stdint.h>

int format_name_prefix(char *name, char *prefix, 
				char *file, char *path){

	/*If everything fits in name ...*/
	if (strlen(file) + strlen(path) + 1 <= 100){

	int count = 0;
	
        for (count = 0; count < strlen(path); count++)
        name[count] = path[count];
        name[count] = '\0';
	strcat(name, "/");
        strcat(name, file);
	return 0;
	}

	/*If name is full ...*/
	else if (strlen(file) == 100){
	if (strlen(path) == 0){	
	strcpy(name, file);
	return 0;
	}	
	else if(strlen(path) >= 100)
	return -1;
	else{
	strcpy(name, file);
	strcpy(prefix, path);
	strcat(prefix, "/");
	return 0;	
	}
	}

	else{
        /*If file name is too big, there isn't a way to break it...*/
	if(strlen(file) > 100){
		return -1;
	}

	/*If path and file name is too big, try to break path ...*/
	else if(strlen(path) + strlen(file) + 1 > 100){
	int count = 0;	
	int break_point;
	int success = 0;
	int count_name = 0;
	int name_count;
	for (count = 0; count < strlen(path); count++){
	
	if (path[count] == '/'){
		if (strlen(path) + strlen(file) + 1 - count <= 100){
		success = 1;
		break_point = count;
		break;
		}		
		}
	}
	/*If unable to break ...*/
	if (success == 0)
	return -1;
	/*If able to break ...*/
		/*Put a little path on name*/
	for (count = 0; count < strlen(path); count++){
	if (count <= break_point)
	prefix[count] = path[count];
	else{
	name[name_count] = path[count];
	name_count++;
	}
	}
	name[name_count] = '/';
	name_count++;
		/*Put file name on name*/
	for (count = 0; count < strlen(name); count++){
	name[name_count] = file[count];
	name_count++;
	}
	return 1;

	}
	}
}


void write_header(char *file, char *path, int out_fd){
	char *name = calloc(100, 1); /*NUL-terminated if NUL fits*/
	uint8_t *mode = calloc(8, 1);
	uint8_t *uid = calloc(8, 1);
	uint8_t *gid = calloc(8, 1);
	uint8_t *size = calloc(12, 1); /*Directories and symlinks are zero*/
	uint8_t *mtime = calloc(12, 1);
	uint8_t *chksum = calloc(8, 1);
	uint8_t *typeflag = calloc(1, 1);
	uint8_t *linkname = calloc(100, 1);/*NUL-terminated if NUL fits*/
	uint8_t *magic = calloc(6, 1);/*must ve "ustar", NUL-terminated*/
	uint8_t *version = calloc(2, 1);/*must be "00" (zero-zero) */
	uint8_t *uname = calloc(32, 1);/*NUL-terminated*/
	uint8_t *gname = calloc(32, 1);/*NUL-terminated*/
	uint8_t *devmajor = calloc(8, 1);
	uint8_t *devminor = calloc(8, 1);
	char *prefix = calloc(155, 1);/*NUL-terminated if NUL fits*/

	/*If unable to format name, ignore file ...*/
	if(format_name_prefix(name, prefix, file, path) == -1){
	printf("unable to format file %s with path %s", file, path);
	return;
	}
	
}

/*Use stat*/
int dfs(const char *path, const char *name){
	DIR *dp;
	struct dirent *entry;
	struct stat buf; /*used for checking entries*/
	
	/*Report error and don't stop ...*/
	if(chdir(path) != 0){
	perror(path);
	return 0;
	}
	
	/*Opening directory specified*/
	dp = opendir(".");
	if (dp == NULL){
	perror("opendir");
	return 0;
	}	

	while ( (entry = readdir(dp)) != NULL){
		
		/*Not interested in traversing up, nor 
 * 	   	  getting stuck in a loop ... */
		if (strcmp(entry -> d_name, ".") != 0 && 
		  strcmp(entry -> d_name, "..") != 0){
		
		/*Strcpy was giving me a hard time ...*/
		char *new_path = calloc(PATH_MAX, 1);
		int count = 0;
		for (count = 0; count < strlen(path); count++)
		new_path[count] = path[count];
		new_path[count] = '\0';
		strcat(new_path, "/");
                strcat(new_path , entry ->d_name);
		if (stat(new_path, &buf) != 0){
                        perror("stat");
                     	continue;
			   }
		/*Not interested in symlinks ...*/
		else if (S_ISLNK(buf.st_mode))
/* sa.sa_handler = &handler_tick;*/
		continue;

		/*If a direcory, traverse down ...*/
		else if (S_ISDIR(buf.st_mode) ){
/*Call header*/
		dfs(new_path, name);
		}
		
		/*If we find a file with the name, 
 * 		  print its path and continue ...*/
		else if (S_ISREG(buf.st_mode)){
/*Call header*/	
		}
		/* free(new_path);*/
		}
	}
	closedir(dp);
	return 0;
}

void command_parser( int *flag_c, int *flag_t, int *flag_x, int *flag_v, 
			int *flag_S, int argc, char *argv[]){
	int count;
	if (argc < 3){
        perror("Usage: mytar [ctcvS]f tarfile [ path [ ... ] ]");
        exit(1);
        }


	for (count = 0; count < strlen(argv[1]) - 1; count++){
		if (argv[1][count] == 'c')
		 *flag_c = 1;
		else if(argv[1][count] == 't')
		 *flag_t = 1;
		else if(argv[1][count] == 'x')
		 *flag_x = 1;
		else if(argv[1][count] == 'v')
		 *flag_v = 1;
		else if(argv[1][count] == 'S')
		 *flag_c = 1;
		else
		printf("unknown flag: %c/n", argv[1][count]);	

	}
}

int main(int argc, char *argv[]){
	
	int *flag_c = malloc(sizeof(int)); 
	int *flag_t = malloc(sizeof(int)); 
	int *flag_x = malloc(sizeof(int));
	int *flag_v = malloc(sizeof(int));
	int *flag_S = malloc(sizeof(int));


	/*Setting all flags to false ...*/
        *flag_c = 0;
        *flag_t = 0;
        *flag_x = 0;
        *flag_v = 0;
        *flag_S = 0;
	
	command_parser(flag_c, flag_t, flag_x, flag_v,
			 flag_S, argc, argv);

	/*Creating a new archive file ...*/	

	

	return 0;
}







