#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void BrowseDList(char *gameTitle) {
   char * token = strtok(gameTitle, "|");
   int counter = 0;
   while( token != NULL ) {
      switch(counter){
         case 0:
            printf( " %s", token );
            token = strtok(NULL, "|");
            counter++;
            break;
         case 1:
            printf( " [%s]", token );
            token = strtok(NULL, "|");
            counter++;
            break;
         case 2:
            printf( "[%s]\n", token );
            token = strtok(NULL, "|");
            counter=0;
            break;
      }
   }
   return;
}

int readDBGames() {
  FILE *fp = fopen("DB_Games.txt", "r");
  if(fp == NULL) {
    perror("Unable to open file!");
    exit(1);
  }
  char *line = NULL;
  size_t len = 0;
  while(getline(&line, &len, fp) != -1) {
    BrowseDList(line);
  }
  fclose(fp);
  free(line);
}

int main(){
   readDBGames();
   return 0;
}
