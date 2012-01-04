TEENAGE MUTANT NINJA SHELL v 1.0.0
==================================

Copyright 2011, Evan Christopher Davis <ecdavis@wtfrak.com>

Teenage Mutant Ninja Shell (TMNSH) is licensed under the Apache License,
Version 2.0, see LICENSE.

Compile & Run
-------------

To compile and run TMNSH, execute the following commands:

    unzip tmnsh-1.0.0.zip
    cd tmnsh-1.0.0.zip
    make
    ./tmnsh

To execute TMNSH shell scripts, simply supply the filename to the program
at the command line. Some example scripts are provided in the scripts
directory:

    ./tmnsh scripts/teapot.sh

Summary
-------

The key observation I made while working on my design for this assignment
if that a shell is similar - at a high level, at least - to an interpreter
for a simple programming language. It reads in text, parses that text
into a form it recognises and then evaluates the resultant expression.

The core of TMNSH is the main_loop() function in src/tmnsh.c. This
function will continuously read data, parse it and execute it until there
is no more data to read.

Input data is read from either a file or the standard input as whole lines.
A line of input is a string of characters terminated by a newline,
semicolon or EOF character. If a hash character (#) is found anywhere on
a line it and all following characters are discarded - this allows us to
place comments in scripts. Blank lines of input are ignored. Lines are
read into a buffer. If the length of the line exceeds the size of the
buffer, all remaining data on the line is consumed and discarded.

Once a line of input has been read, the next step is to tokenise it. The
tokenising function uses the strtok() function to separate the line into
individual tokens separated by spaces. The tokens are stored in a custom
data structure called a "tokarray" which is simply an array of strings
with two fields specifying the current number of tokens and the maximum
number of tokens that the tokarray can contain.

After the input line has been tokenised, the resultant tokarray is passed
to a parsing function. The parser creates an expression data structure and
populates it with pointers to command data structures. A command is a
sequences of arguments, i.e. "ls -a /", while an expression is a sequence
of commands separated by pipes, i.e. "ls -a / | grep usr". Once parsed
into an expression data structure, the latter example can be visualised
as: [["ls", "-a", "/"], ["grep", "usr"]]

The expression returned by the parsing function is finally passed to the
interpreter function. The interpreter loops over the expression's commands,
running each one in turn. Commands are passed to the run_builtin_command()
function first. If no corresponding built-in command can be found, fork()
is called and the command is run using execvp(). The shell will then wait
for the process to complete unless the expression is being run in the
background.

Once this process is completed, TMNSH will either try to read and
interpret another line of input - thereby beginning the process again -
or will exit if there is no more input to be read.

Testing
-------

The testing I did for this assignment was largely focused on two areas:
testing for overflow/off-by-one errors and ensuring memory management
worked as expected. I did this by developing a range of test inputs with
extensive code coverage and running them, fixing any errors that emerged
and repeating the process. I also made sure to output large amounts of
debug data so that I could read through what the shell was doing as it
tokenised and parsed user input. This helped me identify a number of
issues with the way I was allocating and deallocating memory and led to
a more robust final program than I would otherwise have had.

Extensions
----------

 - Run processes in background. Entering an expression with the ampersand
   (&) as the final token will ensure that all commands in that expression
   are run in the background.
 - Built-in commands. TMNSH has two built-in commands: cd and quit. The
   former allows the user to change the current working directory while
   latter allows the user to quit the shell (since ^C is disabled, see
   below). Due to the way that built-in commands are implemented, it is
   trivial to add more.
 - File execution. TMNSH can read and execute shell scripts from a file as
   well as the standard input.
 - Basic signal handling. The shell effectively ignores the SIGINT signal
   and will call wait() when the SIGCHLD signal is raised. This means
   that it is possible to use ^C to exit a child process without exiting
   the shell and that child processes that have finished running in the
   background will not hang around as "zombie" processes.
 - Multiple commands can exist in the same expression when separated by
   the pipe (|) character. Note that I/O pipelining is NOT implemented,
   the commands are simply run in sequence.
 - Comments and line-termination. TMNSH will ignore any input following a
   hash (#) character, treating it as a comment. Multiple expressions can
   exist on the same line when separated by semi-colons, i.e. ls -a;ps -x

Most of these extensions are small and were relatively simple to implement,
but they do a lot to make TMNSH feel like a "real" shell. The ability to
change the current working directory is particularly important for a shell,
as is the ability to run processes in the background. Being able to
execute shell scripts from a file makes it easier to develop complex
scripts for a shell and it is far more convenient to simply execute a file
than enter an extensive script into a shell. Correct signal handling is
important for many programs, not just shells, but when dealing with child
processes it is not just important, but necessary to ensure that your
program acts correctly.

The "sugar" extensions I implemented - the ability to have multiple
expressions per line, multiple commands per expression and to use comments
in scripts - make writing scripts much easier. Allowing multiple commands
per expression is a first step towards an implementation of I/O pipelining
and file redirection.

Limitations
-----------

 - The lack of I/O pipelining and file redirection is a significant
   limitation of TMNSH. These are some of the most powerful features of
   most shells and also rate among the most useful.
 - The lack of line editing abilities makes using the shell in "interactive"
   mode much less pleasant than using something like bash or tcsh. The
   GNU readline library would have made it easy to implement line editing
   and command history, but I instead opted to write my own code rather
   than use a library.
 - The tokeniser's use of the strtok() function is a problem. It means
   that tokens must be separated by spaces, so while "foo | bar" would be
   tokenised into ["foo", "|", "bar"], "foo|bar" would be treated as a
   single token. This is contrary to how most shells work.
 - Finally, the way the parser works makes it difficult to identify syntax
   quickly. Using something like Lex/YACC to generate the parsing code
   would have produced a much better parser, but as with line editing I
   decided to write my own code.
