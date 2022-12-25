// C program for the Flipper Zero
// Renames folders of images and adds meta-txt for animations

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

void findFiles(const char *path);
void sortFiles();
void addPathToFiles();
int printAndConfirm();
void renameFiles();
char* copyString();
void fileWriter();

char fileStart[20] = "frame_";
char fileEnd[] = ".txt";
char path[PATH_MAX + 1];
char *fileNames[PATH_MAX + 1];
char *fileNamesWithPath[PATH_MAX + 1];
char *newFileNamesWithPath[PATH_MAX + 1];
char currentFile = 0;
char fileNumberAsString[3];

int main()
{
    // Input path from user
    printf("Enter path to rename files: ");
    scanf("%s", path);

    findFiles(path);
    sortFiles();
    addPathToFiles();
    int proceed = printAndConfirm(); // can get rid of proceed variable
    if (proceed)
        renameFiles();

    fileWriter();

    return 0;
}

// Lists all files and sub-directories at given path.
void findFiles(const char *path)
{
    struct dirent *dp;
    DIR *dir = opendir(path);

    // Unable to open directory stream
    if (!dir) 
        return; 

    while ((dp = readdir(dir)) != NULL)
    {
        if (strstr(dp->d_name, fileEnd) != NULL)
        {
            fileNames[currentFile] = dp->d_name;
            currentFile++;
        }
    }

    // Close directory stream
    closedir(dir);
}

void sortFiles()
{
    // Sorts files alphabetically
    char *tmp;
    int i = 0, j;
    
    for(i; fileNames[i]; i++) 
    {
        for(j = 0; fileNames[j]; j++) 
        {
            if(strcmp(fileNames[i], fileNames[j]) < 0) 
            {
                tmp = fileNames[i];
                fileNames[i] = fileNames[j];
                fileNames[j] = tmp;
            }
        }
    }
}

void addPathToFiles()
{
    for (int i = 0; i < currentFile ; i++)
    {
        fileNamesWithPath[i] = copyString();
    }

    for (int i = 0; i < currentFile; i++)
    {
        strcat(fileNamesWithPath[i], fileNames[i]);
    }
}

char* copyString()
{
    char* s2;
    s2 = (char*)malloc(PATH_MAX + 1);

    strcpy(s2, path);
    return (char*)s2;
}

int printAndConfirm()
{
    // Print files and get confirmation
    char answer[4];

    printf("Here is the list of image files found:\n");

    for (int i = 0; i < currentFile; i++)
    {
        if(fileNamesWithPath[i] != "\0")
        {
            printf("%s\n", fileNamesWithPath[i]);
        }
    }

    printf("Would you like to proceed (Y or N)? ");
    scanf("%s", answer);

    if (answer[0] == 'Y' || answer[0] == 'y')
    {
        return 1;
    }

    return 0;
}

void renameFiles()
{
    // Renames files
    for (int i = 0; i < currentFile; i++)
    {
        if (fileNamesWithPath[i] != "\0")
        {
            sprintf(fileNumberAsString, "%d", i);

            strcat(fileStart, fileNumberAsString);
            strcat(fileStart, fileEnd);

            newFileNamesWithPath[i] = copyString();
            strcat(newFileNamesWithPath[i], fileStart);

            int result = rename(fileNamesWithPath[i], newFileNamesWithPath[i]);

            if (result == 0)
            {
                printf("The file was renamed to %s\n", fileStart);
            }
            else
            {
                printf("%s could not be renamed.\n", fileNamesWithPath[i]);
            }

            fileStart[6] = '\0';
        }
    }
}

void fileWriter()
{
    char framesAsString[6];
    char *metaFileName;

    char data[PATH_MAX + 1] = "Filetype: Flipper Animation\nVersion: 1\n\nWidth: 128\nHeight: 64\nPassive frames: ";
    char file2[] = "\nActive frames: 0\nFrames order: ";
    char file3[] = "\nActive cycles: 0\nFrame rate: 6\nDuration: 3600\nActive cooldown: 0\n\nBubble slots: 0";

    sprintf(framesAsString, "%d", currentFile);

    strcat(data, framesAsString);
    strcat(data, file2);

    for (int i = 0; i < currentFile; i ++)
    {
        sprintf(framesAsString, "%d ", i);
        strcat(data, framesAsString);
    }

    strcat(data, file3);

    metaFileName = copyString();
    strcat(metaFileName, "meta.txt");

    FILE *fPtr;

    fPtr = fopen(metaFileName, "w");

    if (fPtr == NULL)
    {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fputs(data, fPtr);
    fclose(fPtr);

    printf("File created and saved successfully. :) \n");
}