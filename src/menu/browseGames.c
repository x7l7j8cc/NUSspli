// --- TEST FILE ---
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Function to Print the Game/DLC List on NUSspli
void BrowseDList(char *gameTitle) {
   char * token = strtok(gameTitle, "|");
   int cntTK = 0; int cntLs = 0;
   while( token != NULL ) {
      
      switch(cntTK){
         //Title Name
         case 0: printf("%u. %s ", cntLs, token); token=strtok(NULL,"|"); cntTK++; break;
         //Title ID
         case 1: printf("[%s]", token); token=strtok(NULL,"|"); cntTK++; break;
         //Title Region
         case 2: printf(" (%s)\n", token); token=strtok(NULL,"|"); cntTK=0; break;
      }
      cntLs=cntLs+1;
   }
   return;
}

//Function to Read the Games/DLC Database
int readDBGames() {
  FILE *fp = fopen("DB_Games.txt", "r");
  if(fp == NULL) { perror("Unable to open DB_Games file!"); exit(1); }
  char *line = NULL;
  size_t len = 0;
  while(getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\r\n")] = 0;
    BrowseDList( line );
  }
  fclose(fp); free(line);
}

int main(){
   readDBGames(); //Init Read Database
   return 0;
}
