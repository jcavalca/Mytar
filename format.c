/*This file has two formatting functions used 
 * for formatting the headers specs according to 
 * the std - used for creating tar archive*/
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>

int format_name_prefix2(char *name, char *prefix,
                                char *file, char *path){

        int len_path;
        if (path == NULL)
        len_path = 0;
        else if(path[0] == '\0')
        len_path = 0;
        else
        len_path = strlen(path);
        /*If everything fits in name ...*/
        if (strlen(file) + len_path  <= NAME_CAP){
        
        int count = 0;
        
        for (count = 0; count < len_path; count++)
        name[count] = path[count];
        name[count] = '\0';
        if (len_path != 0)
        if (name[count - 1] != '/')
        strcat(name, "/");
        strcat(name, file);
        return 0;
        }

        /*If name is full ...*/
        else if (strlen(file) == NAME_CAP){
        if (len_path == 0){
        strcpy(name, file);
        return 0;
        }
        else if(len_path >= NAME_CAP)
        return -1;
        else{
        strcpy(name, file);
        strcpy(prefix, path);
        if (path[len_path] != '/')
        strcat(prefix, "/");
        return 0;
        }
        }

        else{
        /*If file name is too big, there isn't a way to break it...*/
        if(strlen(file) > NAME_CAP){
                return -1;
        }
	/*If path and file name is too big, try to break path ...*/
        else if(len_path + strlen(file) > NAME_CAP){
        int count = 0;
        int break_point;
        int success = 0;
        int name_count = 0;
        for (count = 0; count < len_path; count++){

        if (path[count] == '/'){
                if (len_path + strlen(file) +  - count <= NAME_CAP){
                success = 1;
                break_point = count;
		break;
                }
                }
        }
        /*If unable to break ...*/
        if (success == 0)
        return -1;
        /*If able to break ...*/
                /*Put a little path on name*/
        for (count = 0; count < len_path; count++){
        if (break_point >= count){
	if(break_point > count)	/*Not putting slash into prefix*/
        prefix[count] = path[count];
        }else{
        name[name_count] = path[count];
        name_count++;
        }
        }
        if (name[name_count -1] != '/' && name[name_count] != '/'){
        name[name_count] = '/';
        name_count++;}
                /*Put file name on name*/
        for (count = 0; count < strlen(name); count++){
        name[name_count] = file[count];
        name_count++;
        }
        return 1;

        }
        }
        return 1;
}


int format_from_lstat2(char *file, char *path, char *mode,
                        char *uid, char *gid, char *size,
                        char *mtime, char *typeflag,
                        char *devmajor, char *devminor,
                        char *linkname, char *uname,
                        char *gname){
        struct stat buf;
        int perm = 0;
        char conversions[8] = {'0', '1', '2', '3', '4', '5', '6', '7'};
        char *ret;
        struct passwd *pass;
        struct group *group;


        if (-1 == lstat(file, &buf)){
        perror("lstat");
        return -1;
        }

        /*Getting Mode ...*/
                /*Padding with leading zeros and terminating NULL ...*/
        mode[0] = '0';
        mode[1] = '0';
        mode[2] = '0';
        mode[7] = '\0';
                /*UID and GID and sticky bit ...*/
        if (S_ISUID & buf.st_mode)
                perm = perm + 4;
        if (S_ISGID & buf.st_mode)
                perm = perm + 2;
        if (S_ISVTX & buf.st_mode)
                perm = perm + 1;
        mode[3] = conversions[perm];
        perm = 0;
                /*User permissions ...*/
        if (S_IRUSR & buf.st_mode){
        perm = perm + 4; }
        if (S_IWUSR & buf.st_mode)
        perm = perm + 2;
        if (S_IXUSR & buf.st_mode)
        perm = perm + 1;
        mode[4] = conversions[perm];
        perm = 0;
                /*Group permissions ...*/
        if (S_IRGRP & buf.st_mode){
        perm = perm + 4; }
        if (S_IWGRP & buf.st_mode)
        perm = perm + 2;
        if (S_IXGRP & buf.st_mode)
	        perm = perm + 1;
        mode[5] = conversions[perm];
        perm = 0;
                /*Other permissions ...*/
        if (S_IROTH & buf.st_mode){
        perm = perm + 4; }
        if (S_IWOTH & buf.st_mode)
        perm = perm + 2;
        if (S_IXOTH & buf.st_mode)
        perm = perm + 1;
        mode[6] = conversions[perm];
        perm = 0;

        /*Getting UID and GID values ...*/
        if (non_conforming_check((int) buf.st_uid, CAP_8)){
        insert_special_int(uid, 8, (int32_t) buf.st_uid);
        }else{
        ret = dec_to_stroct(buf.st_uid, 8);
        strcpy(uid, ret);
        free(ret);}
         if (non_conforming_check((int) buf.st_gid, CAP_8)){
        insert_special_int(gid, 8, (int32_t) buf.st_gid);
        }else{
        ret = dec_to_stroct(buf.st_gid, 8);
        strcpy(gid, ret);
        free(ret);}

        /*Uname and Gname ...*/
        pass = getpwuid(buf.st_uid);
        if (pass != NULL)
        strcpy(uname, pass -> pw_name);
        group = getgrgid(buf.st_gid);
        if (group != NULL)
        strcpy(gname, group -> gr_name);

        /*Getting size ...*/
        if (S_ISREG(buf.st_mode)){
					/*Cap for 12 bits octal ... throws error
 * 						when implemented as a macro*/
                if (non_conforming_check((int) buf.st_size, 8589934591))
                insert_special_int(size, 8, (int32_t) buf.st_size);
                else{
                ret = dec_to_stroct(buf.st_size, 12);
                strcpy(size, ret);
                free(ret);}
        }
        else{size = strcpy(size, "00000000000");}

        /*Getting mtime ...*/
        ret = dec_to_stroct(buf.st_mtime, 12);
                strcpy(mtime, ret);
        free(ret);

        /*Getting typeflag ...*/
        if (S_ISDIR(buf.st_mode))
        *typeflag = '5';
        else if (S_ISLNK(buf.st_mode))
        *typeflag = '2';
        else if (S_ISREG(buf.st_mode)){
/*NOTE:: Missing to include regular file alternate*/
        *typeflag = '0';
        }


        /*If link ...*/
        if (S_ISLNK(buf.st_mode)){
        if (-1 == readlink(file, linkname, NAME_CAP))
        perror("return readlink");
        return -1;

        }


        return 0;
}

