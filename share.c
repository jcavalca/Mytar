/*This file contains function used by both
 listing and extraction implementations*/


/*Gets name of file, dealing with both its
 *  "name" and "prefix". */
void get_name(char *file_name, uint8_t buf[BLOCK_SIZE]){
        int count;
         /*File name w/ no prefix ...*/
        if (buf[PREFIX_START] == '\0'){
        for (count = 0; count < NAME_CAP; count++){
        file_name[count] = buf[count];
         if (buf[count] == '\0')
        break;
        }
        /*W/ prefix ...*/
        }else{
        char prefix[PREFIX_CAP];
        char name[NAME_CAP];
        for (count = PREFIX_START; count < PREFIX_START + PREFIX_CAP; count++)
        prefix[count - PREFIX_START] = buf[count];
        for (count = 0; count < NAME_CAP; count++)
        name[count] = buf[count];
        strcpy(file_name, prefix);
        strcat(file_name, "/");
        strcat(file_name, name);
        }
        }

/*Zeroes entire buffer of known size*/
void zero_buf(uint8_t buf[BLOCK_SIZE]){
        int count;
        for (count = 0; count < BLOCK_SIZE; count++)
        buf[count] = 0;
}

