/*This files has useful functions that deal with 
 * octal numbers ...*/

char *dec_to_stroct(int n, int size){

        int count = 0;
        char *ret;
        char conversions[8] = {'0', '1', '2', '3', '4', '5', '6', '7'};
        ret = malloc(size);
        if (ret == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
        }
        for (count = 0; count < size - 1; count++)
        ret[count] = '0';
        for (count = 0; n > 0; count++){
        ret[size - count - 2] = conversions[n % 8];
        n = n /8;
        }
        ret[size -1] = '\0';
        return ret;

}

int non_conforming_check(int numb, uint64_t cap){
if (numb > cap)
return -1;
else
return 0;

}
