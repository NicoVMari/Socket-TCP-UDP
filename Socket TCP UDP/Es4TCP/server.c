#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <stdbool.h>

const int MAX_LENGTH = 512;

bool is_numeric(const char *str, char *errorChar)
{
  size_t len = strlen(str);

  for (size_t i = 0; i < len; i++)
  {
    if (str[i] == '\n')
    continue;
    else if (str[i] < '0' || str[i] > '9')
    {
      *errorChar = str[i];
      return false;
    }
  }

  int num;
  if (sscanf(str, "%d", &num) != 1) {
    *errorChar = '\0';
    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {

	int simpleSocket = 0;
	uint16_t simplePort = 0;
	int returnStatus = 0;
	struct sockaddr_in simpleServer;

	if (2 != argc) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (simpleSocket == -1) {
		fprintf(stderr, "Could not create a socket!\n");
		exit(1);
	}
	else {
	 fprintf(stderr, "Socket created!\n");
 	}

	simplePort = (uint16_t)atoi(argv[1]);

	memset(&simpleServer, '\0', sizeof(simpleServer)); 
	simpleServer.sin_family = AF_INET;
	simpleServer.sin_addr.s_addr = htonl(INADDR_ANY);
	simpleServer.sin_port = htons(simplePort);

	returnStatus = bind(simpleSocket,(struct sockaddr *)&simpleServer,sizeof(simpleServer));

	if (returnStatus == 0) {
		fprintf(stderr, "Bind completed!\n-----------------------------\n");
	}
	else {
		fprintf(stderr, "Could not bind to address!\n");
		close(simpleSocket);
		exit(1);
	}

	returnStatus = listen(simpleSocket, 5);

	if (returnStatus == -1) {
		fprintf(stderr, "Cannot listen on socket!\n");
		close(simpleSocket);
		exit(1);
	}

	while (1)
	{

		struct sockaddr_in clientName = { 0 };
		int simpleChildSocket = 0;
		socklen_t clientNameLength = sizeof(clientName);

		/* wait here */
		simpleChildSocket = accept(simpleSocket,(struct sockaddr *)&clientName, &clientNameLength);
		if (simpleChildSocket == -1) {

			fprintf(stderr, "Cannot accept connections!\n");
			close(simpleSocket);
			exit(1);

		}
		else
			fprintf(stderr, "Connections accept!\n");

		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientName.sin_addr), clientIP, INET_ADDRSTRLEN);

		char welcomeMessage[MAX_LENGTH];
		snprintf(welcomeMessage, sizeof(welcomeMessage), "Hello student with IP, calcolo il massimo: %s\n", clientIP);

		write(simpleChildSocket, welcomeMessage, strlen(welcomeMessage));

		char buffer[MAX_LENGTH];
		ssize_t bytesRead = read(simpleChildSocket, buffer, sizeof(buffer) - 1);

		if ((bytesRead > 0 && bytesRead < MAX_LENGTH - 1))
		{
			buffer[bytesRead] = '\0';

			char *token = strtok(buffer, " "), errorChar;
   			int index = 0, max = 0;
	        bool isNum;
	        double data[MAX_LENGTH];


	        while (token != NULL)
	        {
	          char temp[MAX_LENGTH];
	          strcpy(temp, token);



	          isNum = is_numeric(temp, &errorChar);
	          if (!isNum)
	          {
	            fprintf(stderr,"\nERRORE Input non valido!\n\n");
	            char errCharMessage[MAX_LENGTH];
	            write(simpleChildSocket, errCharMessage, strlen(errCharMessage));
	            break;
	          }


	          double num = atof(token);
	          data[index] = num;
	          index++;
	          token = strtok(NULL, " ");
	        }

	        for(int i = 0; i < index; i++){
	        	if(data[i] > max)
	        		max = data[i];
	        }

	    	char response[MAX_LENGTH];
          	snprintf(response, sizeof(response), "OK STATS %d\n", max);
          	fprintf(stderr,"\nDati mandati al client: %d\n\n",max);
          	write(simpleChildSocket, response, strlen(response));
		}
		

		close(simpleChildSocket);
		fprintf(stderr, "Connections close!\n-----------------------------\n");

	}

	close(simpleSocket);
	return 0;

}

