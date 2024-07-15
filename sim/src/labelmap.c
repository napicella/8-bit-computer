#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void debuggerLoadLabels(const char *labelFileContents,
                        char *labelMap[0x10000]) {
  for (int i = 0; i < 0x10000; i++) {
    if (labelMap[i]) {
      free(labelMap[i]);
      labelMap[i] = NULL;
    }
  }

  char *line = strtok(labelFileContents, "\n");
  char al[2];
  // loop through each line
  while (line != NULL) {
    uint16_t *address = (uint16_t *)malloc(sizeof(uint16_t));
    char *label = (char *)malloc(50 * sizeof(char));

    sscanf(line, "%s %x %s", al, address, label);
    if (strncmp(label, "__", 2) != 0) {
      // if the label starts with `__`, do not use it
      labelMap[*address] = label;
    }

    // get the next line
    line = strtok(NULL, "\n");
  }
}

char *readFile(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("Failed to open file");
    return NULL;
  }

  // Determine the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory for the file content plus a null terminator
  char *buffer = malloc(file_size + 1);
  if (buffer == NULL) {
    perror("Failed to allocate memory");
    fclose(file);
    return NULL;
  }

  // Read the file into the buffer
  size_t read_size = fread(buffer, 1, file_size, file);
  if (read_size != file_size) {
    perror("Failed to read file");
    free(buffer);
    fclose(file);
    return NULL;
  }

  // Null-terminate the string
  buffer[file_size] = '\0';

  // Close the file
  fclose(file);

  return buffer;
}