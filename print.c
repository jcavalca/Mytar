/*This file contains usefuls functions for printing 
 * when listing a tar file .*/

# define BLOCK_SIZE 512

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

uint32_t print_size(char *size, uint8_t buf[BLOCK_SIZE]){

        uint32_t val;
        int count;
        for (count = 124; count < 136; count++)
        size[count - 124] = buf[count];
        if(size[0] & 0x80)
        val = extract_special_int(size, 12);
        else
        val = strtol(size, NULL, 8);
        printf( "%13d", val);
        printf(" ");    
        return val;
}

uint32_t get_size(char *size, uint8_t buf[BLOCK_SIZE]){
        uint32_t val;
        int count;
        for (count = 124; count < 136; count++)
        size[count - 124] = buf[count];
        if(size[0] & 0x80)
        val = extract_special_int(size, 12);
        else
        val = strtol(size, NULL, 8);
        return val;
}

uint32_t get_mtime(char *mtime, uint8_t buf[BLOCK_SIZE]){
        time_t val;
        int count;
        for (count = 136; count < 148; count++)
        mtime[count - 136] = buf[count];
        if(mtime[0] & 0x80)
        val = extract_special_int(mtime, 12);
        else
        val = strtol(mtime, NULL, 8);
	return val;

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



