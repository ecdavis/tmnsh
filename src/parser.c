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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "parser.h"
#include "tmnsh.h"


/***** Tokarray Functions ***************************************************/

/**
 * tokarray_t *tokarray_create()
 *
 * Returns a pointer to a new token array structure. The pointer must be
 * passed to tokarray_destroy() once the tokarray is no longer needed.
 */
tokarray_t *tokarray_create() {
	tokarray_t *tokens = malloc(sizeof(tokarray_t));
	int index;
	
	tokens->num_tokens = 0;
	tokens->max_tokens = TOKARRAY_MAX_TOKENS;
	
	for (index = 0; index < tokens->max_tokens; index++) {
		tokens->tokens[index] = NULL;
	}
	
	return tokens;
}

/**
 * void tokarray_destory(tokarray_t *tokens)
 *
 * Safely destroys a token array structure by removing all the token
 * strings before passing the structure pointer to free().
 */
void tokarray_destroy(tokarray_t *tokens) {
	while (tokens->num_tokens > 0) {
		tokarray_token_pop(tokens);
	}
	
	free(tokens);
}

/**
 * int tokarray_token_push(tokarray_t *tokens, char *token)
 *
 * Appends a string argument to the given tokarray's token array. If
 * the tokarray has reached the maximum number of tokens this function
 * will be unsuccessful.
 *
 * Returns 0 if successful, -1 if unsuccessful.
 */
int tokarray_token_push(tokarray_t *tokens, char *token) {
	int index = tokens->num_tokens;
	int size = sizeof(char) * strlen(token) + 1;
	
	if (index >= tokens->max_tokens) {
		return -1;
	}
	
	tokens->tokens[index] = malloc(size);
	bzero(tokens->tokens[index], size);
	strcpy(tokens->tokens[index], token);
	tokens->num_tokens++;
	
	return 0;
}

/**
 * int tokarray_token_pop(tokarray_t *tokens)
 *
 * Removes the last token argument in the given tokarray's token array.
 * If the token array is empty this function will be unsuccessful.
 *
 * Returns 0 if successful, -1 if unsuccessful.
 */
int tokarray_token_pop(tokarray_t *tokens) {
	int index = tokens->num_tokens - 1;
	
	if (index < 0) {
		return -1;
	}
	
	free(tokens->tokens[index]);
	tokens->tokens[index] = NULL;
	tokens->num_tokens--;
	
	return 0;
}


/***** Tokeniser ************************************************************/

/**
 * tokarray_t *tokenise_input(char *buffer)
 *
 * Breaks the given string into tokens, placing them into a tokarray.
 *
 * Returns a pointer to a new token array structure. The pointer must be
 * passed to tokarray_destroy() once the tokarray is no longer needed.
 */
tokarray_t *tokenise_input(char *buffer) {
	char *token;
	tokarray_t *tokens = tokarray_create();
	
	token = strtok(buffer, " ");
	while (token != NULL && tokens->num_tokens != tokens->max_tokens) {
		tokarray_token_push(tokens, token);
		
		token = strtok(NULL, " ");
	}
	
	if (token != NULL) {
		return NULL;
	}
	
	return tokens;
}


/***** Parser ***************************************************************/

/**
 * expression_t *parse_tokens(tokarray_t *tokens)
 *
 * Parses the given tokens into commands, placing each command into a
 * single expression structure.
 *
 * NOTE Commands are separated by the pipe character ('|').
 *
 * Returns a pointer to a new expression structure. The pointer must be
 * passed to expression_destroy() once the expression is no longer
 * needed.
 */
expression_t *parse_tokens(tokarray_t *tokens) {
	expression_t *expr = expression_create();
	command_t *cmd = command_create();
	int index = 0;
	int syntax_error = FALSE;
	
	if (tokens->num_tokens == 0) {
		printf("!tmnsh: Cannot parse a tokarray with 0 tokens.\n");
		expression_destroy(expr);
		command_destroy(cmd);
		return NULL;
	}
	
	/* Parse commands into the expression structure. */
	while (index < tokens->num_tokens) {
		/* Commands are separated in an expression by pipes. */
		if (strcmp(tokens->tokens[index], "|") == 0) {
			if (cmd->num_args < 1) {
				syntax_error = TRUE;
				break;
			} else {
				expression_cmd_push(expr, cmd);
				cmd = command_create();
			}
		} else {
			command_argv_push(cmd, tokens->tokens[index]);
		}
		
		index++;
	}
	
	/* Handle syntax errors. */
	if (syntax_error) {
		printf("!tmnsh: Syntax error at token %d: %s\n", index, tokens->tokens[index]);
		/* Clean up. */
		expression_destroy(expr);
		command_destroy(cmd);
		return NULL;
	}
	
	/* Check to see if this is a background expression. */
	if (cmd->num_args > 1 && strcmp(cmd->argv[cmd->num_args-1], "&") == 0) {
		expr->background = TRUE;
		command_argv_pop(cmd);
	}
	
	/* Check to ensure the most recent command is valid. */
	if (cmd->num_args < 1) {
		printf("!tmnsh: Syntax error at token %d: %s\n", index-1, tokens->tokens[index-1]);
		/* Clean up. */
		command_destroy(cmd);
		return NULL;
	} else {
		expression_cmd_push(expr, cmd);
	}
	
	/* Check to ensure the expression is valid. */
	if (expr->num_cmds < 1) {
		/* Clean up. */
		expression_destroy(expr);
		return NULL;
	}
	
	return expr;
}
