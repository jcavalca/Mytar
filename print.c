/*This file contains usefuls functions for printing 
 * when listing a tar file .*/

# include "define.h"

void print_type(char *typeflag){
if (*typeflag == TYPE_FILE || *typeflag == TYPE_FILE_AL)
        printf("-");
        else if (*typeflag == TYPE_LINK)
        printf("l");
        else if (*typeflag == TYPE_DIR)
        printf("d");
}

void print_names(uint8_t buf[BLOCK_SIZE], char *uname, char *gname){

        int count;
        for (count = UNAME_START; count < UNAME_STOP; count++){
        if (buf[count] == '\0')
        break;
        uname[count - UNAME_START] = buf[count];
        }

        printf("%s/", uname);
        for (count = GNAME_START; count < GNAME_STOP; count++){
        if (buf[count] == '\0')
        break;
        gname[count - GNAME_START] = buf[count];
        }
        printf("%s", gname);
        printf("  ");
}

uint32_t print_size(char *size, uint8_t buf[BLOCK_SIZE]){

        uint32_t val;
        int count;
        for (count = SIZE_START; count < SIZE_STOP; count++)
        size[count - SIZE_START] = buf[count];
        if(size[0] & 0x80)
        val = extract_special_int(size, SIZE_STOP - SIZE_START);
        else
        val = strtol(size, NULL, 8);
        printf( "%13d", val);
        printf(" ");    
        return val;
}

uint32_t get_size(char *size, uint8_t buf[BLOCK_SIZE]){
        uint32_t val;
        int count;
        for (count = SIZE_START; count < SIZE_STOP; count++)
        size[count - SIZE_START] = buf[count];
        if(size[0] & 0x80)
        val = extract_special_int(size, SIZE_STOP - SIZE_START);
        else
        val = strtol(size, NULL, 8);
        return val;
}

uint32_t get_mtime(char *mtime, uint8_t buf[BLOCK_SIZE]){
        time_t val;
        int count;
        for (count = MTIME_START; count < MTIME_STOP; count++)
        mtime[count - MTIME_START] = buf[count];
        if(mtime[0] & 0x80)
        val = extract_special_int(mtime,MTIME_STOP- MTIME_START);
        else
        val = strtol(mtime, NULL, 8);
	return val;

}

void print_mtime(char *mtime, uint8_t buf[BLOCK_SIZE]){
        const struct tm *tm;
        time_t val;
        int count;
        char out[20];
        for (count = MTIME_START; count < MTIME_STOP; count++)
        mtime[count - MTIME_START] = buf[count];
        if(mtime[0] & 0x80)
        val = extract_special_int(mtime, MTIME_STOP- MTIME_START);
        else
        val = strtol(mtime, NULL, 8);
        tm = localtime(&val);
        if (tm != NULL)
        strftime(out, 20, "%Y-%m-%d %H:%M", tm);
        printf("%s ", out);
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



