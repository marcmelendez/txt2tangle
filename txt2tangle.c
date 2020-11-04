/*****************************************************************************
 *                                 txt2tangle                                *
 *                   A very simple literate programming tool                 *
 *****************************************************************************/

/*** Standard libraries ***/

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

/*** PREPROCESSOR OPTIONS ***/

# ifndef MAX_RECURSION_LEVEL
# define MAX_RECURSION_LEVEL 10
# endif

# ifndef COMMENT_LENGTH
# define COMMENT_LENGTH 10
# endif

# ifndef DEFAULT_COMMAND_STRING
# define DEFAULT_COMMAND_STRING "%!"
# endif

# ifndef TEXT_BUFFER_SIZE
# define TEXT_BUFFER_SIZE 1000
# endif

/*** Definitions ***/

typedef enum {false = 0, true} bool;

/*** Auxiliary functions ***/

/* Output code snippets from literate programming source file */
int output_code(char * filename, char * command_string);

/* Insert code from file */
int insertcode(char * txtbuf, char * command_fmt, char * defaultfile, FILE * outputfile, int recursion_level);

/***** Main program *****/

int main(int argc, char * argv[])
{
  /* Usage message */
  if(argc < 2) {
    fprintf(stderr, "txt2tangle: A very simple literate programming tool.\n\n"
                    "Usage: %s [options] <file>\n\n"
                    "Options:\n"
                    "  -c <string>\tSpecify the string preceding txt2tangle commands (default is \"%%!\").\n\n", argv[0]);
    return 0;
  }

  /* Read command-line options */
  int i;
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
      filename = malloc(sizeof(char)*strlen(argv[i]));
      strcpy(filename, argv[i]);
    }
  }

  /* Default command string */
  if(command_string == NULL) {
    command_string = malloc(sizeof(char)*strlen(DEFAULT_COMMAND_STRING));
    strcpy(command_string, DEFAULT_COMMAND_STRING);
  }

  output_code(filename, command_string);

  return 0;
}

/* Output code snippets from literate programming source file */
int output_code(char * filename, char * command_string)
{
  /* Open literate source file stream */
  FILE * sourcefile = fopen(filename, "r");

  if(sourcefile == NULL) {
    fprintf(stderr, "Error: Unable to open file (%s).\n", filename);
    return -1;
  }

  char txtbuf[TEXT_BUFFER_SIZE], word[TEXT_BUFFER_SIZE], command[20];
  char outputfilename[TEXT_BUFFER_SIZE];
  FILE * outputfile = NULL;
  bool printing = false;
  bool newfile;
  int i, j = 0, command_fmt_size;
  char * command_fmt;

  /* Build command format string */
  command_fmt_size = strlen(command_string);
  for(i = 0; i < strlen(command_string); i++)
    if(command_string[i] == '\\' || command_string[i] == '%') command_fmt_size++;

  command_fmt = malloc(sizeof(char)*(command_fmt_size + strlen(" %[^: \n]")));

  for(i = 0; i < strlen(command_string); i++) {
    command_fmt[j] = command_string[i]; j++;
    if(command_string[i] == '\\' || command_string[i] == '%') {
     command_fmt[j] = command_string[i];
     j++;
    }
  }

  strcat(command_fmt, " %[^: \n]");

  /* Read source file and output code snippets */
  while(1) { /* Find the next command string */
    if(!fgets(txtbuf, TEXT_BUFFER_SIZE, sourcefile)) break; /* Read next line */
    word[0] = '\0'; /* Clear previous word */
    sscanf(txtbuf, "%s", word); /* First word at the beginning of a line */
    if(!strncmp(word, command_string, sizeof(command_string[0]))) { /* Command found */
      sscanf(txtbuf, command_fmt, command); /* Read command */

      /* Choose action */
      if((newfile = !strcmp(command, "codefile")) || !strcmp(command, "codecontinue")) { /* New code file */
        sscanf(txtbuf, "%*[^:]: %s", outputfilename); /* Read output file name */
        if(newfile)
          outputfile = fopen(outputfilename, "w"); /* Open file */
        else
          outputfile = fopen(outputfilename, "a"); /* Reopen file */
        if(outputfile == NULL) {
          fprintf(stderr, "Error: Unable to open file (%s).\n", outputfilename);
          return -1;
        }
        printing = true;
      } else if(!strcmp(command, "codeend") || !strcmp(command, "codepause")) { /* Stop code output */
        printing = false;
        fclose(outputfile);
      } else if(!strcmp(command, "codeinsert")) {
        insertcode(txtbuf, command_fmt, filename, outputfile, 0);
      }
    }
    else {
      /* Output code */
      if(printing) fprintf(outputfile, txtbuf);
    }
  }

  fclose(sourcefile);

  return 0;
}

/* Insert code from file */
int insertcode(char * txtbuf, char * command_fmt, char * defaultfile, FILE * outputfile, int recursion_level)
{
  if(recursion_level >= MAX_RECURSION_LEVEL) {
    fprintf(stderr, "Error: maximum recursion level exceeded.\n");
    exit(-2);
  }

  char inputfilename[TEXT_BUFFER_SIZE], codeblockname[TEXT_BUFFER_SIZE], command[20];
  char name[TEXT_BUFFER_SIZE];
  FILE * inputfile = NULL;
  bool located_codeblock;

  /* Read code block name (and input file name, if necessary)  */
 	if(sscanf(txtbuf, "%*[^:]: %s src: %s", codeblockname, inputfilename) == 2) {
   	inputfile = fopen(inputfilename, "r");
  } else {
   	inputfile = fopen(defaultfile, "r");
 	}
 	if(inputfile == NULL) {
   	fprintf(stderr, "Error: Unable to open file (%s).\n", inputfilename);
   	return -1;
  }

 	/* Find beginning of code block */
 	located_codeblock = false;
 	while(1) {
   	if(!fgets(txtbuf, TEXT_BUFFER_SIZE, inputfile)) break; /* Read next line */
   	command[0] = '\0'; /* Clear previous word */
   	sscanf(txtbuf, command_fmt, command); /* Look for codeblock command */
   	if(!strcmp(command, "codeblock")) {
     	sscanf(txtbuf, "%*[^:]: %s", name); /* Get code block name */
     	if(!strncmp(name, codeblockname, sizeof(codeblockname[0]))) { /* Block found */
       	located_codeblock = true;
       	break;
     	}
   	}
 	}

 	/* Error message if the block was not found */
 	if(!located_codeblock) {
   	fprintf(stderr, "Error: code block %s not located in file %s.\n", codeblockname, inputfilename);
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
          insertcode(txtbuf, command_fmt, inputfilename, outputfile, recursion_level + 1);
    }	else {
     	fprintf(outputfile, txtbuf);
   	}
 	}

  return 0;
}
