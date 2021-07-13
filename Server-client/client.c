#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Energia.h"
#include <stdbool.h>
#define PORT 8080
#define SA struct sockaddr


void func(int sockfd)
{
	char buff[MAX];
	double buffy;
	int n=0;

	while (true) 
	{
		memset(buff, 0, MAX);
		buffy = rand()%(CORRENTE_MAX+1);//simula il consumo di ciascun edificio dando valori random
		snprintf(buff,MAX,"%lf",buffy);
		write(sockfd, buff, sizeof(buff));//viene mandato al server il valore del consumo
		memset(buff, 0, MAX);
		read(sockfd, buff, sizeof(buff));//lettura della risposta del server (deve essere 'ok' se non ci sono stati errori)

		if ((strncmp(buff, "control..", 9)) == 0)//se il server invia control significa che sta controllando il consumo
		{
			for(int j=0;j<QUARTIERI;j++)
			{
				memset(buff, 0, MAX);
				read(sockfd, buff, sizeof(buff));//il server invia un numero
				n=atoi(buff);

				if(n!=0)//se è diverso da zero vuol dire che ci sono edifici che superano il limite
				{

					for(int i=0;i<n;i++)
					{
						memset(buff, 0, MAX);
						read(sockfd, buff, sizeof(buff));//il server indica le variazioni
						printf("From Server : %s\n", buff);
					}
					n=0;
				}
				else//se è uguale a zero vuol dire che non ci sono problemi
				{
					memset(buff, 0, MAX);
					read(sockfd, buff, sizeof(buff));
					printf("From Server : %s\n", buff);
				}
			}

		}
		else if ((strncmp(buff, "ok", 2)) != 0) //se il server non invia ne ok ne control il client chiude la comunicazione
		{
			printf("Client Exit...\n");
			break;
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// creazione del socket e verifica
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	memset(&servaddr, 0, sizeof(servaddr));

	// assegnazione IP e PORTA
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Collegare il socket appena creato all'IP dato e alla verifica
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// funzione per comunicare con il server
	func(sockfd);

	// chiusura del socket
	close(sockfd);
	
	return 0;
}
