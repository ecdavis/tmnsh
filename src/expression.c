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

#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "tmnsh.h"


/***** Command Functions ****************************************************/

/**
 * command_t *command_create()
 *
 * Returns a pointer to a new command structure. The pointer must be
 * passed to command_destroy() once the command is no longer needed.
 */
command_t *command_create() {
	int index;
	command_t *cmd = malloc(sizeof(command_t));
	
	cmd->num_args = 0;
	cmd->max_args = COMMAND_MAX_ARGV;
	
	for (index = 0; index < cmd->max_args; index++) {
		cmd->argv[index] = NULL;
	}
	
	return cmd;
}

/**
 * void command_destroy(command_t *cmd)
 *
 * Safely destroys a command structure by removing all the argument
 * strings before passing the structure pointer to free().
 */
void command_destroy(command_t *cmd) {
	while (cmd->num_args > 0) {
		command_argv_pop(cmd);
	}
	
	free(cmd);
}

/**
 * int command_argv_push(command_t *cmd, char *arg)
 *
 * Appends a string argument to the given command's argument array. If
 * the command has reached the maximum number of arguments this function
 * will be unsuccessful.
 *
 * Returns 0 if successful, -1 if unsuccessful.
 */
int command_argv_push(command_t *cmd, char *arg) {
	int index = cmd->num_args;
	int size = sizeof(char) * strlen(arg) + 1;
	
	if (index >= cmd->max_args) {
		return -1;
	}
	
	cmd->argv[index] = malloc(size);
	bzero(cmd->argv[index], size);
	strcpy(cmd->argv[index], arg);
	cmd->num_args++;
	
	return 0;
}

/**
 * int command_argv_pop(command_t *cmd)
 *
 * Removes the last string argument in the given command's argument array.
 * If the argument array is empty this function will be unsuccessful.
 *
 * Returns 0 if successful, -1 if unsuccessful.
 */
int command_argv_pop(command_t *cmd) {
	int index = cmd->num_args - 1;
	
	if (index < 0) {
		return -1;
	}
	
	free(cmd->argv[index]);
	cmd->argv[index] = NULL;
	cmd->num_args--;
	
	return 0;
}


/***** Expression Functions *************************************************/

/**
 * expression_t *expression_create()
 *
 * Returns a pointer to a new expression structure. The pointer must be
 * passed to expression_destroy() once the expression is no longer
 * needed.
 */
expression_t *expression_create() {
	expression_t *expr = malloc(sizeof(expression_t));
	int index;
	
	expr->background = FALSE;
	expr->num_cmds = 0;
	expr->max_cmds = EXPRESSION_MAX_CMDS;
	
	for (index = 0; index < expr->max_cmds; index++) {
		expr->cmds[index] = NULL;
	}
	
	return expr;
}

/**
 * void expression_destroy(expression_t *expr)
 *
 * Safely destroys an expression structure by destroying all the cmmand
 * structures before passing the structure pointer to free().
 */
void expression_destroy(expression_t *expr) {
	while (expr->num_cmds) {
		expression_cmd_pop(expr);
	}
	
	free(expr);
}

/**
 * int expression_cmd_push(expression_t *expr, command_t *cmd)
 *
 * Appends a command structure to the given expressions's command array.
 * If the expression has reached the maximum number of command this
 * function will be unsuccessful.
 *
 * Returns 0 if successful, -1 if unsuccessful.
 */
int expression_cmd_push(expression_t *expr, command_t *cmd) {
	int index = expr->num_cmds;
	
	if (index >= expr->max_cmds) {
		return -1;
	}
	
	expr->cmds[index] = cmd;
	expr->num_cmds++;
	
	return 0;
}

/**
 * int expression_cmd_pop(expression_t *expr)
 *
 * Removes the last command structure in the given expression's command
 * array. If the command array is empty this function will be
 * unsuccessful.
 *
 * Returns 0 if successful, -1 if unsuccessful.
 */
int expression_cmd_pop(expression_t *expr) {
	int index = expr->num_cmds - 1;
	
	if (index < 0) {
		return -1;
	}
	
	command_destroy(expr->cmds[index]);
	expr->cmds[index] = NULL;
	expr->num_cmds--;
	
	return 0;
}
