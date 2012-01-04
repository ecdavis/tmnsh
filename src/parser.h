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

#define TOKARRAY_MAX_TOKENS 513


/***** Structures ***********************************************************/

typedef struct tokarray_s {
	int num_tokens;
	int max_tokens;
	char *tokens[TOKARRAY_MAX_TOKENS];
	} tokarray_t;

/***** Function Declarations ************************************************/

/* Tokarray Functions */
tokarray_t *tokarray_create();
void tokarray_destroy(tokarray_t *tokens);
int tokarray_token_push(tokarray_t *tokens, char *token);
int tokarray_token_pop(tokarray_t *tokens);

/* Tokeniser */
tokarray_t *tokenise_input(char *buffer);

/* Parser */
expression_t *parse_tokens(tokarray_t *tokens);
