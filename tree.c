/*This file contains a function that does a dfs 
 * on a directory given when creating a tar archive*/

int dfs2(char *path, int fd_tar, int *flag_v){
        DIR *dp;
        struct dirent *entry;
        struct stat buf; /*used for checking entries*/
        char *test = malloc(100);
        /*Report error and don't stop ...*/
        if(chdir(path) != 0){
        perror(path);
        return 0;
        }
        free(test);
        /*Opening directory specified*/
        dp = opendir(".");
        if (dp == NULL){
        perror("opendir");
        return 0;
        }

        while ( (entry = readdir(dp)) != NULL){

                /*Not intereste in traversing up, nor 
 *                   getting stuck in a loop ... */
                if (strcmp(entry -> d_name, ".") != 0 &&
                  strcmp(entry -> d_name, "..") != 0){

                /*Strcpy was giving me a hard time ...*/
                char *new_path = calloc(PATH_MAX, 1);
                int count = 0;
                int fd_in ;
                for (count = 0; count < strlen(path); count++)
                new_path[count] = path[count];
                new_path[count] = '\0';
                if (new_path[count-1] != '/')
                strcat(new_path, "/");
                strcat(new_path , entry ->d_name);
                if (*flag_v == 1)
                printf("%s\n", new_path);
                if (stat(entry -> d_name, &buf) != 0){
                        perror("stat");
                        continue;
                           }
                /*Interested in symlinks ...*/
                else if (S_ISLNK(buf.st_mode))
                write_header2(new_path, NULL, fd_tar);
                /*If a direcory, traverse down ...*/
                else if (S_ISDIR(buf.st_mode) ){
                write_header2(entry -> d_name, path, fd_tar);
                dfs2(entry -> d_name, fd_tar, flag_v);
                }

                /*If we find a file with the name, 
 *                   print its path and continue ...*/
                else if (S_ISREG(buf.st_mode)){
                write_header2(entry -> d_name, path, fd_tar);
                fd_in = open(entry -> d_name,   O_RDONLY);
                if (fd_in == -1)
                perror("open");
                write_file2(fd_in, fd_tar);
                /*Call header*/
                }
                /* free(new_path);*/
                }
        }
        closedir(dp);
        return 0;
}

