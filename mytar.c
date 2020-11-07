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
# include <ctype.h>
# include "print.c"
# include "oct.c"
# include "format.c"
# include "write.c"
# include "tree.c"
# define BLOCK_SIZE 512
# define CAP_8 2097151
# define CAP_12 8589934591



/*This checks if we should list a file ...*/
int file_check(char *file_name, int *flag_specific, 
		int num_s, char **specific_file){

	/*No specific files given ...*/
	if (*flag_specific == 0)
	return 1;	
	/*Checking if file is in the given ones ...*/
	else if (*flag_specific == 1){
	int count;
	for (count = 0; count < num_s; count++){
	if (strcmp(specific_file[count], file_name) == 0)
	return 1;
	}
	}
	/*Couldn't find it ...*/
	return 0;


}


void zero_buf(uint8_t buf[BLOCK_SIZE]){
	int count;
	for (count = 0; count < BLOCK_SIZE; count++)
        buf[count] = 0;
}

/*For listind we need name (if verbose is on,
 * 				 we also need size, perm, 
 * 				 user/grp and mtime)
*/
void read_list(int fd_tar, int *flag_v, int *flag_t, 
		int *flag_specific, int numb_s, char **specific_files)
{
	uint8_t buf[BLOCK_SIZE];
	int count;
	char *file_name = calloc(200, 1);
	char *size = calloc(12, 1);	
	char *mode; char *uname; char *gname;char *mtime;
	char *typeflag; 
	int mode_int;
	int dbs; /*# of data blocs to skip*/
	uint32_t int_size;	
	int read_ret;
	int jump;
	int check = 0;
	char linkname[100];
	typeflag = calloc(1,1);
	zero_buf(buf);

	if (file_name == NULL || size == NULL || typeflag == NULL){
        perror("calloc");
        exit(EXIT_FAILURE);}

	if (-1 == lseek(fd_tar, SEEK_SET, 0))
	perror("unable to read tar");
	
	read_ret = read(fd_tar, &buf, BLOCK_SIZE);

	while (buf[0] != '\0' || read_ret == 0)	{

	/*File name w/ no prefix ...*/	
	if (buf[345] == '\0'){
        for (count = 0; count < 100; count++){
        if (buf[count] == '\0')
        break;
        file_name[count] = buf[count];
        }	
	/*W/ prefix ...*/
        }else{
        int stop = 0;
         for (count = 0; buf[count + 345] != '\0'; count++){
        stop++;
        file_name[count] = buf[count + 345];}
        for (count = 0; count < 100; count++)
	if (count == 0 && buf[0] == '/')
	continue;	
	else{
        if (buf[count] == '\0')
        break;
        file_name[stop + count]  = buf[count];}
        }
	*typeflag = buf[156];
	
	check = file_check(file_name, flag_specific, numb_s, specific_files);
		
	if (*flag_v == 1 && *flag_t == 1){
	uname = calloc(8, 1);
	gname = calloc(8, 1);
	mtime = calloc(8, 1);
	mode = calloc(8, 1);
	if (uname == NULL || gname== NULL ||
	    mtime== NULL || typeflag == NULL){
	perror("malloc");
	exit(EXIT_FAILURE);}


	/*Type of file ...*/
	*typeflag = buf[156];
	if (check == 1)
	print_type(typeflag);
	/*Mode ..*/
	for (count = 100; count < 108; count++)
	mode[count - 100] = buf[count];
	mode_int = strtol(mode, NULL, 8);
	if (check == 1)
	print_perm(mode_int);
	/*Names*/
	if (check == 1)
	print_names(buf, uname, gname);
	/*Size*/
	if (check == 1)
	int_size = print_size(size, buf);
	/*mtime*/
	if (check == 1)
	print_mtime(mtime, buf);
	}
	int_size = get_size(size, buf);
	if (check == 1){
	printf("%s \n", file_name);
	}
	/*Jump data blocks of current file ...*/
	dbs = int_size / BLOCK_SIZE;
	if (int_size % BLOCK_SIZE != 0)
	dbs = dbs + 1;
	jump = dbs * BLOCK_SIZE;
	if (dbs != 0){	
	if (-1 == lseek(fd_tar, jump , SEEK_CUR))
	perror("lseek");	
	}

	read_ret = read(fd_tar, &buf, BLOCK_SIZE);
	}
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

void check_tar(char *file_tar, int fd_tar){
	struct stat buf;
	char read_buf[BLOCK_SIZE];
	if (-1 == lstat(file_tar, &buf)){
	perror("lstat");
	exit(EXIT_FAILURE);
	}

	if (-1 == read(fd_tar, read_buf, BLOCK_SIZE)){
	perror("read");
	 exit(EXIT_FAILURE);
	}		

	if (S_ISREG(buf.st_mode) || 	 /*tar should be reg. files*/
	buf.st_size % BLOCK_SIZE == 0 || /*tar should have integer # blocks*/
	buf.st_size != 0 ||		 /*tar should be non=empty*/
	isalpha(read_buf[0]) != 0)
	return;
	else{
	perror("malformed tar file");
        exit(EXIT_FAILURE);
	}
	if (-1 == lseek(fd_tar, 0, SEEK_SET)){
        perror("lstat");
        exit(EXIT_FAILURE);
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
	write_header2(argv[count], NULL, fd_tar);	
	if (-1 == lstat(argv[count], &buf))
	perror("lstat");
	if (*flag_v == 1)
	printf("%s\n", argv[count]);	
	/*If a dir, don't write file but traverse it ...*/
	if (S_ISDIR(buf.st_mode))
	dfs2(argv[count], fd_tar, flag_v);
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
	

	else if(*flag_t == 1){
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
	
	check_tar(argv[2], fd_tar);
	read_list(fd_tar, flag_v, flag_t, flag_specific, 
		argc - 3,  specific_files);
	}


		

	free(flag_c);
	free(flag_t);	
	free(flag_x);
	free(flag_v);
	free(flag_S);
	return 0;
}



