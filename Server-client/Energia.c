#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "Energia.h"



/*
La funzione assegnazionevalore assegna a ogni elemento la giusta quantità di energia calcolando il rapporto tra l'energia disponibile
e l'energia totale massima di cui ha bisogno
*/
void assegnazionevalore(double energia_disponibile,double totale,double*array_dei_valori,int max,double* corrente_per_elemento)
{
    double rapporto=energia_disponibile/totale;

    for(int i=0;i<max;i++)
    {
        array_dei_valori[i]=(corrente_per_elemento[i]*rapporto);
    }
}


/*void recuperodati(Edificio**array,int max,int*edifici,double*somme_di_energia)//simula il consumo di ciascun edificio dando valori random senza collegarsi a nessun server
{
    srand(time(NULL));
    for(int i=0;i<max;i++)
    {
        for(int j=0;j<edifici[i];j++)
        {
            array[i][j].corrente_cons=rand()%((int)(array[i][j].max_corrente));
            somme_di_energia[i]+=array[i][j].corrente_cons;
        }
    }

}*/

/*La funzione regolazioneconsumo confronta il consumo dell'edificio con quello quello che dovrebbe consumare in media.
Se maggiore riduce il suo consumo diminuendo o eliminando completamente l'eccesso
*/
double regolazioneconsumo(Edificio**array,int quartiere,int edifici,double max_per_quartiere,double energia_disponibile,double differenza,int sockfd)
{
    double* media_consumo=(double*)malloc(edifici*sizeof(double));//array contenente le medie dei consumi per ogni edificio di un quartiere
    int*indici=(int*)malloc(edifici*sizeof(int));
    double*eccessi=(double*)malloc(edifici*sizeof(double));
    double nuovo_consumo=0;

    char buff[MAX];
    char* aux=(char*)malloc(20*sizeof(char));

    int cont=0;

    double eccesso_totale=0;

    assegnazionevalore(energia_disponibile,max_per_quartiere,media_consumo,edifici,&array[quartiere]->max_corrente);//viene calcolata la media del consumo per ogni edificio

    for(int i=0;i<edifici;i++)
    {
        if(array[quartiere][i].corrente_cons>media_consumo[i])//se il consumo supera la media 
        {
           cont++;
           eccesso_totale+=array[quartiere][i].corrente_cons-media_consumo[i];//calcolo dell'eccesso totale
           indici[cont-1]=i;
           eccessi[cont-1]=array[quartiere][i].corrente_cons-media_consumo[i];//viene memorizzato l'eccesso
        }
        else
            nuovo_consumo+=array[quartiere][i].corrente_cons;
    }

    double rapporto_eccessi=differenza/eccesso_totale;
    memset(buff, 0, MAX);
    snprintf(buff,20*sizeof(char),"%d",cont);
    write(sockfd, buff, sizeof(buff));


    for(int i=0;i<cont;i++)
    {
        memset(buff, 0, MAX);
        eccessi[i]=eccessi[i]*rapporto_eccessi;
        //printf("Riduzione consumo EDIFICIO %d QUARTIERE %d da %lfV",indici[i],quartiere,array[quartiere][indici[i]].corrente_cons);
        strcpy(buff,"Riduzione consumo EDIFICIO ");
        snprintf(aux,20*sizeof(char),"%d",indici[i]);
        strcat(buff,aux);
        strcat(buff," QUARTIERE ");
        snprintf(aux,20*sizeof(char),"%d",quartiere);
        strcat(buff,aux);
        strcat(buff," da ");
        snprintf(aux,20*sizeof(char),"%lf",array[quartiere][indici[i]].corrente_cons);
        strcat(buff,aux);
        array[quartiere][indici[i]].corrente_cons-=eccessi[i];//il consumo viene diminuito in rapporto al suo eccesso
        //printf(" a %lfV\n",array[quartiere][indici[i]].corrente_cons);
        strcat(buff," a ");
        snprintf(aux,20*sizeof(char),"%lf",array[quartiere][indici[i]].corrente_cons);
        strcat(buff,aux);
        write(sockfd, buff, sizeof(buff));
        
        nuovo_consumo+=array[quartiere][indici[i]].corrente_cons;
    }
    
    free(aux);
    aux=NULL;
    free(media_consumo);
    media_consumo=NULL;
    free(indici);
    indici=NULL;
    free(eccessi);
    eccessi=NULL;

    return nuovo_consumo;//restituisce il nuovo consumo del quartiere 
}


