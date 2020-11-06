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
# include <grp.h>
# include <arpa/inet.h>
# include "special.c"
# include <time.h>


# define BLOCK_SIZE 512
# define CAP_8 2097151
# define CAP_12 8589934591


int format_name_prefix(char *name, char *prefix, 
				char *file, char *path){

	int len_path;
	if (path == NULL)
	len_path = 0;
	else if(path[0] == '\0')
	len_path = 0;
	else
	len_path = strlen(path);
	/*If everything fits in name ...*/
	if (strlen(file) + len_path + 1 <= 100){

	int count = 0;
	
        for (count = 0; count < len_path; count++)
        name[count] = path[count];
        name[count] = '\0';
	if (len_path != 0)
	if (name[count -1] != '/')
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
	if (name[name_count -1] != '/')
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

int non_conforming_check(int numb, uint64_t cap){
if (numb > cap)
return -1;
else
return 0;

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
	struct passwd *pass;
	struct group *group;


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
	if (non_conforming_check((int) buf.st_uid, CAP_8)){
	insert_special_int(uid, 8, (int32_t) buf.st_uid);
	}else{
	ret = dec_to_stroct(buf.st_uid, 8);
	strcpy(uid, ret);
	free(ret);}
	 if (non_conforming_check((int) buf.st_gid, CAP_8)){
        insert_special_int(gid, 8, (int32_t) buf.st_gid);
        }else{
	ret = dec_to_stroct(buf.st_gid, 8);
	strcpy(gid, ret);
        free(ret);}
	
	/*Uname and Gname ...*/
	pass = getpwuid(buf.st_uid);
	if (pass != NULL)
	strcpy(uname, pass -> pw_name);
	group = getgrgid(buf.st_gid);
	if (group != NULL)
        strcpy(gname, group -> gr_name);

	/*Getting size ...*/
	if (S_ISREG(buf.st_mode)){	

		if (non_conforming_check((int) buf.st_size, 8589934591))
		insert_special_int(size, 8, (int32_t) buf.st_size);
		else{
		ret = dec_to_stroct(buf.st_size, 12);
		strcpy(size, ret);
		free(ret);}
	}
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
	char *chksum = calloc(8, 1);
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
	int sum;
	int count = 0;
	/*If unable to format name, ignore file ...*/
	if(format_name_prefix(name, prefix, file, path) == -1){
	perror("format name");
	return;
	}

	/*If unable to format mode, ignore file ...*/
	if(format_from_lstat(file, path, mode, uid, gid, size, 
				mtime, typeflag, devmajor, 
				devminor, linkname, uname, gname) == -1){
	perror("format from lstat");
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
	else if ( count < 124){
	buf[count] = gid[count - 116];
	}else if ( count < 136)
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
	else if ( count < 337){
	buf[count] = devmajor[count - 329];
	}else if ( count < 345){
	buf[count] = devminor[count - 337];
	}else if (count < 510)
	buf[count] = prefix[count - 345];

	}
	sum = 0;	
	/*Getting/writing chksum ...*/
	for (count = 0; count < BLOCK_SIZE; count++)
	sum = sum + buf[count];
	sum = sum + 8*((uint8_t) ' '); /*Adding plus 8 spaces for 
						chksum*/
	strcpy(chksum, dec_to_stroct(sum, 8));
	for (count = 148; count < 156; count++)
	buf[count] = chksum[count - 148];

	if (write(fd_out, buf, BLOCK_SIZE) != BLOCK_SIZE){
	perror("write");	
	exit(EXIT_FAILURE);
	}
}

int write_file(int fd_in, int fd_out){

	uint8_t buf[BLOCK_SIZE];
	int ret; 
	int count;
	        
	for (count = 0; count<BLOCK_SIZE; count++)
	buf[count] = 0;

	ret =  read(fd_in, &buf, BLOCK_SIZE);
	while (ret == BLOCK_SIZE){
	if (write(fd_out, &buf,  BLOCK_SIZE) !=  BLOCK_SIZE){
	perror("read");
	return -1;
        }
	for (count = 0; count<BLOCK_SIZE; count++)
        buf[count] = 0;
	ret = read(fd_in, &buf, BLOCK_SIZE);
	}
	if (ret == -1){
        perror("read");
	return -1;}
	/*Writing left-overs ...*/
	if (ret < BLOCK_SIZE && ret != 0){
	if (write(fd_out, &buf,  BLOCK_SIZE) !=  BLOCK_SIZE){
        perror("read");
        return -1;
        }}
	return 0;	

}

/*Use stat*/
int dfs(char *path, int fd_tar, int *flag_v){
	DIR *dp;
	struct dirent *entry;
	struct stat buf; /*used for checking entries*/
	char *test = malloc(100);	
	/*Report error and don't stop ...*/
	if(chdir(path) != 0){
	perror(path);
	return 0;
	}
	free(test);
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
		int fd_in ;
		for (count = 0; count < strlen(path); count++)
		new_path[count] = path[count];
		new_path[count] = '\0';
		strcat(new_path, "/");
                strcat(new_path , entry ->d_name);
		if (*flag_v == 1)
		printf("%s\n", new_path);
		if (stat(entry -> d_name, &buf) != 0){
                        perror("stat");
                     	continue;
			   }
		/*Interested in symlinks ...*/
		else if (S_ISLNK(buf.st_mode))
		write_header(new_path, NULL, fd_tar);
		/*If a direcory, traverse down ...*/
		else if (S_ISDIR(buf.st_mode) ){
		write_header(entry -> d_name, path, fd_tar);
		dfs(entry -> d_name, fd_tar, flag_v);
		}
		
		/*If we find a file with the name, 
  		  print its path and continue ...*/
		else if (S_ISREG(buf.st_mode)){	
		write_header(entry -> d_name, path, fd_tar);
		fd_in = open(entry -> d_name,   O_RDONLY);
                if (fd_in == -1)
                perror("open");
		write_file(fd_in, fd_tar);	
		/*Call header*/	
		}
		/* free(new_path);*/
		}
	}
	closedir(dp);
	return 0;
}


