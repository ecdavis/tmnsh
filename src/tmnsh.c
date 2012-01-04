/*****************************************************************************
 *
 * Copyright 2011 Evan Christopher Davis <ecdavis@wtfrak.com>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/***** Includes *************************************************************/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "expression.h"
#include "interpreter.h"
#include "parser.h"
#include "tmnsh.h"


/***** CLI Functions ********************************************************/

/**
 * void show_welcome()
 *
 * Prints a welcome message to the standard output.
 */
void show_welcome() {
	printf("\nTeenage Mutant Ninja Shell v 1.0.0\nTo escape type 'quit' and hit return.\n\n");
}

/**
 * void show_prompt()
 *
 * Prints the prompt (currently the current working directory) to the
 * standard output.
 */
void show_prompt() {
	char cwd[CWD_MAX_SIZE];
	
	getcwd(cwd, CWD_MAX_SIZE);
	
	printf("%s > ", cwd);
}

/**
 * void show_usage()
 *
 * Prints a usage message to the standard output.
 */
void show_usage() {
	printf("usage: tmnsh [filename]\n");
}


/***** Input Functions ******************************************************/

/**
 * int read_data(FILE *stream, char *buffer, int buffer_size)
 *
 * Reads at most buffer_size-1 bytes of input from the given stream into
 * the buffer supplied.
 *
 * NOTE Semicolons are treated as newline characters.
 * NOTE If a line of input is too long to fit in the buffer, it will
 *      NOT result in an overflow, but simply be "chomped" from the
 *      input stream and discarded.
 * NOTE Hash characters ('#') and any following characters up to a
 *      newline or EOF will be discarded - comment your scripts!
 *
 * Returns TRUE if there is more data to read from the stream, FALSE
 * otherwise.
 */
int read_data(FILE *stream, char *buffer, int buffer_size) {
	char character;
	char *buf = buffer;
	int bytes_read = 0;
	
	while (bytes_read <= buffer_size-1) {
		character = fgetc(stream);
		
		if (character == ';' || character == '\n' || character == EOF ||
			character == '#') {
				break;
		}
		
		*buf++ = character;
		bytes_read++;
	}
	
	*buf = '\0';
	
	while ((character != ';') && (character != '\n') && (character != EOF)) {
		character = fgetc(stream);
	}
	
	if (character == EOF) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/***** Main Interpreter Loop ************************************************/

/**
 * void main_loop(FILE *stream, int interactive)
 *
 * Reads, tokenises, parses ane executes input from the given stream. If
 * interactive mode is on, outputs a welcome message and a prompt.
 */
void main_loop(FILE *stream, int interactive) {
	char buffer[BUFFER_MAX_SIZE];
	char last_line[BUFFER_MAX_SIZE];
	int more_to_read = TRUE;
	tokarray_t *tokens;
	expression_t *expr;
	
	bzero(buffer, BUFFER_MAX_SIZE);
	bzero(last_line, BUFFER_MAX_SIZE);
	
	if (interactive == TRUE) {
		show_welcome();
	}
	
	while (more_to_read == TRUE) {
		if (interactive == TRUE) {
			show_prompt();
		}
		
		/* Read a line of input. */
		more_to_read = read_data(stream, buffer, BUFFER_MAX_SIZE);
		
		if (strlen(buffer) == 0) {
			continue;
		}
		
		strcpy(last_line, buffer);
		
		/* Tokenise the input. */
		tokens = tokenise_input(buffer);
		
		if (tokens == NULL) {
			printf("!tmnsh: Could not tokenise input: %s\n", last_line);
			continue;
		}
		
		/* Parse the tokens into an expression. */
		expr = parse_tokens(tokens);
		
		if (expr == NULL) {
			printf("!tmnsh: Could not parse input: %s\n", last_line);
			tokarray_destroy(tokens); /* Clean up. */
			continue;
		}
		
		/* Interpret the expression and execute the commands. */
		interpret_expression(expr);
		
		expression_destroy(expr); /* Clean up. */
		bzero(buffer, BUFFER_MAX_SIZE);
		bzero(last_line, BUFFER_MAX_SIZE);
	}
}


/***** Signal Handlers ******************************************************/

void sigchld_handler(int sig) {
	wait(NULL);
}

void sigint_handler(int sig) {
	fflush(stdout);
}


/***** Main Function ********************************************************/

/**
 * int main(int argc, char *argv[], char *envp)
 *
 * Determines whether to run in interactive mode or to read in
 * expressions from a file.
 */
int main(int argc, char *argv[], char *envp[]) {
	signal(SIGCHLD, sigchld_handler);
	signal(SIGINT, sigint_handler);
	
	if (argc > 2) {
		show_usage();
	} else if (argc == 2) {
		/* Read expressions from a file. */
		FILE *file = fopen(argv[1], "r");
		
		if (file == NULL) {
			printf("!tmnsh: Could not open file '%s'.\n", argv[1]);
			exit(1);
		}
		
		main_loop(file, FALSE);
		fclose(file);
	} else {
		/* Read expressions from standard input. */
		main_loop(stdin, TRUE);
	}
	
	return 0;
}
