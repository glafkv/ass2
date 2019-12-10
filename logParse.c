#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>

char errMsg[256];

char *subsetSumHandler(int sum, int n, int *subset)
{
	//will hold the results of trying to find a subset sum
	static char message[1024];
	//clear the static string
	sprintf(message, "%c", '\0');
	
	if(sum == 0)
	{
		sprintf(message, "0 is equal to 0");
		return message;
	}
	
	int isSum;
	//trying to find sum
	isSum = subsetSum(sum, subset, n, message);
	if(isSum == 0)
	{
		//it failed
		sprintf(message, "No subset of numbers summed to %d.", sum);
	}
	else
	{
		//get the string in the format of x + x +
		int len;
		len = strlen(message);
		//replace the last + with =
		message[len-2] = '=';
		
		//add sum after =
		char temp[14];
		sprintf(temp, "%d%c\n", sum, '\0');
		strncat(message, temp, 1024-len);
	}
	return message;
}
int subsetSum(int sum, int *subset, int n, char *msg)
{
	//base cases
	if(sum == 0)
		return 1;
	else if(n == 0 && sum != 0)
		return 0;
	
	//if last number of curren tarray is greater than sum, don't include it and continue
	if(subset[n-1] > sum)
		return subsetSum(sum, subset, n-1, msg);
	
	//find the subset with the current last element
	int equals;
	equals = subsetSum(sum - subset[n-1], subset, n-1, msg);
	//subset is found with current last element
	if(equals > 0)
	{
		//save element in msg
		char temp[16];
		snprintf(temp, 13, "%d + ", subset[n-1]);
		strncat(msg, temp, 1024 - strlen(msg));
		//recursive
		return equals;
	}
	//if no subset with current last eleemnet was found, try to find one without
	int notEquals;
	notEquals = subsetSum(sum, subset, n-1, msg);
	if(notEquals > 0)
		return notEquals; //recurse
	
	//if no subset, return failure
	return 0;
}

int main(int argc, char *argv[])
{
	//files and getopt variables
	int option, time = 10, inputTime;
	char *input;
	char *output;
	
	//set default file names
	input = "input.dat";
	output = "output.dat";
	
	while((option = getopt(argc, argv, "hi:o:t:")) != -1)
	{
		switch(option)
		{
			case 'h':
				printf("\tHelp menu\n");
				printf("-h : prints help menu\n");
				printf("-i : pick your input file. default is input.dat\n");
				printf("-o : pick your output file. default is output.dat\n");	
				printf("-t : pick the max time. default is 10 seconds\n");
				exit(0);
				break;
			case 'i':
				input = optarg;
				break;
			case 'o':
				output = optarg;
				break;
			case 't':
				inputTime = atoi(optarg);
				if(inputTime > 0)
					time = inputTime;
				break;
			default:
				printf("Error\n");
				exit(1);
		}
	}
	
	//file open code
	FILE *outfile;
	FILE *infile;
	outfile = fopen(output, "w");
	infile = fopen(input, "r");
		
	//check if files exist
	if(infile == NULL)
	{
		perror("logParse.c: Error: ");
		exit(EXIT_FAILURE);
	}
	if(outfile == NULL)
	{
		perror("logParse.c: Error: ");
		exit(EXIT_FAILURE);
	}
	//string parsing variables
	int numLines;
	char line[1024];
	char str[1024];
	//pid string conversion variables
	char pidString[10];
	char endString[100];
	
	//parent child variables
	int status, pid, childPID;
	
	//get the number of lines that are in the file and convert for the loop
	numLines = atoi(fgets(line, sizeof(line), infile));
	
	//close the input file for now
	fclose(infile);
	
	int i;
	for(i = 1; i < numLines + 1; i++)
	{
		pid = fork();
		//error
		if(pid < 0)
		{
			perror("logParse.c: Error: ");
		}
		else if(pid == 0) //child area
		{
			int pid = getpid();
	
			//open th einput file
			infile = fopen(input, "r");
			//get the next line
			fgets(line, sizeof(line), infile);
			
			//loop through the line
			int j;
			for(j = 0; j < i; j++)
			{
				fgets(line, sizeof(line), infile);
			}
			//close file
			fclose(infile);
	
			//parsing through the lines
			char *token;
			static int nums[128];
			int i = 2;
			//parse at end of the line
			token = strtok(line, "\n");
			//parse at spaces
			token = strtok(line, " ");
			
			if(token != NULL)
				nums[1] = atoi(token); //number of lines
			
			//advance the strtok
			token = strtok(NULL, " ");
			//loop through the rest of the line and store the line
			while(token != NULL && i < 128)
			{
				nums[i++] = atoi(token); //store token as a number
				token = strtok(NULL, " "); //advance the token
			}

			//set first element of array as number of elements in set
			nums[0] = i - 2;
			
			//parse the numbers
			int *subset;
			int sum;
			int n;
			subset = nums+2;
			sum = nums[1];
			n = nums[0];
			
			//get back the printed messages
			char *message;
			message = subsetSumHandler(sum, n, subset);
		
			fprintf(outfile, "%d: %s\n", pid, message);
			exit(0);
		}
		else
		{
			waitpid(pid, &status, 0);
			//for printing all child pids after file processing
			sprintf(pidString, "%d ", pid);
			strcat(endString, pidString);
		}
	}
	//print all the children and the parent
	fprintf(outfile, "Parent PID: %d\n", getpid());
	fprintf(outfile, "All children: %s\n", endString);
	
	//close the file
	fclose(outfile);
	
	return 0;
}



