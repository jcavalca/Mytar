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

# include "define.h"
# include "share.c"
# include "special.c"
# include "print.c"
# include "oct.c"
# include "format.c"
# include "write.c"
# include "tree.c"
# include "check.c"
# include "list.c"
# include "extract.c"


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
	read_x2(fd_tar, flag_v, flag_specific,
                argc - 3,  specific_files);
	}


		

	free(flag_c);
	free(flag_t);	
	free(flag_x);
	free(flag_v);
	free(flag_S);
	return 0;
}



