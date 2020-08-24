#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#define GetCurrentDir getcwd

char* countDirectoryFiles(char* cwd, char* argv[], _Bool* isRecursive);

// Checks if the file is to be counted by checking the argv array until the -e flag.
_Bool fileIncluded (char* fileName, char* argv[]);

// Counts the number of lines of a single file.
int countLinesOfFile(char *fileName);

int main(int argc, char* argv[])
{
_Bool* isRecursive = (_Bool *)malloc(sizeof(_Bool));

            if ( strcmp(argv[argc-1], "-r") == 0)
                *isRecursive = 1;
                        
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) 
            {
                countDirectoryFiles(cwd, argv, isRecursive); // Count lines of files in the current working directory
            }
            else
            {
                printf("Failed to get working directory.");
            }

            return 0;
}

// Checks if the file is to be counted by checking the argv array until the -e flag.
_Bool fileIncluded (char* fileName, char* argv[])
{
_Bool isIncluded = 0;

            // i starts at 1 to ignore the executable
            for ( int i = 1 ; strcmp(argv[i], "-e") != 0 ; i++){
                if (strstr(fileName, argv[i]) != NULL)
                    isIncluded = 1;
            }
            
            return isIncluded;
}

// Counts the number of lines of a single file.
int countLinesOfFile(char *fileName)
{
FILE *fp;

            fp = fopen(fileName, "r");
            if( fp == NULL ){
                printf("Failed to open the file \"%s\".\n", fileName);
                return EXIT_FAILURE;
            }

            int lineCount = 0;
            int c;
            while ((c=fgetc(fp))!= EOF){
                if ( c == '\n')
                    lineCount++;
            }

            printf("FILE: \"%s\" has %d lines.\n", fileName, lineCount);

            fclose(fp);
            fp = NULL;
            return lineCount;
}

// Counts the number of lines of the files present in the directory
char* countDirectoryFiles(char* cwd, char* argv[], _Bool* isRecursive){
struct dirent *pDirent;
DIR *pDir;

            pDir = opendir (cwd);
            if (pDir == NULL) {
                printf ("Cannot open directory '%s'\n", argv[1]);
                exit(EXIT_FAILURE);
            }
            
            // Count each file and if there is a directory call the function on it
            while ((pDirent = readdir(pDir)) != NULL) {
                if (pDirent->d_type == 4 && *isRecursive) // If it is a directory
                {
                    if (strcmp(pDirent->d_name, ".") !=0 && strcmp(pDirent->d_name, "..") !=0){
                        char* tmpCWD = malloc(2 + strlen(cwd) + strlen(pDirent->d_name)); // tmpCWD size is cwd + / + d_name + \0
                        strcpy(tmpCWD, cwd);
                        countDirectoryFiles(strcat( strcat(tmpCWD, "/"), pDirent->d_name), argv, isRecursive);
                        free(tmpCWD);
                    }
                }                    
                else
                {
                    if (fileIncluded(pDirent->d_name, argv))
                    {
                        char* tmpCWD = malloc(2 + strlen(cwd) + strlen(pDirent->d_name)); // tmpCWD size is cwd + / + d_name + \0
                        strcpy(tmpCWD, cwd);
                        countLinesOfFile ( strcat( strcat(tmpCWD, "/"), pDirent->d_name));
                        free(tmpCWD);
                        //countLinesOfFile (pDirent->d_name);
                    }
                }
            }

            closedir (pDir);
}