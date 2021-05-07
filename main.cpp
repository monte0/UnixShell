#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

const int MAX_LINE = 80;

void printIntro();
std::string getString();
void checkRepeat(std::string s[MAX_LINE/2 + 1], int isnew, int &back);
void parseString(std::string input, std::string s[MAX_LINE/2 + 1], int &size);
void printCommands(std::string s[MAX_LINE/2 + 1]);
int findRedirection(std::string s[]);

int main(void)
{
  char* c[MAX_LINE/2 + 1];//arguments to pass into execvp
  std::string s[MAX_LINE/2 + 1];//agruments from user in string array
  int total = 0;//number of arguments in command
  int keep_going;//if 1, then keep going in the loop, if 0 go to top of loop
  int isNew = 1;//keeps track if there is history or not, so far its new, so no history
  std::string one;//get entire command from user

  printIntro();
  pid_t pid;


  while(1)
    {
      keep_going = 1;
      one = getString();//get user input

      if(one == "exit")
        {
          //exit program
	  std::cout << "exiting" << std::endl;
          exit(1);
        }
      else if(one == "!!")//user wants to repeat previous command
	{
	  checkRepeat(s, isNew, keep_going);
	  if(keep_going == 0)
	    {
	      //go to beginning of loop
	      continue;
	    }
	}
      else//a new command was entered
	{//update arguments in string array, then copy into char* array
	  parseString(one, s, total);
          for(int i = 0; i < total; i++)//store arguments from string array to char*
	    {
	      c[i] = (char*)s[i].c_str();
	    }
          c[total] = NULL;
	  if(s[total - 1] == "&")
            {
              c[total - 1] = NULL;
            }
	}

      pid = fork();
      if(pid < 0)//error
	{
	  std::cout << "Fork failed" << std::endl;
	}
      else if(pid == 0)//child
	{
	  int num = findRedirection(s);//check if user wants to redirect input/output
	  isNew = 0;//there will be a previous command after this
	  
	  if(num == -1)//no redirection was involved
	    {
	      if(execvp(c[0], c) < 0) //perform execvp()
                {
		  std::cout << "Error in execvp" << std::endl;
                }
	    }
	  else if(s[num] == "<")//read from a file, <
	    {
	      int fd0 = open(c[total - 1], O_RDONLY);
	      dup2(fd0, 0);
	      c[num] = NULL;
	      if(execvp(c[0], c) < 0) //perform execvp()
                {
		  std::cout << "Error in execvp" << std::endl;
                }
	    }
	  else//write to a file, >
	    {
	      int fd1 = open(c[total - 1], O_WRONLY | O_APPEND);
	      dup2(fd1, 1);
	      c[num] = NULL;
	      if(execvp(c[0], c) < 0) //perform execvp()
                {
		  std::cout << "Error in execvp" << std::endl;
                }
	    }
	}
      else//parent
	{
	  if(s[total - 1] == "&")//if user entered &
	    {
	      std::cout << "I am the Parent" << std::endl << std::endl;
	      wait(NULL);
	    }
	  else
	    {
	      wait(NULL);
	      isNew = 0;
	    }
	}
    }
  return 0;
}
//Print basic information about the program
void printIntro()
{
  std::cout << "CS 433 Programming Assignment 2" << std::endl;
  std::cout << "Author: Miguel Cea" << std::endl;
  std::cout << "Date: 03/05/2020" << std::endl;
  std::cout << "Course: CS433 (Operating Systems)" << std::endl;
  std::cout << "Description: Program to implement a shell interface" << std::endl;
  std::cout << "==================================" << std::endl;
}

//return input from user
std::string getString()
{
  std::string input;
  std::cout << "osh>";
  std::getline(std::cin, input);
  return input;
}

//user just entered !! and wants to repeat previous command
//is isNew is 1 then there is no previous command history
//if its 0 then output previous command to screen then exit
//to run execvp
void checkRepeat(std::string s[MAX_LINE/2 + 1], int isnew, int &back)
{
  if(isnew == 1)
    {//there is no previous command
      std::cout << "No command history" << std::endl;
      back = 0;
      return;
    }
  else//there was a previous command, print it, and then return to execvp
    {
      printCommands(s);
      return;
    }
}

//breaks up the string input from user into arguments and stores them
//in an array. input is the entire command that user entered.
//s is the array that holds the arguments. Size is the number 
//of arguments the command has.
void parseString(std::string input, std::string s[MAX_LINE/2 + 1], int &size)
{
  int t = 0;//numbers of arguments in input
  //break up string input into arguments
  char* token;
  char* copy = (char*)input.c_str();
  token = strtok_r(copy, " ", &copy);
  while(token != NULL)
    {
      s[t] = token;//store arguments into string array
      token = strtok_r(NULL, " ", &copy);
      t++;
    }
  s[t] = "\0";
  size = t;//return size of arguments to parameter
}

//prints an array, which holds the arguments
void printCommands(std::string s[MAX_LINE/2 + 1])
{
  int x = 0;
  while(s[x] != "\0")
    {
      std::cout << s[x] << " ";
      x++;
    }
  std::cout << std::endl;
}

//checks to see if the user wants to redirect output/input or not
//functions goes through array to find < or > and returns the index
//it is at. if no < or > is found then it returns -1.
int findRedirection(std::string s[])
{
  int i = 0;
  while(s[i] != "\0")
    {
      if(s[i] == ">")
	{
	  return i;
	}

      if(s[i] == "<")
	{
	  return i;
	}
      i++;
    }
  return -1;
}
