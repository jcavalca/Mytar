/*This file contains functions that either write
 * the header or contents of a file to be 
 * archived in a tar. */
# include "define.h"


void write_header2(char *file, char *path, int fd_out){
        char *name = calloc(NAME_SIZE, 1); /*NUL-terminated if NUL fits*/
        char *mode = calloc(MODE_SIZE, 1);
        char *uid = calloc(UID_SIZE, 1);
        char *gid = calloc(GID_SIZE, 1);
        char *size = calloc(SIZE_SIZE, 1); /*Directories and symlinks are zero*/
        char *mtime = calloc(MTIME_SIZE, 1);
        char *chksum = calloc(CHKSUM_SIZE, 1);
        char *typeflag = calloc(TYPEFLAG_SIZE, 1);
        char *linkname = calloc(LINK_SIZE, 1);/*NUL-terminated if NUL fits*/
        char *magic = calloc(MAGIC_SIZE, 1);/*must ve "ustar", NUL-terminated*/
        char *version = calloc(VERSION_SIZE, 1);/*must be "00" (zero-zero) */
        char *uname = calloc(UNAME_SIZE, 1);/*NUL-terminated*/
        char *gname = calloc(GNAME_SIZE, 1);/*NUL-terminated*/
        char *devmajor = calloc(DEVMAJOR_SIZE, 1);
        char *devminor = calloc(DEVMINOR_SIZE, 1);
        char *prefix = calloc(PREFIX_SIZE, 1);/*NUL-terminated if NUL fits*/
        uint8_t *buf = calloc(BLOCK_SIZE, 1);
        int sum;
        int count = 0;

	if(name == NULL || mode == NULL || uid == NULL ||
	   gid == NULL || size == NULL || mtime == NULL ||
	   chksum == NULL || typeflag == NULL || linkname == NULL ||
	   magic == NULL || version == NULL || uname == NULL ||
	   gname == NULL || devmajor == NULL ||devminor == NULL ||
           prefix == NULL || buf == NULL){
	perror("calloc");
	exit(EXIT_FAILURE);
	}


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

        if (count < NAME_END)
        buf[count] = name[count];
        else if ( count < MODE_END)
        buf[count] = mode[count - NAME_END];
        else if ( count < UID_END)
	buf[count] = uid[count - MODE_END];
        else if ( count < GID_END){
        buf[count] = gid[count - UID_END];
        }else if ( count < SIZE_END)
        buf[count] = size[count - GID_END];
        else if ( count < MTIME_END)
        buf[count] = mtime[count - SIZE_END];
        else if ( count < CHKSUM_END)
        buf[count] = chksum[count - MTIME_END];
        else if ( count < TYPEFLAG_END)
        buf[count] = typeflag[count - CHKSUM_END];
        else if ( count < LINK_END)
        buf[count] = linkname[count - TYPEFLAG_END];
        else if ( count < MAGIC_END)
        buf[count] = magic[count - LINK_END];
        else if ( count < VERSION_END)
        buf[count] = version[count - MAGIC_END];
        else if ( count < UNAME_END)
        buf[count] = uname[count - VERSION_END];
        else if ( count < GNAME_END)
        buf[count] = gname[count - UNAME_END];
        else if ( count < DEVMAJOR_END){
        buf[count] = devmajor[count - GNAME_END];
        }else if ( count < DEVMINOR_END){
        buf[count] = devminor[count - DEVMAJOR_END];
        }else if (count < PREFIX_END)
        buf[count] = prefix[count - DEVMINOR_END];

        }
        sum = 0;
        /*Getting/writing chksum ...*/
        for (count = 0; count < BLOCK_SIZE; count++)
        sum = sum + buf[count];
        sum = sum + CHKSUM_SIZE*((uint8_t) ' '); /*Adding plus 8 spaces for 
                                                chksum*/
        strcpy(chksum, dec_to_stroct(sum, CHKSUM_SIZE));
        for (count = MTIME_END; count < CHKSUM_END; count++)
        buf[count] = chksum[count - MTIME_END];

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
