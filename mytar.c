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
# include <time.h>
# include <ctype.h>
# include <utime.h>

# include "special.c"
# include "print.c"
# include "oct.c"
# include "format.c"
# include "write.c"
# include "tree.c"
# include "check.c"
# include "list.c"

# define BLOCK_SIZE 512
# define CAP_8 2097151
# define CAP_12 8589934591



void get_name(char *file_name, uint8_t buf[BLOCK_SIZE]){
	int count;
	 /*File name w/ no prefix ...*/
        if (buf[345] == '\0'){
        for (count = 0; count < 100; count++){
        file_name[count] = buf[count];
	 if (buf[count] == '\0')
        break;
        }
        /*W/ prefix ...*/
        }else{
	char prefix[155];
	char name[100];	
	for (count = 345; count < 345 + 155; count++)
	prefix[count - 345] = buf[count];
	for (count = 0; count < 100; count++)
        name[count] = buf[count];	
	strcpy(file_name, prefix);
	strcat(file_name, "/");	
	strcat(file_name, name);
	}
	}

void zero_buf(uint8_t buf[BLOCK_SIZE]){
	int count;
	for (count = 0; count < BLOCK_SIZE; count++)
        buf[count] = 0;
}

void write_contents(char *file_name, int fd_tar, int size){

	uint8_t buf[BLOCK_SIZE];
	int dbs = size / BLOCK_SIZE; /*data blocks to be written ..*/
	int read_ret, write_ret;
	int count;
	int fd_out;
	if (size % BLOCK_SIZE > 0)
	dbs = dbs + 1;
	
	zero_buf(buf);
	
	fd_out = open(file_name, O_RDWR);
	if (fd_out < 0)
	perror("open");

	/*Archive is structured in blocks ...*/
  	read_ret = read(fd_tar, &buf, BLOCK_SIZE);		
	
	if (-1 == read_ret){
	perror("read");
	return;}
	
	for (count = 0; count < dbs; count++){

	if ((count == dbs - 1) && (size % BLOCK_SIZE > 0)){
	write_ret = write(fd_out, &buf, size % BLOCK_SIZE);
	break;}
	else
	write_ret = write(fd_out, &buf, read_ret);
	
	if (write_ret < 0)
	perror("write");

	read_ret = read(fd_tar, &buf, BLOCK_SIZE);
	
	}
}

