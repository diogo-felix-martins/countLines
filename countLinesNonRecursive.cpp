// TODO: Use std::filesystem

#include <iostream>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stack>

using namespace std;

// Globals
int totalLines = 0;

// Check if -r flag exists
bool checkArgumentsForRecursive(int argc, char* argv[])
{
bool isRecursive = false;
string recursiveFlag = "-r";

            for ( int i = 1 ; i < argc ; i++){
                if ( recursiveFlag.compare(argv[i]) == 0 )
                    isRecursive = true;
            }

            return isRecursive;
}

// Checks wether a path is a directory
bool isDirectory(const string path)
{
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

// Checks wether a path is a regular file
bool isRegularFile(const string path)
{
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    return S_ISREG(path_stat.st_mode);
}

// Checks if the file is to be counted by checking the argv array until the -e flag.
bool fileIncluded (const string fileName, char* argv[])
{
bool isIncluded = false;

            // i starts at 1 to ignore the executable
            for ( int i = 1 ; strcmp(argv[i], "-e") != 0 ; i++){
                if ( fileName.find(argv[i]) != string::npos ) // If filename contains any argv before the -e flag
                    isIncluded = true;
            }
            
            return isIncluded;
}

// Counts the number of lines of a single file.
int countLinesOfFile(string fileName)
{
int lineCount = 0;
            ifstream inFile(fileName); 
            lineCount = count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');

            cout << "FILE: " << fileName << " has " << lineCount << " lines." << endl;

            return lineCount;
}

// Joins the file path and file name
string joinFileName(const string filePath, const string fileName)
{
string joinedFileName;

            joinedFileName = filePath + "/" + fileName;

            return joinedFileName;

}

// Counts the number of lines of the files present in the directory
void countDirectoryFiles(string cwd, char* argv[])
{
            if ( auto dir = opendir(cwd.c_str()) ) {
                while ( auto f = readdir(dir) ) {
                    if ( !f->d_name || f->d_name[0] == '.' )
                        continue; // Skip everything that starts with a dot

                    string fileName(f->d_name); // Convert char* to std::string
                    string filePath = joinFileName (cwd , fileName);
                    if ( isRegularFile(filePath) && fileIncluded(fileName, argv) )
                        totalLines += countLinesOfFile(filePath);

                }
                
                closedir(dir);
            }else
                cout << "Failed to open directory: " << cwd << endl;
}

void updateDirectoryList(string newDir, stack<string>& dirList, char* argv[])
{     
            dirList.push(newDir); // Add new dir to the string stack
            countDirectoryFiles(newDir, argv);
}

bool fetchAllDirectories(string cwd, stack<string>& dirList, char* argv[])
{
bool dirListUpdated = false;

            if ( auto dir = opendir(cwd.c_str()) ) {
                while ( auto f = readdir(dir) ) {
                    if ( !f->d_name || f->d_name[0] == '.' )
                        continue; // Skip everything that starts with a dot

                    string fileName(f->d_name); // Convert char* to std::string
                    string dirPath = joinFileName (cwd , fileName);
                    if ( isDirectory(dirPath) )
                    {
                        updateDirectoryList(dirPath, dirList, argv);
                        dirListUpdated = true;
                    }

                }
                
                closedir(dir);
            }else
                cout << "Failed to open directory: " << cwd << endl;
            
            return dirListUpdated;
}

void showstack(stack <string> s) 
{ 
    cout << endl << "=================== STACK ===================" << endl;
    while (!s.empty()) 
    { 
        cout << '\n' << s.top(); 
        s.pop(); 
    } 
    cout << '\n'; 
    cout << endl << "================= END STACK =================" << endl;
} 

int main(int argc, char* argv[])
{
bool isRecursive = checkArgumentsForRecursive(argc, argv);
stack <string> dirList;

            if (isRecursive) // Count files in all directories
            {   
                updateDirectoryList(".", dirList, argv);
                
                while( !dirList.empty() ) // While stack has data
                {                     
                    string newDir = dirList.top();
                    dirList.pop();
                    fetchAllDirectories(newDir, dirList, argv);
                }
            }
            else
            {
                countDirectoryFiles(".", argv); // Count files in current directory only
            }

            
            cout << endl << "Total lines: " << totalLines << endl;

            return 0;
}
