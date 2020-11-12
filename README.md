# Mytar


This program in an implementation of an ancient UNIX program called Tar(1), which consists of a file archiving tool. A typical example of an important application of Tar can be for a backup procedure, where it's important to keep record of old files. This implementation includes only regular files, directories and symlinks. It also supports 5 different flags:

1- The f-flag specificies archive filename and was chosen to be mandatory in order to run this implementation. 

2- The v-flag used for increasing verbosity. It can be used along with the f-flag and either one of the following three flags.

3- The c-flag that creates a new archive (or truncates existing one) and all remaining command line arguments are taken as paths to be added to the archive. If one of the paths consists of a directory, then all the files and directories below it are added to the archive in DFS order. 

4-The t-flag that lists the contents of a given Tar archive. If paths are given, it will only list such paths and their descendents (again in a DFS order). 

5- The x-flag that extracts files from a given Tar archive. It uses the same logic as the t-flag for paths given. One note for the x-flag is that if a path is given where it includes non-existant parent directories, the x-flag will create all the parent directories in order to extract the path. 

Here are example runs for this program :

 $ ./mytar cf test.tar War_And_Peace.txt 
 
 $ ./mytar cvf test.tar War_And_Peace.txt 
War_And_Peace.txt

 $ ./mytar tf test.tar 
War_And_Peace.txt 

 $ ./mytar tvf test.tar 
-rw------- jcavalca/domain u        3286874 2020-11-12 10:12 War_And_Peace.txt 

 $ ./mytar xvf test.tar 
War_And_Peace.txt



