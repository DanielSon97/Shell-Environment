### What is a shell?

sshell is a simple shell program. The core features of sshell include the 
implementation of a few select builtin commands ("exit", "pwd", "cd"), 
the redirection of standard output or standard error that can be written to 
files, and the piping of multiple commands together. 

### High Level Overview

In a nutshell, the program can be divided into three parts: the main 
function, the command line parser, and the three launchers. 

Upon execution, the main function immediately calls the command line parser 
function, which loops indefinitely until the user chooses to exit. This 
function obtains the command line, parses it, and then chooses from one of 
three appropriate launchers:

* Launcher for commands involving pipes
* Launcher for commands involving output redirection
* Launcher for all other commands

#### Part 1 of 3: The Main Function

The main function calls the command line parser function, awaits for its 
completion, and obtains its return code. A return code of 0 would solely 
indicate that the user chose to quit, and if so, the main function would 
print "bye..." before terminating the shell.

#### Part 2 of 3: The Command Line Parser

This function loops indefinitely until the user chooses to exit. On each 
iteration, it prompts the user for the command line, parses it, sets the 
appropriate flag, and calls the appropriate launcher. A flag of 1 indicates 
that it should call the output redirection launcher, a 2 indicates that it 
should use the piping launcher, and a 3 indicates that it should use 
the regular launcher.

For cases 1 and 3, strtok() is used exclusively to separate the commands 
from the arguments. This is simple to do since these types of strings only 
involve a single command. We then store the command and the argument strings
into an array of strings, in the 0th and 1st positions, respectively.

For case 2 involving pipes, since these strings involve multiple command 
lines, using strtok() alone would be very tricky to do. In this case we 
loop through each character in the string and once the metacharacter is hit,
we save this metacharacter and this index, and copy the command line which
sits in between the previous metacharacter and this current metacharacter.
Rinse and repeat until the null terminator is reached. In this case, we copy
the final command line which sits in between the previous metacharacter
and this current null terminator.

#### Part 3 of 3: Launchers

##### Launcher: Output Redirection

In this launcher, we use the fork, wait, and exec approach in order to launch
commands. The difference between this launcher and the regular launcher is 
that in this launcher, instead of writing to standard output (e.g., the
terminal), the standard output or standard error can instead be redirected 
to the specified file by utilizing close() and dup(). A flag is passed into
this launcher which determines whether the standard output or the standard
error is to be redirected.

##### Launcher: Regular Launcher

In this launcher, we use the fork, wait, and exec approach in order to launch
commands that do not involve output redirection or piping. Built-in functions
"pwd" and "cd" are implemented by using getcwd() and chdir() respectively. In
the case of "cd", chdir() seems to have no effect when executed in the child
process. Thus, "cd" is executed in the parent process. All other commands are
launched via execvp(). 

##### Launcher: Pipeline

This function always has four arrays passed in, even if the actual number 
of command lines entered by the user is less. The parser is the one that
determines the actual number of command lines, by counting the number of
metacharacters that occur in the user input string, and passes this number
to the piping function. E.g., 2 pipes would indicate 3 command lines.

The approach to the piping of commands is that the parent process creates
all the pipes first and then waits for all the child processes to terminate.
There was an initial issue where we would get blank outputs or that the
shell prompt would only return if we pressed enter after the output. This
problem was that the waiting process wasn't implemented correctly. In order
to sort the children in order (and to get the return codes of each in order),
we put wait(status) in a loop, and simultaneously get the return code via
WEXITSTATUS(status).

Finally, since we can't return arrays in C, we append "1" to the return codes,
and return this number.

### Testing
Since the bulk of the program relies a lot on the parser, a great deal of time
was spent using printf() to ensure that strings were parsed correctly. Since 
our development environment was on Ubuntu, testing can easily be done on the
terminal itself as opposed to working on the CSIF via SSH.

### Ideas Used From External Sources
* https://stackoverflow.com/questions/5457608/how-to-remove-the-character-at-a-given-index-from-a-string-in-c
* https://brennan.io/2015/01/16/write-a-shell-in-c/
* https://stackoverflow.com/questions/6205195/given-a-starting-and-ending-indices-how-can-i-copy-part-of-a-string-in-c