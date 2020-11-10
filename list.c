/*This file contains the implementation of the t flag ...*/


/*For listing we need name (if verbose is on,
                               we also need size, perm, 
                               user/grp and mtime)
 */
void read_t2(int fd_tar, int *flag_v, int *flag_t,
                int *flag_specific, int numb_s, char **specific_files)
{
        uint8_t buf[BLOCK_SIZE];
        int count;
        char *file_name = calloc(NAME_CAP + PREFIX_CAP, 1);
        char *size = calloc(12, 1);
        char *mode; char *uname; char *gname;char *mtime;
        char *typeflag;
        int mode_int;
        int dbs; /*# of data blocs to skip*/
        uint32_t int_size;
        int read_ret;
        int jump;
        int check = 0;
        int len;
        typeflag = calloc(1,1);
        zero_buf(buf);

        if (file_name == NULL || size == NULL || typeflag == NULL){
        perror("calloc");
        exit(EXIT_FAILURE);}

        if (-1 == lseek(fd_tar, SEEK_SET, 0))
        perror("unable to read tar");

        read_ret = read(fd_tar, &buf, BLOCK_SIZE);
        if (read_ret < 0){
        perror("read");
        return;
        }
        while (buf[0] != '\0' || read_ret == 0) {

        get_name(file_name, buf);
        len = strlen(file_name);
        if (file_name[len - 1] == '/' && file_name[len - 2] == '/')
        file_name[len -1] = '\0';

        *typeflag = buf[156];

        check = file_check2(file_name, flag_specific, numb_s, specific_files);

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
        if (read_ret < 0){
        perror("read");
        return;
        }
        }
         if (*flag_v == 1 && *flag_t == 1){
        free(uname);
        free(gname);
        free(mtime);
        free(mode);

        }
        free(size);
        free(typeflag);
        free(file_name);
}

