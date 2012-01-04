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

#define TRUE 1
#define FALSE 0

#define CWD_MAX_SIZE 513
#define BUFFER_MAX_SIZE 4097

/***** Function Declarations ************************************************/

/* CLI Functions */
void show_welcome();
void show_prompt();
void show_usage();

/* NOTE The below two declarations are commented out because expression.h
 *      has a hissy fit about the FILE type when compiling and including
 *      the relevant header in this header doesn't seem to address that.
 */
/* Input Functions */
/*int read_data(FILE *stream, char *buffer, int buffer_size);*/

/* Main Interpreter Loop */
/*void main_loop(FILE *stream, int interactive);*/

/* Main Function */
int main(int argc, char *argv[], char *envp[]);
