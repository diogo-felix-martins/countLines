
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define _BSD_SOURCE
#define GetCurrentDir getcwd
int totalLines = 0;

// ****************************
//      STAK IMPLEMENTATION
// ****************************
/*
AUTHOR: MATTHIAS VAN GESTEL - maregt0@gmail.com
Belgium - Lessius - Denayer
--------------------------------------------------------*/

#define DATASIZE PATH_MAX

typedef struct FenNode{
	char data[DATASIZE];
	struct FenNode *previousNode;
}FenNode;

FenNode *topStackPointer = NULL;
FenNode *lastDirectoryChecked = NULL;

void printStack(void){
	printf("***FenStack***\n");
	printf("**************\n");
	int numberoffNodes = 0;
	FenNode *TempPointer = topStackPointer;
	while(TempPointer != NULL){
		numberoffNodes++;
		printf("%s\n",TempPointer->data);
		TempPointer = TempPointer->previousNode;
	};
	printf("**************\n");
	printf("Counted %d Nodes\n",numberoffNodes);
	printf("***FenStack***\n");
	return;
}

int pop(char returnString[]){
	if(topStackPointer == NULL) return 0;
	strcpy(returnString,topStackPointer->data);
	FenNode *PreviousNodePointer = topStackPointer->previousNode;
	free(topStackPointer);
	topStackPointer = PreviousNodePointer;
	return 1;
}

int push(char data[]){
	FenNode *NewNodePointer = malloc(sizeof(FenNode));
	if(!NewNodePointer) return 0;
	strcpy(NewNodePointer->data,data);
	NewNodePointer->previousNode = 	topStackPointer;
	topStackPointer = NewNodePointer;
	return 1;
}


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
void countDirectoryFiles(char* cwd, char* argv[])
{
struct dirent *pDirent;
DIR *pDir;

            pDir = opendir (cwd);
            if (pDir == NULL) {
                printf ("Cannot open directory '%s'\n", cwd);
                exit(EXIT_FAILURE);
            }
            
            // Count each file and if there is a directory call the function on it
            while ((pDirent = readdir(pDir)) != NULL) {
                if (pDirent->d_type == DT_REG && fileIncluded(pDirent->d_name, argv)) // If it is a regular file
                {
                    totalLines += countLinesOfFile (joinFileName(cwd, pDirent->d_name));
                }
            }

            closedir (pDir);
}

void updateDirectoryList(char* newDir, char* argv[])
{     
            push(newDir); // Add new dir to the string stack
            countDirectoryFiles(newDir, argv);
}

bool fetchAllDirectories(char* cwd, char* argv[])
{
struct dirent *pDirent;
DIR *pDir;
bool dirListUpdated = false;

            pDir = opendir (cwd);
            
            if (pDir == NULL) {
                printf ("Cannot open directory '%s'\n", cwd);
                exit(EXIT_FAILURE);
            }
            
            // If there is a directory add it to the list
            while ((pDirent = readdir(pDir)) != NULL) {
                if (pDirent->d_type == DT_DIR && strcmp(pDirent->d_name, ".") !=0 && strcmp(pDirent->d_name, "..") !=0 && pDirent->d_name[0] != '.') // If it is a directory with a name different than . and .. or starting with .
                {
                    updateDirectoryList(joinFileName(cwd, pDirent->d_name), argv);
                    dirListUpdated = true;
                }
            }

            closedir (pDir);
            return dirListUpdated;
}

int main(int argc, char* argv[])
{
bool isRecursive = checkArgumentsForRecursive(&argc, argv);
bool dirListUpdated = true;
            
            char cwd[PATH_MAX];
            
            if (getcwd(cwd, sizeof(cwd)) != NULL) 
            {
                if (isRecursive) // Count files in all directories
                {   
                    updateDirectoryList(cwd, argv);
                    
                    while(dirListUpdated)
                    {
                        FenNode *stackReaderPointer = topStackPointer;
                        
                        dirListUpdated = fetchAllDirectories(stackReaderPointer->data, argv);
                        stackReaderPointer = stackReaderPointer->previousNode;
                    }
                }
                else
                {
                    countDirectoryFiles(cwd, argv); // Count files in current directory only
                }
            }
            else
            {
                printf("Failed to get working directory.");
            }

            printf("\nTotal lines: %d\n", totalLines);

            return 0;
}
