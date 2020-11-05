# include <stdlib.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <string.h>
# include <limits.h>
# include <stdint.h>
# include <pwd.h>
# include <fcntl.h>

# define BLOCK_SIZE 512

int format_name_prefix(char *name, char *prefix, 
				char *file, char *path){

	int len_path = 0;
	if (path != NULL)
	len_path = 0;
	else if(path[0] == '\0')
	len_path = 0;
	/*If everything fits in name ...*/
	if (strlen(file) + len_path + 1 <= 100){

	int count = 0;
	
        for (count = 0; count < len_path; count++)
        name[count] = path[count];
        name[count] = '\0';
	strcat(name, "/");
        strcat(name, file);
	return 0;
	}

	/*If name is full ...*/
	else if (strlen(file) == 100){
	if (len_path == 0){	
	strcpy(name, file);
	return 0;
	}	
	else if(len_path >= 100)
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
	else if(len_path + strlen(file) + 1 > 100){
	int count = 0;	
	int break_point;
	int success = 0;
	int name_count;
	for (count = 0; count < len_path; count++){
	
	if (path[count] == '/'){
		if (len_path + strlen(file) + 1 - count <= 100){
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
	for (count = 0; count < len_path; count++){
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
	return 1;
}
/*This function creates an octal string represent. for a 32-int*/
char *dec_to_stroct(int n, int size){
	
	int count = 0;
	char *ret;
	char conversions[8] = {'0', '1', '2', '3', '4', '5', '6', '7'};
	ret = malloc(size);
	if (ret == NULL){
	perror("malloc");
	exit(EXIT_FAILURE);
	} 
	for (count = 0; count < size - 1; count++)
	ret[count] = '0';
	for (count = 0; n > 0; count++){
	ret[size - count - 2] = conversions[n % 8];
	n = n /8;
	}
	ret[size -1] = '\0';
	return ret;

}

int format_from_lstat(char *file, char *path, char *mode, 
			char *uid, char *gid, char *size,
			char *mtime, char *typeflag, 
			char *devmajor, char *devminor, 
			char *linkname, char *uname, 
			char *gname){
	struct stat buf;
	int perm = 0;	
	char conversions[8] = {'0', '1', '2', '3', '4', '5', '6', '7'};	
	char *ret;
	int inter;
	struct passwd *pass;
	/*Finding file ...*/
	if (path != NULL){

	/*Report error ...*/
        if(chdir(path) != 0){
        perror(path);
        return -1;
        }	
	}
	
	if (-1 == lstat(file, &buf)){
	perror("lstat");
	return -1;
	}

	/*Getting Mode ...*/
		/*Padding with leading zeros and terminating NULL ...*/	
	mode[0] = '0';
	mode[1] = '0';
	mode[2] = '0';
	mode[7] = '\0';
		/*UID and GID and sticky bit ...*/
	if (S_ISUID & buf.st_mode)
		perm = perm + 4;
	if (S_ISGID & buf.st_mode)
                perm = perm + 2;
	if (S_ISVTX & buf.st_mode)
                perm = perm + 1;
	mode[3] = conversions[perm];
	perm = 0;
		/*User permissions ...*/
	if (S_IRUSR & buf.st_mode){
	perm = perm + 4; }
	if (S_IWUSR & buf.st_mode)
	perm = perm + 2;
	if (S_IXUSR & buf.st_mode)
	perm = perm + 1;
	mode[4] = conversions[perm];
	perm = 0;
		/*Group permissions ...*/
	if (S_IRGRP & buf.st_mode){
        perm = perm + 4; }
        if (S_IWGRP & buf.st_mode)
        perm = perm + 2;
        if (S_IXGRP & buf.st_mode)
        perm = perm + 1;
	mode[5] = conversions[perm];
	perm = 0;
		/*Other permissions ...*/
	if (S_IROTH & buf.st_mode){
        perm = perm + 4; }
        if (S_IWOTH & buf.st_mode)
        perm = perm + 2;
        if (S_IXOTH & buf.st_mode)
        perm = perm + 1;
	mode[6] = conversions[perm];
	perm = 0;
	
	/*Getting UID and GID values ...*/
	ret = dec_to_stroct(buf.st_uid, 12);
	strcpy(uid, ret);
	free(ret);
	ret = dec_to_stroct(buf.st_gid, 12);
	strcpy(gid, ret);
        free(ret);	
	
	/*Uname and Gname ...*/
	pass = getpwuid(buf.st_uid);
	if (pass != NULL)
	strcpy(uname, pass -> pw_name);
	pass = getpwuid(buf.st_gid);
	if (pass != NULL)
        strcpy(uname, pass -> pw_name);

	/*Getting size ...*/
	if (S_ISREG(buf.st_mode)){	
	ret = dec_to_stroct(buf.st_size, 12);
	strcpy(size, ret);
	free(ret);}
	else{size = strcpy(size, "00000000000");}

	/*Getting mtime ...*/
	ret = dec_to_stroct(buf.st_mtime, 12);
	strcpy(mtime, ret);
	free(ret);

	/*Getting typeflag ...*/
	if (S_ISDIR(buf.st_mode))
	*typeflag = '5';
	else if (S_ISLNK(buf.st_mode))
	*typeflag = '2';
	else if (S_ISREG(buf.st_mode)){
/*NOTE:: Missing to include regular file alternate*/
	*typeflag = '0';
	}

	/*Getting major and minor dev numbs ...*/

	inter = major(buf.st_dev);
	strcpy(devmajor, dec_to_stroct(inter, 8));
	inter = minor(buf.st_dev);
	strcpy(devminor, dec_to_stroct(inter,  8));	

	/*If link ...*/
	if (S_ISLNK(buf.st_mode)){
	if (-1 == readlink(file, linkname, 100))
	perror("return readlink");
	return -1;

	}
	

	return 0;
}


void write_header(char *file, char *path, int fd_out){
	char *name = calloc(100, 1); /*NUL-terminated if NUL fits*/
	char *mode = calloc(8, 1);
	char *uid = calloc(8, 1);
	char *gid = calloc(8, 1);
	char *size = calloc(12, 1); /*Directories and symlinks are zero*/
	char *mtime = calloc(12, 1);
	uint8_t *chksum = calloc(8, 1);
	char *typeflag = calloc(1, 1);
	char *linkname = calloc(100, 1);/*NUL-terminated if NUL fits*/
	char *magic = calloc(6, 1);/*must ve "ustar", NUL-terminated*/
	char *version = calloc(2, 1);/*must be "00" (zero-zero) */
	char *uname = calloc(32, 1);/*NUL-terminated*/
	char *gname = calloc(32, 1);/*NUL-terminated*/
	char *devmajor = calloc(8, 1);
	char *devminor = calloc(8, 1);
	char *prefix = calloc(155, 1);/*NUL-terminated if NUL fits*/
	uint8_t *buf = calloc(BLOCK_SIZE, 1);
	int count = 0;
	/*If unable to format name, ignore file ...*/
	if(format_name_prefix(name, prefix, file, path) == -1){
	printf("unable to format file %s with path %s", file, path);
	return;
	}

	/*If unable to format mode, ignore file ...*/
	if(format_from_lstat(file, path, mode, uid, gid, size, 
				mtime, typeflag, devmajor, 
				devminor, linkname, uname, gname) == -1){
        printf("unable to format file %s with path %s", file, path);
        return;
        }
	/*Magic number and version ...*/
	strcpy(magic, "ustar");	
	version[0] = '0';
	version[1] = '0';	
	/*Chksum ... */
	*chksum = 0;


	for (count = 0; count < BLOCK_SIZE; count++){

	if (count < 100)
	buf[count] = name[count];
	else if ( count < 108)
	buf[count] = mode[count - 100];
	else if ( count < 116)
	buf[count] = uid[count - 108];
	else if ( count < 124)
	buf[count] = gid[count - 116];
	else if ( count < 136)
	buf[count] = size[count - 124];
	else if ( count < 148)
	buf[count] = mtime[count - 136];
	else if ( count < 156)
	buf[count] = chksum[count - 148];
	else if ( count < 157)
	buf[count] = typeflag[count - 156];
	else if ( count < 257)
	buf[count] = linkname[count - 157];
	else if ( count < 263)
	buf[count] = magic[count - 257];
	else if ( count < 265)
	buf[count] = version[count - 263];
	else if ( count < 297)
	buf[count] = uname[count - 265];
	else if ( count < 329)
	buf[count] = gname[count - 297];
	else if ( count < 337)
	buf[count] = devmajor[count - 329];
	else if ( count < 345)
	buf[count] = devminor[count - 337];
	else if (count < 510)
	buf[count] = prefix[count - 345];




	}


	if (write(fd_out, buf, BLOCK_SIZE) != BLOCK_SIZE){
	perror("write");	
	exit(EXIT_FAILURE);
	}
}

int write_file(int fd_in, int fd_out){

	uint8_t buf[BLOCK_SIZE];
	int ret = read(fd_in, &buf, BLOCK_SIZE);
        
	while (ret == BLOCK_SIZE){
	if (write(fd_out, &buf,  BLOCK_SIZE) !=  BLOCK_SIZE){
	perror("read");
	return -1;
        }
	ret = read(fd_in, &buf, BLOCK_SIZE);
	}
	if (ret == -1){
        perror("read");
	return -1;}
	/*Writing left-overs ...*/
	if (ret < BLOCK_SIZE){
	if (write(fd_out, &buf,  BLOCK_SIZE) !=  BLOCK_SIZE){
        perror("read");
        return -1;
        }}
	return 0;	

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
 	   	  getting stuck in a loop ... */
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
		continue;

		/*If a direcory, traverse down ...*/
		else if (S_ISDIR(buf.st_mode) ){
		dfs(new_path, name);
		}
		
		/*If we find a file with the name, 
  		  print its path and continue ...*/
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

	int fd_in;
	int fd_header;
	int fd_file;

	if ((flag_c == NULL) || (flag_t == NULL)
		|| (flag_x == NULL) ||
		 (flag_v == NULL) || (flag_S == NULL))
	{
	perror("malloc");
	exit(EXIT_FAILURE);
	}
	
	
	/*Setting all flags to false ...*/
        *flag_c = 0;
        *flag_t = 0;
        *flag_x = 0;
        *flag_v = 0;
        *flag_S = 0;
		
/*	command_parser(flag_c, flag_t, flag_x, flag_v,
			 flag_S, argc, argv);*/

	/*Creating a new archive file ...*/	
	
	fd_in = open("Makefile",   O_RDONLY);
	fd_header =  open("test_header", O_RDWR);
	fd_file = open("test_header", O_RDWR);
	
	write_header("Makefile", NULL, fd_header);
	write_file(fd_in, fd_file);

	free(flag_c);
	free(flag_t);	
	free(flag_x);
	free(flag_v);
	free(flag_S);
	return 0;
}







