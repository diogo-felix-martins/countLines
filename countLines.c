#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define _BSD_SOURCE
#define GetCurrentDir getcwd
int totalLines = 0;

// Check if -r flag exists
bool checkArgumentsForRecursive(int* argc, char* argv[])
{
bool isRecursive = false;

            for ( int i = 1 ; i < *argc ; i++){
                if (strstr(argv[i], "-r") != NULL)
                    isRecursive = true;
            }

            return isRecursive;
}

// Checks if the file is to be counted by checking the argv array until the -e flag.
bool fileIncluded (char* fileName, char* argv[])
{
bool isIncluded = false;

            // i starts at 1 to ignore the executable
            for ( int i = 1 ; strcmp(argv[i], "-e") != 0 ; i++){
                if (strstr(fileName, argv[i]) != NULL)
                    isIncluded = true;
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

// Joins the file path and file name
char* joinFileName(char* filePath, char* fileName)
{
char* joinedFileName;

            char* tmpCWD = malloc(2 + strlen(filePath) + strlen(fileName)); // tmpCWD size is cwd + / + d_name + \0
            strcpy(tmpCWD, filePath);
            joinedFileName = strcat(strcat(tmpCWD, "/"), fileName);

            return joinedFileName;

}

// Counts the number of lines of the files present in the directory
char* countDirectoryFiles(char* cwd, char* argv[], const bool isRecursive){
struct dirent *pDirent;
DIR *pDir;

            pDir = opendir (cwd);
            if (pDir == NULL) {
                printf ("Cannot open directory '%s'\n", argv[1]);
                exit(EXIT_FAILURE);
            }
            
            // Count each file and if there is a directory call the function on it
            while ((pDirent = readdir(pDir)) != NULL) {
                switch (pDirent->d_type){
                    case DT_DIR: // Directory
                        if (isRecursive && strcmp(pDirent->d_name, ".") !=0 && strcmp(pDirent->d_name, "..") !=0)
                        {
                            countDirectoryFiles(joinFileName(cwd, pDirent->d_name), argv, isRecursive);
                        }
                        break;

                    case DT_REG: // Regular file
                        if (fileIncluded(pDirent->d_name, argv))
                        {
                            totalLines += countLinesOfFile (joinFileName(cwd, pDirent->d_name));
                        }
                        break;

                    default:
                        printf("Unintended file type.");
                }
            }

            closedir (pDir);
}


int main(int argc, char* argv[])
{
bool isRecursive = checkArgumentsForRecursive(&argc, argv);
                        
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) 
            {
                countDirectoryFiles(cwd, argv, isRecursive); // Count lines of files in the current working directory
            }
            else
            {
                printf("Failed to get working directory.");
            }

            printf("\nTotal lines: %d", totalLines);

            return 0;
}
