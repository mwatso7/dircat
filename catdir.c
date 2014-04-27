/* catdir.c
 * accept a path as an argument and output the contents of all regular files.
 *
 * Max Watson
 * mwatso7@kent.edu
 * 04/10/2014
 */

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 100

void cat(char *path){
  unsigned char buff[BUFF_SIZE];
  size_t numread, numwrote = 1;
  DIR *dirp = NULL;
  FILE * f;
  struct dirent *dent = NULL;
  struct stat st;

  //length of the path and possible filename
  unsigned int pathlen = strlen(path);
  unsigned int max_file_len = pathconf(path, _PC_NAME_MAX);
  unsigned int tmp_path_len = pathlen + max_file_len + 2;

  char tmp_path[ tmp_path_len ];
  memset(tmp_path,0, tmp_path_len);

  //copy in the bytes from the path
  strcpy(tmp_path,path);
  if( tmp_path[pathlen - 1] != '/' ){
    tmp_path[pathlen] = '/';
    tmp_path[pathlen + 1] = '\0';
    pathlen = strlen(tmp_path);
  }


  dirp = opendir( path );
  while( dent = readdir(dirp) ){
    if( dent == NULL ) break;

    if( dent->d_name[0] != '.' ){
      //copy the file name onto the end of the temp path name
      strcat(tmp_path,dent->d_name);

      // if the next directory object is a file write it's contents to stdout
      stat( tmp_path, &st );
      if(S_ISREG(st.st_mode)){
        f = fopen(tmp_path, "r");
        while( numread > 0 ){
          memset(buff, 0, BUFF_SIZE);
          numread = fread(buff, sizeof(unsigned char), BUFF_SIZE, f);
          numwrote = fwrite(buff, sizeof(unsigned char), numread, stdout);
        }
        numread = 1;
        fclose(f);
      }

      // null byte to allow remapping of path
      tmp_path[pathlen] = '\0';
    }
  }
  closedir(dirp);

}

int main(int argc, char **argv){

 struct stat st;

  //if no args, output current directory
  if( argc == 1 ){ cat( "." ); }
  else if ( argc == 2 ){
    // if the argument is a valid directory then output
    stat( argv[1], &st );
    if( S_ISDIR(st.st_mode) ){ cat( argv[1] ); }
    else { printf("Please enter a valid directory"); }
  }
  return 0;
}

