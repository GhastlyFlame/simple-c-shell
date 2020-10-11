#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/*Ethan McGill CprE 308 Project 1 Simple Shell*/

int main(int argc, char * argv[]){

  char * prompt = "308sh> ";

  int bgprocess = 0;
  int bgpid;
  int bgstatus;
  int j;
  int spaces;

  if(argc != 1 && argc != 3){
    perror("Too few or too little arguments.\n");
  }
  else if(argc ==3 ){
    if(strcmp(argv[1], "-p") != 0){
      perror("Usage: ./308sh [-p] <Your text here>\n");
    } else {
      prompt = argv[2];
      prompt = strcat(prompt, "> ");
    }
  }

  //Continuously run our shell until the uer tells us to stop
  while(1){

    char buffer[256];
    unsigned long size = 256;

    //Print out the prompt
    printf("%s", prompt);

    //**This chunk of code separates the argument string into an argument array**
    char charArray[1024];
    fgets(charArray, 1024, stdin);

    //Make sure the string is not just a new space, otherwise it will go out of bounds
    if(charArray[0] != '\n'){
      charArray[strlen(charArray)-1] = '\0';
    }

    int arrayLen = strlen(charArray);

    int spaces =0; //This variable keeps track of the number of spaces found in the arg string, so I know how large to make the argument array
    for(j =0; j<arrayLen; j++){
      if(charArray[j] == ' '){
        spaces++;
      }
    }

    //Initialize the argument array to have space for a null at the end
    char* command[(spaces+2)];
    //Initialize the entire array to be null to start
    for(j =0; j<spaces+2; j++){
      command[j] = NULL;
    }

    int i = 0;

    //Cut the string string up into individual array indexes
    char * cut = strtok(charArray, " ");
    while(cut != NULL){
        command[i] = cut;
        cut = strtok(NULL, " ");
        i++;
    }

    //Check if a background process has changed
    bgpid = waitpid(-1, &bgstatus, WNOHANG);

    //If we detect that a process changed, print that a background process exited with the correct code
    if(bgpid && bgprocess > 0){
      if(WIFEXITED(bgstatus)){
        printf("[%d] Exit (%d) \n", bgpid, WEXITSTATUS(bgstatus));
        bgprocess--;
      }
    }

    //Handle when the user inputs exit
    if((strcmp(command[0], "exit") == 0) && (strcmp(command[spaces], "&") != 0)){
      exit(0);
    }

    //Handle when the user inputs pid
    else if((strcmp(command[0], "pid") ==0) && (strcmp(command[spaces], "&") != 0)){
      printf("%d\n", getpid());
    }

    //Handle when the user inputs ppid
    else if((strcmp(command[0], "ppid") == 0) && (strcmp(command[spaces], "&") != 0)){
      printf("%d\n", getppid());
    }

    //Handle when the user inputs cd
    else if((strcmp(command[0], "cd") == 0) && (strcmp(command[spaces], "&") != 0)){
      //Check if only cd was inputted, if so cd to the home directory
      if(spaces == 0){
        char * homedir = getenv("HOME");
        chdir(homedir);
        //Otherwise cd to the given link
      } else {
        chdir(command[1]);
      }
    }

    //Handle when the user inputs pwd
    else if((strcmp(command[0], "pwd") == 0) && (strcmp(command[spaces], "&") != 0)){
      buffer[256];
      size = 256;
      getcwd(buffer, size);
      printf("%s\n", buffer);
    }

    //Handle when a user kills a process by id
    else if((strcmp(command[0], "kill") == 0) && (strcmp(command[spaces], "&") != 0)){

      pid_t pid;
      int status;
      int child;
      //Create the command argument for execvp
      char* cmd[] = {
        "kill",
        command[1],
        NULL
      };
      int killed;

      //fork to create a new process for killing the specified process
      if((pid = fork()) <0 ){
        printf("Error forking\n");
        exit(1);
      }
      else if(pid == 0){
        //Run the kill command on the specified process
        if(execvp("kill", cmd) <0){
          printf("Unknown Command\n");
          exit(1);
        }
      } else {

        //Print out that the process started
        printf("[%d] %s\n", pid, command[0]);

        //Wait for the process to finish
        while(wait(&status) != pid);

      }
      //Print that the kill process has exited
      printf("[%d] %s Exit %d \n", pid, command[0], WEXITSTATUS(status));
      //Wait for the process to be killed and prinft the killed status
      waitpid(-1, &killed, 0);
      printf("[%d] Killed (%d) \n", atoi(command[1]), WTERMSIG(killed));
      //Tell the program that we have one less background process running
      bgprocess--;
    }

    //Handle when a user executes an unimplimented command
    else if((strcmp(command[spaces], "&") != 0) &&(strcmp(command[0], "\n"))){

      pid_t pid;
      int status;
      char* cmd;
      char temp[256];

      //Fork to a new process to call execvp
      if((pid = fork()) <0 ){
        printf("Error forking\n");
        exit(1);
      }
      else if(pid == 0){
        //try to execute the command given by the user
        if(execvp(command[0], command) <0){
          printf("Cannot exec %s: No such file or directory\n", command[0]);
          exit(255);
        }
      } else {

        //If it was sucessful, this code will get only the name of command that was executed
        int slashes =0;
        //Count the number of slashes that were in the given command
        for(j=0; j<strlen(command[0]); j++){
          if(command[0][j] == '/'){
            slashes++;
          }
        }

        //If there was more than one slash strip them away to leave only the command executed
        if(slashes >1){
          strcpy(temp, command[0]);
          cmd=strtok(temp, "/");
          //printf("%s\n", cmd);
          for(j=0; j<slashes-1; j++){
            cmd=strtok(NULL, "/");
          }
        //If there was one, strip it away and leave only the command executed
        }
        else if(slashes == 1){
          strcpy(temp, command[0]);
          cmd=strtok(temp, "/");
        }
        //Otherwise there was no slashes and you only inputed the command that was ran
        else {
          cmd = command[0];
        }

        //Print that the process started
        printf("[%d] %s\n", pid, cmd);

        //Wait for the process to end before moving on
        while(wait(&status) != pid);
      }

      //Print when the process has exited
      printf("[%d] %s Exit %d \n", pid, cmd, WEXITSTATUS(status));

    }

    //Handle when a user inputs to run a command in the background
    else if((strcmp(command[spaces], "&") == 0)){

      pid_t pid;
      char temp[256];
      char *cmd;
      command[spaces] = NULL;

      //Fork to a new process to run execvp on
      if((pid = fork()) <0 ){
        printf("Error forking\n");
        exit(1);
      }

      else if(pid == 0){
        //Trying to execute the command that was given by the user
        if(execvp(command[0], command) <0){
          printf("Unknown Command\n");
          exit(1);
        }
      } else {

        //If the command is successfully ran we want to print out just the name of the command that was given
        int slashes =0;
        //Count the number of slashes that were given with the command
        for(j=0; j<strlen(command[0]); j++){
          if(command[0][j] == '/'){
            slashes++;
          }
        }

        //IF there were more than one slashes included in the command strip them array leaving only the command
        if(slashes >1){
          strcpy(temp, command[0]);
          cmd=strtok(temp, "/");
          for(j=0; j<slashes-1; j++){
            cmd=strtok(NULL, "/");
          }
        }

        //If there was one slash strip it away leaving only the command
        else if(slashes == 1){
          strcpy(temp, command[0]);
          cmd=strtok(temp, "/");
        }

        //Otherwise there were no slashes and we just want to print the given command
        else {
          cmd = command[0];
        }

        //We want to print the process if and name of the process
        printf("[%d] %s\n", pid, cmd);

        //DONT wait for the process to finish, and tell the shell that we have +1 background processes now
        bgprocess++;
      }
    }

    }
}
