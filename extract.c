/*This file implements the x flag of ustar*/




/*Write contents of regular files that are 
  extracted.*/
void write_contents2(char *file_name, int fd_tar, int size){

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
        if (count != dbs - 1)
        read_ret = read(fd_tar, &buf, BLOCK_SIZE);

        }
}

/*If given a file to extract, this function
  creates all non existent parent directories
  in the path of the file. */
void parent_dirs2(char *file_name){
        int count, inner_count;
        int len = strlen(file_name);
        char *parent = calloc(len, 1);
        struct stat buf;
        if (parent == NULL){
        perror("calloc");
        exit(EXIT_FAILURE);
        }

        for (count = 0; count < len; count++){
        if (count != len - 1 && file_name[count] == '/'){
                for (inner_count = 0; inner_count < count; inner_count++)
                parent[inner_count] = file_name[inner_count];
                /*Is parent dir doesn't exist*/
                if (-1 == lstat(parent, &buf)){

                if (-1 == mkdir(parent,
                         (S_IRUSR | S_IWUSR| S_IXUSR |
                         S_IRGRP | S_IWGRP | S_IXGRP |
                         S_IROTH | S_IWOTH | S_IXOTH) )){
                perror("mkdir parent");}
                }


        }
        }
        free(parent);
}

/*For creating, we need size, type, name, permissions*/
/*Restores mtime !*/
void read_x2(int fd_tar, int *flag_v,
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
        struct stat test;
        int dir_open = 1;
        int was_present = 0;
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
        was_present = 0;
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
                if (-1 == lstat(file_name, &test)){
                was_present = 0;
                parent_dirs2(file_name);
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
                else{was_present = 1;}
        }
        /*If a symlink ...*/
        else if (*typeflag == '2'){
                parent_dirs2(file_name);
                if (-1 == lstat(file_name, &test)){
                char *linkname = calloc(100, 1);

                for (count = 157; count < 257; count++)
                linkname[count - 157] = buf[count];

                open_ret = symlink(linkname ,file_name);

                free(linkname);}
                else{was_present = 1;}
        }
        /*If a directory ...*/
        else if (*typeflag == '5'){
                parent_dirs2(file_name);
                if (-1 == lstat(file_name, &test)){
                was_present = 0;
                dir_open = 1;
                if (exec == 1)
                open_ret = mkdir(file_name,
                        (S_IRUSR | S_IWUSR| S_IXUSR |
                         S_IRGRP | S_IWGRP | S_IXGRP |
                         S_IROTH | S_IWOTH | S_IXOTH) );
                else
                open_ret =  mkdir(file_name,
                        (S_IRUSR | S_IWUSR|
                         S_IRGRP | S_IWGRP |
                         S_IROTH | S_IWOTH));
                }
                else{
                open_ret = 1;
                dir_open = 0;
                was_present = 1;
                }
        }


        if ((open_ret < 0 && *typeflag != '5' && was_present == 0 ))
        perror("unable to extract");
        else if (!(*typeflag == '5' && dir_open == 0)  && was_present == 0){
        if (*typeflag != '0' && *typeflag != '\0')
	     close(open_ret);}
        if (open_ret >= 0 && check == 1 &&
                (*typeflag == '0' || *typeflag == '\0')
                 && was_present == 0)
        write_contents2(file_name, fd_tar, int_size);



        /*Restoring mtime ...*/
        if (open_ret >= 0 &&
                !(*typeflag == '5')
                 && was_present == 0
        ){
        struct utimbuf time;
        int val = get_mtime(mtime, buf);

        if (localtime(&time.actime) == NULL)
        perror("time");
        time.modtime = val;
        if ( -1 == utime(file_name, &time))
        perror("utime");
        }
        if (check == 1  && was_present == 0)/*Getting next file ...*/
        read_ret = read(fd_tar, &buf, BLOCK_SIZE);
        if (read_ret < 0)
        perror("read");
        }
        /*Not interested in contents we are not extracting ...*/
        if (check != 1 || was_present == 1)  {
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

