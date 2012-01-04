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

/***** Defines **************************************************************/

#define COMMAND_MAX_ARGV 513
#define EXPRESSION_MAX_CMDS 513


/***** Structures ***********************************************************/

/* Command Structure */
typedef struct command_s {
	int num_args;
	int max_args;
	char *argv[COMMAND_MAX_ARGV];
	} command_t;

/* Expression Structure */
typedef struct expression_s {
	int background;
	int num_cmds;
	int max_cmds;
	command_t *cmds[EXPRESSION_MAX_CMDS];
	} expression_t;


/***** Function Declarations ************************************************/

/* Command Functions */
command_t *command_create();
void command_destroy(command_t *cmd);
int command_argv_push(command_t *cmd, char *arg);
int command_argv_pop(command_t *cmd);

/* Expression Functions */
expression_t *expression_create();
void expression_destroy(expression_t *expr);
int expression_cmd_push(expression_t *expr, command_t *cmd);
int expression_cmd_pop(expression_t *expr);
