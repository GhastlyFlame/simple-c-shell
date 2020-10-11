# simple-c-shell

Project Requirements

1. Your shell should accept a -p <prompt> option on the command line when it is initiated. The <prompt> option should become the user prompt string. If this option is not specified, the default prompt of “308sh> ” should be used.
  
2. Your shell should run as an infinite loop accepting input from the user and running commands until the user requests to exit. 

3. Your shell should support two types of user commands: (1) built-in commands, and (2) program commands. 
(a)Built-in commands
• exit – the shell should terminate and accept no further input from the user 
• pid – the shell should print its process ID 
• ppid – the shell should print the process ID of its parent 
• cd <dir> – change the working directory of the shell process. With no arguments, change to the user’s home directory (which is stored in the environment variable HOME) 
• pwd – print the current working directory 
(b)Program commands require your shell to spawn a child process to execute the user input (exactly as typed) using the execvp() call. This means a user command will be entered either using an absolute path, a path relative to the current working directory, or a path relative to a directory listed in the PATH environment variable.
  
4. The shell should notify the user if the requested command is not found or fails for any reason. 

5. When executing a program command (not a built-in command), print out the creation and exit status information for child processes: 
(a) When a child process is spawned, print the process ID (PID) before executing the specified command. It should only be printed once and it must be printed before any output from the command. You can include the program name if you find it useful. 
(b) When a child process is finished, print out its PID and exit status. Revisit Lab 2 for a refresher on how to handle exit status.

6. Your shell should support background program commands using suffix “&.” By default, the shell should block (wait for the child to exit) for each command. Thus the prompt will not be available for new user input until the command has completed. However, if the command ends with suffix “&,” the child process should run in the background, meaning the shell will immediately prompt the user for further input without waiting for the child process to finish. You still need to print out the creation and exit status of these background processes.
