/*This file contains functions that either write
 * the header or contents of a file to be 
 * archived in a tar. */

void write_header2(char *file, char *path, int fd_out){
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
        if(format_name_prefix2(name, prefix, file, path) == -1){
        perror("format name");
        return;
        }

        /*If unable to format mode, ignore file ...*/
        if(format_from_lstat2(file, path, mode, uid, gid, size,
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


int write_file2(int fd_in, int fd_out){

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