void print_perm(int mode_int){

 	if (mode_int & S_IRUSR)
        printf("r");
        else
        printf("-");
        if(mode_int & S_IWUSR)
        printf("w");
        else
        printf("-");
        if(mode_int & S_IXUSR)
        printf("x");
        else
        printf("-");

        if (mode_int & S_IRGRP)
        printf("r");
        else
        printf("-");
        if(mode_int & S_IWGRP)
        printf("w");
        else
        printf("-");
        if(mode_int & S_IXGRP)
        printf("x");
        else
        printf("-");

         if (mode_int & S_IROTH)
        printf("r");
        else
        printf("-");
        if(mode_int & S_IWOTH)
        printf("w");
        else
        printf("-");
        if(mode_int & S_IXOTH)
        printf("x");
        else
        printf("-");

        printf(" ");
}

void print_type(char *typeflag){
if (*typeflag == '0' || *typeflag == '\0')
        printf("-");
        else if (*typeflag == '2')
        printf("l");
        else if (*typeflag == '5')
        printf("d");
}

void print_names(uint8_t buf[BLOCK_SIZE], char *uname, char *gname){
	
	int count;
	for (count = 265; count < 273; count++){
        if (buf[count] == '\0')
        break;
        uname[count - 265] = buf[count];
        }

        printf("%s/", uname);
        for (count = 297; count < 305; count++){
        if (buf[count] == '\0')
        break;
        gname[count - 297] = buf[count];
        }
        printf("%s", gname);
	printf("  ");
}

void print_size(char *size, uint8_t buf[BLOCK_SIZE]){

	int32_t val;
	int count;
  	for (count = 124; count < 136; count++)
        size[count - 124] = buf[count];
  	if(size[0] & 0x80)
 	val = extract_special_int(size, 12);
        else
        val = strtol(size, NULL, 8);
        printf( "%13d", val);
	printf(" ");	

}

void print_mtime(char *mtime, uint8_t buf[BLOCK_SIZE]){
	const struct tm *tm;
	time_t val;
        int count;
       	char out[20];
	for (count = 136; count < 148; count++)
        mtime[count - 136] = buf[count];
        if(mtime[0] & 0x80)
        val = extract_special_int(mtime, 12);
        else
        val = strtol(mtime, NULL, 8);	
	tm = localtime(&val);
	if (tm != NULL)
	strftime(out, 20, "%Y-%m-%d %H:%M", tm);
	printf("%s ", out);


}

