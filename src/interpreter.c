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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "expression.h"
#include "interpreter.h"
#include "tmnsh.h"


/***** Interpreter **********************************************************/

/**
 * void interpret_expression(expression_t *expr)
 *
 * Given an expression, this function will execute each command in the
 * expression before either waiting for all the commands to finish or
 * returning immediately (if the expression's background flag is TRUE).
 */
void interpret_expression(expression_t *expr) {
	int builtin_run;
	int index;
	pid_t pid;
	
	for (index = 0; index < expr->num_cmds; index++) {
		builtin_run = interpret_builtin_command(expr->cmds[index]);
		
		if (builtin_run == TRUE) {
			continue;
		}
		
		pid = interpret_command(expr->cmds[index]);
		if (!expr->background) {
			waitpid(pid, NULL, 0);
		}
	}
}

/**
 * int interpret_builtin_command(command_t *cmd)
 *
 * Returns TRUE if a builtin command is found and executed. FALSE
 * otherwise.
 */
int interpret_builtin_command(command_t *cmd) {
	int result;
	
	/* cd - The change directory command. */
	if (strcasecmp(cmd->argv[0], "cd") == 0) {
		result = chdir(cmd->argv[1]);
		
		if (result == -1) {
			printf("!tmnsh: cd - %s (%d)\n", strerror(errno), errno);
		}
		
		return TRUE; /* Builtin command found. */
	}
	
	/* quit - The quit command. */
	if (strcasecmp(cmd->argv[0], "quit") == 0) {
		exit(0);
		return TRUE; /* Builtin command found - here for cleanliness. */
	}
	
	return FALSE;
}

/**
 * pid_t interpret_command(command_t *cmd)
 *
 * Creates a child process by calling fork() and then runs the given
 * command in that child process by calling execvp().
 *
 * Returns the ID of the child process running the given command.
 */
pid_t interpret_command(command_t *cmd) {
	int result;
	pid_t pid;
	
	pid = fork();
	if (pid == 0) {
		result = execvp(cmd->argv[0], cmd->argv);
		
		if (result == -1) {
			printf("!tmnsh: %s - %s (%d)\n", cmd->argv[0], strerror(errno),
			 		errno);
		}
		
		exit(1); /* This will exit the CHILD process. */
		
		return pid;
	} else {
		return pid;
	}
}
