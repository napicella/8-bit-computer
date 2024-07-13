#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


static char tmpBuffer[256] = {0};


/* Implementation of strlcpy() for platforms that don't already have it. */
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
  }
	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}
	return(s - src - 1);	/* count does not include NUL */
}

char *strupr(char s[])
{
  char	*p;

  for (p = s; *p; ++p)
    *p = toupper(*p);

  return(s);

}

static int isProbablyConstant(const char* str)
{
  strlcpy(tmpBuffer, str, sizeof(tmpBuffer) - 1);
  strupr(tmpBuffer);
  return strcmp(str, tmpBuffer) == 0;
}

void debuggerLoadLabels(const char* labelFileContents, char *labelMap[0x10000])
{
  for (int i = 0; i < sizeof(labelMap) / sizeof(const char*); ++i)
  {
    if (labelMap[i])
    {
      free(labelMap[i]);
      labelMap[i] = NULL;
    }
  }

  if (labelFileContents)
  {
    char lineBuffer[1024];

    char *p = (char*)labelFileContents;

    for (;;)
    {
      char* end = strchr(p, '\n');
      if (end == NULL)
        break;

      // skip the fist 3 characters in the label file ('al ')
      // which is used by the VICE emulator (https://www.cc65.org/doc/debugging-2.html)
      strlcpy(lineBuffer, p +3, end - p);
      p = end + 1;

      size_t labelStart = (size_t )-1, labelEnd = (size_t)-1, valueStart = (size_t)-1, valueEnd = (size_t)-1;

      int i = 0;
      int len = (int)strlen(lineBuffer);
      for (i = 0; i < len; ++i)
      {
        char c = lineBuffer[i];
        if (c == 0) break;
        if (!isspace(c) && c != '=' && c != '$')
        {
          if (labelStart == -1)
          {
            labelStart = i;
          }
          else if (labelEnd != -1 && valueStart == -1)
          {
            valueStart = i;
          }
        }
        else
        {
          if (labelStart != -1 && labelEnd == -1)
          {
            labelEnd = i;
          }
          else if (valueStart != -1 && valueEnd == -1)
          {
            valueEnd = i;
          }
        }
      }

      if (valueStart == -1)
      {
        continue;
      }
      else if (valueEnd == -1)
      {
        valueEnd = i;
      }


      char valueStr[100] = { 0 };

      strlcpy(valueStr, lineBuffer + valueStart, valueEnd - valueStart + 1);

      unsigned int value = 0;
      sscanf(valueStr, "%x", &value);

      uint16_t addr = (uint16_t)value;

      bool isUnused = strstr(lineBuffer, "; unused") != NULL;


      if (!labelMap[addr] || (isProbablyConstant(labelMap[addr]) && !isUnused))
      {
        char* label = (char*)malloc((labelEnd - labelStart) + 1);
        strlcpy(label, lineBuffer + labelStart, labelEnd - labelStart + 1);
        labelMap[addr] = label;
      }
    }
  }
}

char* readFile(const char *filename) {
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