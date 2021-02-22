// --- TEST FILE ---
//Testing Online with: https://www.onlinegdb.com/online_c_compiler

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Function to Print the Game/DLC List on NUSspli
void BrowseDList(char *gameTitle, int cLines) {
   char * gTitle = strtok(gameTitle, "|");
   int cntTK = 0;
   while( gTitle != NULL ){
      switch(cntTK){
         //Title Name
         case 0: printf("%u. %s ", cLines, gTitle); gTitle=strtok(NULL,"|"); cntTK++; break;
         //Title ID
         case 1: printf("[%s]", gTitle); gTitle=strtok(NULL,"|"); cntTK++; break;
         //Title Region
         case 2: printf(" (%s)\n", gTitle); gTitle=strtok(NULL,"|"); cntTK=0; break;
      }
      
   }
   return;
}

//Function to Read the Games/DLC from Database
int readDBGames(char *DBfile) {
  FILE *fp = fopen(DBfile, "r");
  if(fp == NULL) { fprintf(stderr, "!!--> Unable to open %s file <--!!", DBfile); exit(1); }
  char *line = NULL;
  size_t len = 0;
  int count_lines = 0;
  while(getline(&line, &len, fp) != -1) {
    count_lines++;
    line[strcspn(line, "\r\n")] = 0;
    BrowseDList( line, count_lines );
  }
  fclose(fp); free(line);
}

int main(){
   readDBGames("DB_Games.txt"); //Init Read Database
   return 0;
}
