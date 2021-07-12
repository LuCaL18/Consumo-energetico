#ifdef _WIN32
//For Windows
int betriebssystem = 1;
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <ws2def.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int betriebssystem = 2;
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "Energia.h"

#define PORT 8080
#define SA struct sockaddr

int main()
{

	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli; 
    int edifici;

    double *contatori=(double*)malloc(QUARTIERI*sizeof(double));//array con i valori dell'energia per ogni contatore associato a un quartiere
    int num_edifici[QUARTIERI]={7,5,6,5,7};//array che contiene il numero di edifici per quartiere


    /* La città è vista come una matrice aventi come righe il numero di quartieri e come colonne il numero di edifici per quartiere.
    Ogni riga può quindi essere maggiore, minore o uguale alle altre; ogni cella rappresenta un edificio che ha come valori il massimo di energia che può consumare 
    e l'energia che sta consumando attualmente*/
    Edificio**mappa=(Edificio**)malloc(QUARTIERI*sizeof(Edificio*));
        for(int i=0;i<QUARTIERI;i++)
            mappa[i]=(Edificio*)malloc(num_edifici[i]*sizeof(Edificio));

    double somma=0;
    double *max_corrente_per_quartiere=(double*)malloc(QUARTIERI*sizeof(double));//array con i massimi consumi di ogni contatore per quartiere

    for(int i=0;i<QUARTIERI;i++)
    {
		mappa[i][0].max_corrente=CORRENTE_MAX;
        max_corrente_per_quartiere[i]=mappa[i][0].max_corrente;
        for(int j=1;j<num_edifici[i];j++)
        {
            mappa[i][j].max_corrente=CORRENTE_MAX;//legge i Volt massimi che può utilizzare ogni casa
            max_corrente_per_quartiere[i]+=mappa[i][j].max_corrente;
        }

        somma+=max_corrente_per_quartiere[i];
    }


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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Collegare il socket appena creato all'IP dato e alla verifica
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Ora il server è pronto per ascoltare
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accettare il pacchetto di dati dal cliente e la verifica
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server acccept failed...\n");
		exit(0);
	}
	else
		printf("server acccept the client...\n");

	// Funzione per la comunicazione tra il server e il client e il controllo dell'energia
	Controllo_energia(connfd,somma,contatori,QUARTIERI,max_corrente_per_quartiere,num_edifici,mappa);

	// chiusura del socket
	close(sockfd);
}
