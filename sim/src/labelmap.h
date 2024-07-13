#ifndef LABELMAP_H
#define LABELMAP_H

char* readFile(const char* filename);
void debuggerLoadLabels(const char* labelFileContents, char* labelMap[0x10000]);

#endif