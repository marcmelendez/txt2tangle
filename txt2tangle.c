/*****************************************************************************
 *                                 txt2tangle                                *
 *                   A very simple literate programming tool                 *
 *                          Marc Meléndez Schofield                          *
 *****************************************************************************/

/*** Standard libraries ***/

# define _GNU_SOURCE
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

/*** PREPROCESSOR OPTIONS ***/

# ifndef MAX_RECURSION_LEVEL
# define MAX_RECURSION_LEVEL 10
# endif

# ifndef DEFAULT_COMMAND_STRING
# define DEFAULT_COMMAND_STRING "%!"
# endif

# ifndef TEXT_BUFFER_SIZE
# define TEXT_BUFFER_SIZE 1500
# endif

/*** Definitions ***/

# define max(a,b) ((a)>(b))?a:b
typedef enum {false = 0, true} bool;

/*** Auxiliary functions ***/

/* Output code snippets from literate programming source file */
int output_code(char * filename, char * command_string, char * command_fmt);

/* Copy the path of a filename to the pathname string */
int copy_path(char * filename, char * pathname, char * basename);

/* Insert code from file */
int insert_code(char * txtbuf, char * command_fmt, char * defaultfilename,
                FILE * outputfile, int recursion_level, bool definition);

/* Output a buffer to a file as is (no format string interpretation) */
int output_buffer(FILE * outputfile, char * txtbuf);

/***** Main program *****/

int main(int argc, char * argv[])
{
  /* Usage message */
  if(argc < 2) {
    fprintf(stderr, "txt2tangle: A very simple literate programming tool.\n\n"
                    "Usage: %s [options] <file>\n\n"
                    "Options:\n"
                    "  -c <string>\tSpecify the string preceding txt2tangle "
                    "commands (default is \"%%!\").\n\n", argv[0]);
    return 0;
  }

  /* Read command-line options */
  int i, j = 0;
  char * filename = NULL;
  char * command_string = NULL;
  for(i = 0; i < argc; i++) {
    if(argv[i][0] == '-') {
      switch(argv[i][1]) {
        case 'c':
           if(i + 1 < argc) {
             command_string = malloc(sizeof(char)*strlen(argv[i + 1]));
             strcpy(command_string, argv[i + 1]);
             i++;
           }
           else {
             fprintf(stderr, "Error: expected argument after -c missing.\n");
             return -1;
           }
           break;
        default:
           fprintf(stderr, "Error: unrecognised option (%s).\n", argv[i]);
           return -1;
      }
    }
    else {
      filename = malloc(sizeof(char)*(strlen(argv[i]) + 1));
      strcpy(filename, argv[i]);
    }
  }


  /* Default command string */
  if(command_string == NULL) {
    command_string = malloc(sizeof(char)*(strlen(DEFAULT_COMMAND_STRING) + 1));
    strcpy(command_string, DEFAULT_COMMAND_STRING);
  }

  /* Build command format string */
  int command_fmt_size;
  char * command_fmt;

  command_fmt_size = strlen(command_string);
  /* Count backslashes and percents twice */
  for(i = 0; i < strlen(command_string); i++)
    if(command_string[i] == '\\' || command_string[i] == '%')
      command_fmt_size++;

  command_fmt = malloc(sizeof(char)*(command_fmt_size + strlen(" %[^: \n]")));

  for(i = 0; i < strlen(command_string); i++) {
    command_fmt[j] = command_string[i]; j++;
    /* Deal with the backslash and percent characters */
    if(command_string[i] == '\\' || command_string[i] == '%') {
     command_fmt[j] = command_string[i];
     j++;
    }
  }
  command_fmt[j] = '\0'; /* String end */

  strcat(command_fmt, " %[^: \n]");

  /* Write code */
  output_code(filename, command_string, command_fmt);

  /* Clean up */
  free(command_fmt);
  free(filename);

  return 0;
}

