# include <stdint.h>
# include <sys/types.h>
# include <string.h>

/*Given functions ...*/

/*Inserts a number when it is non conforming
  to the octal representation defined by ustar
  standard. */
int insert_special_int(char *where, size_t size, int32_t val){

int err = 0;

if (val < 0 || (size < sizeof(val)) ){
err++;
}else{

memset(where, 0, size);
*(int32_t *)(where+size-sizeof(val)) = htonl(val);
*where |= 0x80;
}

return err;
}

/*Inserts a number when it is non conforming
  to the octal representation defined by ustar
  standard. */

uint32_t extract_special_int(char *where, int len){

int32_t val =-1;

if ( (len>=sizeof(val)) && (where[0] & 0x80)){

        val = *(int32_t *)(where+len-sizeof(val));
        val = ntohl(val);
}
return val;
}