/*For creating, we need size, type, name, permissions*/
/*Restores mtime !*/
void read_x(int fd_tar, int *flag_v, 
                int *flag_specific, int numb_s, char **specific_files)
{
	uint8_t buf[BLOCK_SIZE];
        int count;
        char *file_name = calloc(200, 1);
        char *size = calloc(12, 1);	
	char *typeflag = calloc(1, 1);
	char *mode = calloc(8, 1);
	char *mtime = calloc(8, 1);
	int read_ret, check, exec = 0, int_size = 0, mode_int = 0;
	int open_ret;		
	if (file_name == NULL || size == NULL || 
		typeflag == NULL || mode == NULL ||
		mtime == NULL){
        perror("calloc");
        exit(EXIT_FAILURE);}

	if (-1 == lseek(fd_tar, SEEK_SET, 0))
        perror("unable to read tar");
	zero_buf(buf);

	read_ret = read(fd_tar, &buf, BLOCK_SIZE);

	if (read_ret < 0){
        perror("read");
        return;
        }
        while (buf[0] != '\0' || read_ret == 0) {

       	get_name(file_name, buf);
	*typeflag = buf[156];

        check = file_check2(file_name, flag_specific, numb_s, specific_files);
	int_size = get_size(size, buf);	

	if (check == 1){
	/*Mode ..*/
        for (count = 100; count < 108; count++)
        mode[count - 100] = buf[count];
        mode_int = strtol(mode, NULL, 8);	

	if (*flag_v == 1)
	printf("%s\n", file_name);
	
	if (mode_int & S_IXUSR)
	exec = 1;
	if (mode_int & S_IXGRP)
        exec = 1;
	if (mode_int & S_IXOTH)
        exec = 1;
	/*If a regular file ...*/
	if (*typeflag == '0' || *typeflag == '\0'){
		if (exec == 1){
		open_ret = open(file_name, O_CREAT | O_TRUNC  | O_APPEND, 
			(S_IRUSR | S_IWUSR| S_IXUSR |
			 S_IRGRP | S_IWGRP | S_IXGRP |
			 S_IROTH | S_IWOTH | S_IXOTH) );	
		}
		else{
		open_ret = open(file_name, O_CREAT | O_TRUNC | O_APPEND,
                        (S_IRUSR | S_IWUSR| 
                         S_IRGRP | S_IWGRP |
			 S_IROTH | S_IWOTH));
		}
	}
	/*If a symlink ...*/
	else if (*typeflag == '2'){
		char *linkname = calloc(100, 1);
		
		for (count = 157; count < 257; count++)
		linkname[count - 157] = buf[count];

		open_ret = symlink(linkname ,file_name);

		free(linkname);
	}
	/*If a directory ...*/
	else if (*typeflag == '5'){	
		open_ret = mkdir(file_name, 
                        (S_IRUSR | S_IWUSR| S_IXUSR |
                         S_IRGRP | S_IWGRP | S_IXGRP |
                         S_IROTH | S_IWOTH | S_IXOTH) );	
	}
	if (open_ret < 0)
	printf("unable to extract %s\n", file_name);
	else{
	if (*typeflag != '0' && *typeflag != '\0')
	close(open_ret);}
	if (open_ret > 0 && check == 1 &&  
		(*typeflag == '0' || *typeflag == '\0'))
	write_contents(file_name, fd_tar, int_size);

	/*Restoring mtime ...*/
	if (open_ret >= 0){
	struct utimbuf time;
	int val = get_mtime(mtime, buf);

	if (localtime(&time.actime) == NULL)
	perror("time");
	time.modtime = val;
	if ( -1 == utime(file_name, &time))
	perror("utime");
	}
		
	
	if (-1 == close(open_ret))
	perror("close");
	if (check == 1)/*Getting next file ...*/
	read_ret = read(fd_tar, &buf, BLOCK_SIZE);
        if (read_ret < 0)
        perror("read");
	}
	/*Not interested in contents we are not extracting ...*/
	if (check != 1) {
	int jump, dbs;

	dbs = int_size / BLOCK_SIZE;
        if (int_size % BLOCK_SIZE != 0)
        dbs = dbs + 1;
        jump = dbs * BLOCK_SIZE;
        if (dbs != 0){
        if (-1 == lseek(fd_tar, jump , SEEK_CUR))
        perror("lseek");
        }

        read_ret = read(fd_tar, &buf, BLOCK_SIZE);
        if (read_ret < 0){
        perror("read");
        return;
	}
	
	}
	}
	free(file_name);
	free(mode);
	free(size);
	free(mtime);
	free(typeflag);		
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


int main(int argc, char *argv[]){
	
	int *flag_c = malloc(sizeof(int)); 
	int *flag_t = malloc(sizeof(int)); 
	int *flag_x = malloc(sizeof(int));
	int *flag_v = malloc(sizeof(int));
	int *flag_S = malloc(sizeof(int));
	int *flag_specific = malloc(sizeof(int));
	uint8_t *end_tar = calloc(2*BLOCK_SIZE, 1);
	int count;
	struct stat buf;	

	int fd_tar;
	int fd_in;

	if ((flag_c == NULL) || (flag_t == NULL)
		|| (flag_x == NULL) || (end_tar == NULL) ||
		 (flag_v == NULL) || (flag_S == NULL)
		|| (flag_specific == NULL))
	{
	perror("malloc");
	exit(EXIT_FAILURE);
	}
	*flag_specific = 0;
	
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
	int len = strlen(argv[count]);
	if (-1 == lstat(argv[count], &buf))
        perror("lstat");
	
	if (S_ISDIR(buf.st_mode) && argv[count][len - 1] != '/')
	write_header2(strcat(argv[count], "/"), NULL, fd_tar);
	else
	write_header2(argv[count], NULL, fd_tar);	

	if (*flag_v == 1)
	printf("%s\n", argv[count]);	

	/*If a dir, don't write file but traverse it ...*/
	if (S_ISDIR(buf.st_mode))
	dfs2(argv[count], NULL, fd_tar, flag_v);

	/*If a regular file, write file ...*/
	if (S_ISREG(buf.st_mode)){
	fd_in = open(argv[count],   O_RDONLY);
	if (fd_in == -1){
        perror(argv[count]);
        }
	write_file2(fd_in, fd_tar);}
	}

	/*Writing 2 NULL data blocks at end ...*/
	if (write(fd_tar, end_tar, 2*BLOCK_SIZE) != 2*BLOCK_SIZE)
	perror("write");
	free(end_tar);
	}
	

	else if(*flag_t == 1 || *flag_x == 1){
	char **specific_files;	
	/*If listing specific files ...*/
	if (argc>3){
	*flag_specific = 1;
	specific_files = malloc((argc - 3)*sizeof(char*));
	for (count = 3; count < argc ; count++)
	specific_files[count - 3] = argv[count];
	}

	fd_tar = open(argv[2], O_RDONLY);
	if (-1 == fd_tar){
	perror("could not open tar");	
	exit(EXIT_FAILURE);
	}
	
	check_tar2(argv[2], fd_tar);
	if (*flag_t == 1)
	read_t2(fd_tar, flag_v, flag_t, flag_specific, 
		argc - 3,  specific_files);
	else
	read_x(fd_tar, flag_v, flag_specific,
                argc - 3,  specific_files);
	}


		

	free(flag_c);
	free(flag_t);	
	free(flag_x);
	free(flag_v);
	free(flag_S);
	return 0;
}