/* Output code snippets from literate programming source file */
int output_code(char * filename, char * command_string, char * command_fmt)
{
  /* Open literate source file stream */
  FILE * sourcefile = fopen(filename, "r");

  if(sourcefile == NULL) {
    fprintf(stderr, "Error: Unable to open file (%s).\n", filename);
    return -1;
  }

  /* Move to target directory */
  char targetdir[TEXT_BUFFER_SIZE], targetbase[TEXT_BUFFER_SIZE];
  copy_path(filename, targetdir, targetbase);
  char * cwd = get_current_dir_name();
  chdir(targetdir);

  char txtbuf[TEXT_BUFFER_SIZE], word[TEXT_BUFFER_SIZE], command[20];
  char outputfilename[TEXT_BUFFER_SIZE];
  FILE * outputfile = NULL;
  bool printing = false;
  bool newfile;

  /* Read source file and output code snippets */
  while(1) { /* Find the next command string */
    /* Read next line */
    if(!fgets(txtbuf, TEXT_BUFFER_SIZE, sourcefile)) break;

    /* First word at the beginning of a line */
    word[0] = '\0'; /* Clear previous word */
    sscanf(txtbuf, "%s", word);

    /* See if the first word was a command */
    if(!strncmp(word, command_string,
                strlen(command_string))) { /* Command found */
      command[0] = '\0'; /* Clear previous command */
      sscanf(txtbuf, command_fmt, command); /* Read command */
      /* Choose action */
      if((newfile = !strcmp(command, "codefile")) /* Open code file */
         || !strcmp(command, "codecontinue")) {
        sscanf(txtbuf, "%*[^:]: %s",
               outputfilename); /* Read output file name */
        if(newfile)
          outputfile = fopen(outputfilename, "w"); /* Open file */
        else
          outputfile = fopen(outputfilename, "a"); /* Reopen file */
        if(outputfile == NULL) {
          fprintf(stderr, "Error: Unable to open file (%s).\n",
                  outputfilename);
          return -1;
        }
        printing = true; /* Turn on printing */
      } else if(!strcmp(command, "codeend")
                || !strcmp(command, "codepause")) { /* Stop code output */
        printing = false;
        fclose(outputfile);
      } else if(!strcmp(command, "codeinsert")) { /* Insert a code block */
        if(printing)
          insert_code(txtbuf, command_fmt, filename, outputfile, 0, false);
      } else if(!strcmp(command, "codedefinition")) { /* Insert a definition */
        if(printing)
          insert_code(txtbuf, command_fmt, filename, outputfile, 0, true);
      }
    }
    else {
      /* Output code */
      if(printing) output_buffer(outputfile, txtbuf);
    }
  }

  fclose(sourcefile);
  chdir(cwd);

  return 0;
}
/* Copy the path and basename of a filename to separate strings */
int copy_path(char * filename, char * pathname, char * basename)
{
  int pathlength = 0;
  for(int i = 0; i < TEXT_BUFFER_SIZE; ++i) {
    if(filename[i] == '/') pathlength = i;
    else if(filename[i] == '\0') {
      strncpy(pathname, filename, pathlength);
      if(pathlength > 0) strcpy(basename, &filename[pathlength + 1]);
      else strcpy(basename, filename);
      pathname[pathlength] = '\0';
      return 0;
    }
  }
  fprintf(stderr, "Error: filename exceeds maximum text buffer size.\n");
  exit(-1);
  return 0;
}
/* Insert code from file */
int insert_code(char * txtbuf, char * command_fmt, char * defaultfilename,
                FILE * outputfile, int recursion_level, bool definition)
{
  if(recursion_level >= MAX_RECURSION_LEVEL) {
    fprintf(stderr, "Error: maximum recursion level exceeded.\n");
    exit(-2);
  }
  char inputfilename[TEXT_BUFFER_SIZE], inputpathname[TEXT_BUFFER_SIZE];
  char inputbasename[TEXT_BUFFER_SIZE];
  char codeblockname[TEXT_BUFFER_SIZE];
  char command[20];
  char name[TEXT_BUFFER_SIZE];
  FILE * inputfile = NULL;
  char callerfilename[TEXT_BUFFER_SIZE], callerbasename[TEXT_BUFFER_SIZE];
  char parentpathname[TEXT_BUFFER_SIZE];
  char * callerpathname = get_current_dir_name(); /* Caller file directory */
  bool located_codeblock;

  /* Build caller block file name */
  copy_path(defaultfilename, parentpathname, callerbasename);
  strcpy(callerfilename, callerpathname);
  strcat(callerfilename, "/");
  strcat(callerfilename, callerbasename);

  /* Read code block name (and input file name, if necessary)  */
  inputfilename[0] = '\0';
  if(sscanf(txtbuf, "%*[^:]: %s src: %s", codeblockname, inputfilename) < 2) {
    strcpy(inputfilename, defaultfilename);
  }

  /* Open file (using the right path) */
  if(!definition) inputfile = fopen(inputfilename, "r");
  else {
    copy_path(inputfilename, inputpathname, inputbasename);
    if(parentpathname[0] != '\0') strcat(parentpathname, "/");
    if(parentpathname[0] == '/') strcat(parentpathname, inputbasename);
    else strcat(parentpathname, inputfilename);
    inputfile = fopen(parentpathname, "r");
  }

  if(inputfile == NULL) {
    if(!definition)
      fprintf(stderr, "Error: Unable to open file (%s/%s).\n",
                      get_current_dir_name(),
                      inputbasename);
    else
      fprintf(stderr, "Error: Unable to open file (%s).\n",
                      inputfilename);
    return -1;
  }

  /* Find path to input file and move to input file directory if necessary */
    copy_path(inputfilename, inputpathname, inputbasename);
    chdir(inputpathname);

  /* Find beginning of code block */
  located_codeblock = false;
  while(1) {
    if(!fgets(txtbuf, TEXT_BUFFER_SIZE, inputfile)) break; /* Read next line */
    command[0] = '\0'; /* Clear previous word */
    sscanf(txtbuf, command_fmt, command); /* Look for codeblock command */
    if(!strcmp(command, "codeblock")) {
      name[0] = '\0'; /* Clear previous name */
      sscanf(txtbuf, "%*[^:]: %s", name); /* Get code block name */
      if(!strncmp(name, codeblockname,
        max(strlen(codeblockname),strlen(name)))) { /* Block found */
        located_codeblock = true;
        break;
      }
    }
  }

  /* Error message if the block was not found */
  if(!located_codeblock) {
    fprintf(stderr, "Error: code block %s not located in file %s.\n",
            codeblockname, inputfilename);
    exit(-3);
  }

  /* Print out block until end is reached */
  while(1) {
    if(!fgets(txtbuf, TEXT_BUFFER_SIZE, inputfile)) break; /* Read next line */
    command[0] = '\0'; /* Clear previous command */
    sscanf(txtbuf, command_fmt, command);
    if(!strcmp(command, "codeblockend")) { /* Stop at end of block */
      fclose(inputfile);
      break;
    } else if(!strcmp(command, "codeinsert")) {
       insert_code(txtbuf, command_fmt, inputbasename, outputfile,
                   recursion_level + 1, false);
    } else if(!strcmp(command, "codedefinition")) {
       insert_code(txtbuf, command_fmt, callerfilename, outputfile,
                   recursion_level + 1, true);
    } else if(!strcmp(command, "codecomment")) {
       /* Do nothing (ignore the line) */
    } else if(!strcmp(command, "codepause")) {
       fprintf(stderr, "Error: codepause inside codeblock: %s.\n",
               codeblockname);
       exit(-1);
    } else {
      output_buffer(outputfile, txtbuf);
    }
  }

  /* Return to caller file directory if necessary */
  chdir(callerpathname);
  free(callerpathname);

  return 0;
}
/* Output a buffer to a file as is (no format string interpretation) */
int output_buffer(FILE * outputfile, char * txtbuf) {
  char output_string[2*TEXT_BUFFER_SIZE];
  int i, j;

  output_string[0] = '\0';

  j = 0;
  for(i = 0; i < strlen(txtbuf); i++) {
    output_string[j] = txtbuf[i]; j++;
    if(txtbuf[i] == '%') {
     output_string[j] = txtbuf[i];
     j++;
    }
  }
  output_string[j] = '\0';

  fprintf(outputfile, output_string);

  return 0;
}