/*For listind we need name (if verbose is on,
 * 				 we also need size, perm, 
 * 				 user/grp and mtime)
*/
void read_list(int fd_tar, int *flag_v, int *flag_t)
{
	uint8_t buf[BLOCK_SIZE];
	int count;
	char *file_name = calloc(200, 1);
	char *mode;	
	char *uname;
	char *gname;
	char *mtime;
	char *typeflag;
	char *size;
	int mode_int;

	if (-1 == lseek(fd_tar, SEEK_SET, 0))
	perror("unable to read tar");

	read(fd_tar, &buf, BLOCK_SIZE);
	
	if (*flag_v == 1 && *flag_t == 1){
	uname = calloc(8, 1);
	gname = calloc(8, 1);
	mtime = calloc(8, 1);
	typeflag = calloc(1,1);
	mode = calloc(8, 1);
	size = calloc(12, 1);
	if (uname == NULL || gname== NULL ||
	    mtime== NULL || typeflag == NULL){
	perror("malloc");
	exit(EXIT_FAILURE);}

	/*Type of file ...*/
	*typeflag = buf[156];
	print_type(typeflag);
	/*Mode ..*/
	for (count = 100; count < 108; count++)
	mode[count - 100] = buf[count];
	mode_int = strtol(mode, NULL, 8);
	print_perm(mode_int);
	/*Names*/
	print_names(buf, uname, gname);
	/*Size*/
	print_size(size, buf);
	/*mtime*/
	print_mtime(mtime, buf);
	}

	if (file_name == NULL){
	perror("malloc");
        exit(EXIT_FAILURE);}

	/*No prefix ...*/
	if (buf[345] == '\0'){
	for (count = 0; count < 100; count++)
	file_name[count] = buf[count];
	
	/*Has prefix ...*/
	}else{
	int stop = 0;
	 for (count = 0; buf[count + 345] != '\0'; count++){
	stop++;
        file_name[count] = buf[count + 345];}
	 for (count = 0; count < 100; count++)
        file_name[stop + count]  = buf[count];
	}
	printf("%s \n", file_name);


	free(file_name);	
}


void command_parser( int *flag_c, int *flag_t, int *flag_x, int *flag_v, 
			int *flag_S, int argc, char *argv[]){
	int count;
	if (argc < 3){
        perror("Usage: mytar [ctcvS]f tarfile [ path [ ... ] ]");
        exit(EXIT_FAILURE);
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
		printf("unknown flag: %c\n", argv[1][count]);	

	}
}

void check_tar(char *file_tar){
	struct stat buf;
	if (-1 == lstat(file_tar, &buf)){
	perror("lstat");
	exit(EXIT_FAILURE);
	}
	
	if (S_ISREG(buf.st_mode) || 	 /*tar should be reg. files*/
	buf.st_size % BLOCK_SIZE == 0 || /*tar should have integer # blocks*/
	buf.st_size == 0)		 /*tar should be non=empty*/
	return;
	else{
	perror("malformed tar file");
        exit(EXIT_FAILURE);
	}
	

}



int main(int argc, char *argv[]){
	
	int *flag_c = malloc(sizeof(int)); 
	int *flag_t = malloc(sizeof(int)); 
	int *flag_x = malloc(sizeof(int));
	int *flag_v = malloc(sizeof(int));
	int *flag_S = malloc(sizeof(int));
	uint8_t *end_tar = calloc(2*BLOCK_SIZE, 1);
	int count;
	struct stat buf;	

	int fd_tar;
	int fd_in;

	if ((flag_c == NULL) || (flag_t == NULL)
		|| (flag_x == NULL) || (end_tar == NULL) ||
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
		
	command_parser(flag_c, flag_t, flag_x, flag_v,
			 flag_S, argc, argv);

	/*If we are not creating or listing or extracting, 
 	  what's the point ...*/
	if (*flag_c == 0  && 
	    *flag_t == 0  && 
	    *flag_x == 0){
	perror("Usage: mytar [ctcvS]f tarfile [ path [ ... ] ]");
	exit(EXIT_FAILURE);
	}

	/*Creating a new archive file ...*/	
	if (*flag_c == 1){

	 fd_tar = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,
                 (S_IWUSR | S_IXUSR| S_IRUSR | S_IROTH | S_IXOTH | S_IWOTH) );
        if (fd_tar == -1){
        perror("couldn't create tar file");
        exit(EXIT_FAILURE);
        }



	/*Writing all arguments ...*/
	for (count = 3; count < argc; count++){
	write_header(argv[count], NULL, fd_tar);	
	if (-1 == lstat(argv[count], &buf))
	perror("lstat");
	if (*flag_v == 1)
	printf("%s\n", argv[count]);	
	/*If a dir, don't write file but traverse it ...*/
	if (S_ISDIR(buf.st_mode))
	dfs(argv[count], fd_tar, flag_v);
	/*If a regular file, write file ...*/
	if (S_ISREG(buf.st_mode)){
	fd_in = open(argv[count],   O_RDONLY);
	if (fd_in == -1){
        perror(argv[count]);
        }
	write_file(fd_in, fd_tar);}
	}

	/*Writing 2 NULL data blocks at end ...*/
	if (write(fd_tar, end_tar, 2*BLOCK_SIZE) != 2*BLOCK_SIZE)
	perror("write");
	free(end_tar);
	}
	
	else if(*flag_t == 1){
	fd_tar = open(argv[2], O_RDONLY);
	if (-1 == fd_tar){
	perror("could not open tar");	
	exit(EXIT_FAILURE);
	}
	
	check_tar(argv[2]);
	read_list(fd_tar, flag_v, flag_t);
	}


		

	free(flag_c);
	free(flag_t);	
	free(flag_x);
	free(flag_v);
	free(flag_S);
	return 0;
}







