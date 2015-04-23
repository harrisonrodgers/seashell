#### NAME

seashell − a simple shell for you to use written in c

#### EXTERNAL SYNTAX

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`seashell [batchfile]`  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`seashell < batchfile`

#### INTERNAL SYNTAX

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`command [arguments] [< input_file] [>[>] output_file] [2>[>] error_file] [&]`  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`command [arguments] [< input_file] [&>[>] output_and_error_File] [&]`

#### DESCRIPTION

*seashell* is a basic shell. It provides various built-in features, described below, and is able to execute processes found in the directories listed in your PATH environment variable. *seashell* supports input and output redirection, and background execution. Additionally *seashell* can process a batch file, if provided with one, instead of the standard input.

#### ARGUMENTS

A maximum of one argument will be accepted, *seashell* will assume it to be the name of a file containing shell commands, each separated by a newline. The batchfile will be read line by line and the commands executed, when the end-of-file is reached *seashell* will terminate.

#### SHELL GRAMMAR

**Simple Commands**  
Simple commands can be executed by typing in the name of a process, or built-in feature, to be executed followed by the arguments to be provided to the process being executed. When you run a command, if it matches the name of a built-in feature then that feature is executed, else if it matches the name of a process executable stored in a directory listed by the PATH environment variable then that process is executed, else an error message is printed.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Examples:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ date`  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ touch newFile.txt`

There are some special arguments which you can use when constructing a command, these arguments are not passed to the process being executed, rather they are interpreted by *seashell* which performs corresponding advanced features. Please continue reading to learn about these.

**Background Execution**  
Processes can be executed in the background. This means that the shell will not wait for these processes to complete before presenting you with the prompt to input new commands with.

This means you are able to start a long running process and send it to the background, then continue on with your work while the process continues. Please note, any output from a process running in the background will still be displayed to the console.

To execute a process in the background, ensure `&` is at the end of the command, separated by a space.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Examples:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ ping www.google.com -c 10 &`  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ unzip -q ~/downloads/archive.zip &`

**Redirection**  
The files from which a process uses to obtain input from, write standard output to, and write error output to, can be redefined. This allows you to perform cool actions such as saving the output of a process to a file, or passing the contents of a file to a process to use as input.

Input can be redirected using `< inputFile`. Output can be redirected using `> outputFile`. Errors can be redirected using `2> errorFile`. Output and errors can both be redirected using `&> logFile`.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Input redirection:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ cat < dictionary.txt`  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ grep error < logfile.txt`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Output redirection:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ ping www.google.com -c 100 > record.txt`  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ dir > listing.txt`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Error redirection:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ find / -name "homework.c" 2> /dev/null`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Output & Error redirection:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ unzip archive.zip &> log.txt`

Redirecting standard output or error output to a existing file will result in that file loosing all existing data. Using an extra `>` symbol, you can instruct *seashell* to append the output to a file.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Output redirection, with instruction to append:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ ping www.google.com -c 100 >> record.txt`  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ dir > listing.txt`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Error redirection, with instruction to append:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ find / -name "homework.c" 2>> /dev/null`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Output & Error redirection, with instruction to append:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ unzip archive.zip &>> log.txt`

Multiple forms of redirection (input redirection, standard output redirection, error output redirection) can be applied to a single process.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Example:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ cat < input_File > output_file 2> error_file`

#### BUILT IN COMMANDS

Some commands are provided by *seashell*, these are part of the *seashell* process. When you run one of these commands instead of a process with the matching name being executed, *seashell* executes an inbuilt function (which may or may not involve the execution of various external processes).

**cd <directory>**  
Changes the current working directory to the given <directory>. If no argument is provided, the name of the current working directory is printed. There are some special directory names which can be used:

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`.` = current directory  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`..` = previous directory  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`~` = path specified by the HOME environment variable  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`-` = path specified by the OLDPWD environment variable

Appropriate error messages will be printed if issues are encountered with the given directory (does not exist, is not a directory, permission denied, ...). The appropriate environment variables, PWD and OLDPWD, will be updated accordingly.

**clr**  
Clears output currently displayed on the screen.

**dir [options] [directory]**  
Lists the contents of the arguments provided assuming they are directories. If no argument is provided, the contents of the current directory is printed. This command is an alias of ls, and passes the arguments -a and -l, you can learn more about ls, and it’s options, by executing ’man ls’. Note: the output of dir can be redirected.

**environ**  
Lists all of the environment variables. Each variable is displayed on a separate line in the form of `’variable=value’`. See the MISC > Environment Variables section for more info.

**echo [arguments]**  
Displays the arguments provided to the screen followed by a new line. Note: the output of echo can be redirected.

**help**  
Displays this user manual (located in the directory of the shell binary) using man, and displayed using less, Note: the output of help can be redirected.

**pause**  
Pauses the operation of *seashell* until <Enter> is pressed.

**quit**  
Terminates the execution of *seashell*.

#### MISC

**Environment Variables**  
These variables hold information such as, preferred text editor, home directory, language, current working directory ...

When executed each process is given an almost identical copy of their parent’s (the process that executed it) environment variable. The environment variables provided by the parent might have minor differences to the parent’s environment variables.

*seashell* alters specific environment variables. These changes help ensure the processes that it starts have the appropriate information required. These environment variables include:  

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`PWD` - the current working directory  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`OLDPWD` - the previous current working directory  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`SHELL` - the path to the seashell executable

#### AUTHOR

Harrison Rodgers <hrod1137@uni.sydney.edu.au>

#### SEE ALSO

**sh**(1), **csh**(1), **tcsh**(1), **bash**(1), **zsh**(1)
