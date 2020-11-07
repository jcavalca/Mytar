/*This file includes functions that performs
 * important checks ... */


/*This checks for valid tar archive file ...*/
void check_tar2(char *file_tar, int fd_tar){
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

        if (S_ISREG(buf.st_mode) ||      /*tar should be reg. files*/
        buf.st_size % BLOCK_SIZE == 0 || /*tar should have integer # blocks*/
        buf.st_size != 0 ||              /*tar should be non=empty*/
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




/*This checks if we should list a file ...*/
int file_check2(char *file_name, int *flag_specific,
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