void memorizzadati(double energia,int numero_registrazioni)//salva i dati riguardanti il consumo medio di energia elettrica totale
{
    FILE*ptr;
    static int tempo=INTERVALLO_TEMPO;
    double media;

    media=energia/numero_registrazioni;

    ptr=fopen("Risultati.txt","a");

    fprintf(ptr,"%d %lf\n",tempo,media);

    fclose(ptr);

    tempo+=INTERVALLO_TEMPO;

}

void Controllo_energia(int sockfd,double somma,double*contatori,int quartieri,double*max_corrente_per_quartiere,int*edifici,Edificio**citta)
{
    char buff[MAX];
    double cont_principale;
    srand(time(NULL));//serve per la simulazione
    cont_principale=ENERGIA_MIN+(rand()%(ENERGIA_MAX-ENERGIA_MIN));//num_minimo+rand()%range(massimo)
    double nuovo_principale=cont_principale;
    assegnazionevalore(cont_principale,somma,contatori,quartieri,max_corrente_per_quartiere);//assegna i valori in base al loro consumo

    size_t time1,time2;
    time(&time1);
    int cont=0;
    double consumo_energia_totale=0;

	//loop infinito per controllare sempre
	while (true) 
    {
        double* energie_cons=(double*)calloc(quartieri,sizeof(double));
        if(cont_principale!=nuovo_principale)//se l'energia della centrale varia bisognerà associare nuovi valori ad ogni contatore del quartiere
        {
            assegnazionevalore(cont_principale,somma,contatori,quartieri,max_corrente_per_quartiere);
            cont_principale=nuovo_principale;
        }
        for(int i=0;i<quartieri;i++)
        {
            for(int j=0;j<edifici[i];j++)
            {
                memset(buff, 0, MAX);//mette a zero  tutto
		        // legge il messaggio dal client e lo copia nel buffer
		        read(sockfd, buff, sizeof(buff));
		        //stampa del messaggio inviato dal client
		        printf("From client: %s\n ", buff);
                citta[i][j].corrente_cons=atof(buff);//memorizzazione della corrente consiumata dall'edificio 
                energie_cons[i]+=citta[i][j].corrente_cons;
                memset(buff, 0, MAX);
		        // copia il messaggio del server nel buffer
                if(i==(quartieri-1)&&j==(edifici[i]-1))
                   strcpy(buff,"control.."); 
                else
                    strcpy(buff,"ok");
		        // e manda il buffer al client
		        write(sockfd, buff, sizeof(buff));
            }
        }


        char* aux=(char*)malloc(sizeof(int));
        for(int i=0;i<quartieri;i++)
        {

            if(energie_cons[i]>contatori[i])//l'energia consumata è maggiore di quello che dovrebbe e quindi bisognerà regolare il consumo per evitare ulteriori danni 
            {
                printf("WARNING\n");
                energie_cons[i]=regolazioneconsumo(citta,i,edifici[i],max_corrente_per_quartiere[i],contatori[i],energie_cons[i]-contatori[i],sockfd);//viene regolato il consumo delle case che stanno consumando più del dovuto   
            }
            else//altrimenti si avvisa il client che è tutto apposto
            {
                memset(buff, 0, MAX);
                strcpy(buff,"0");
                write(sockfd, buff, sizeof(buff));
                strcpy(buff,"Quartiere ");
                snprintf(aux,sizeof(int),"%d",i);
                strcat(buff,aux);
                strcat(buff," stabile");
                write(sockfd, buff, sizeof(buff));
            }
            consumo_energia_totale+=energie_cons[i];
        }
        cont++;
        time(&time2);

        if(difftime(time2,time1)>((INTERVALLO_TEMPO)*60))//stampa i dati dopo un determinato intervallo
        {

            memorizzadati(consumo_energia_totale,cont);
            consumo_energia_totale=0;
            cont=0;
            nuovo_principale=ENERGIA_MIN+(rand()%(ENERGIA_MAX-ENERGIA_MIN));//simula una variazione di energia alla centrale elettrica
            printf("VARIAZIONE ENERGIA ELETTRICA CENTRALE\n");
	    time(&time1);
        }
        free(aux);
        aux=NULL;
        free(energie_cons);
        energie_cons=NULL;

	}
}


