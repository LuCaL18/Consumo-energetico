#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include "Energia.h"


int main()
{
    FILE *ptr;
    int quartieri;
    int edifici;

    double cont_principale;

    srand(time(NULL));//serve per la simulazione

    ptr=fopen("Dati.csv","r");//il file contiene il numero di quartieri e di case della città con i rispettivi valori massimi di consumo 

    fscanf(ptr,"%d",&quartieri);//numero quartieri della città
    double *contatori=(double*)malloc(quartieri*sizeof(double));//array con i valori dell'energia per ogni contatore associato a un quartiere
    int *num_edifici=(int*)malloc(quartieri*sizeof(int));
    char* linea=(char*)malloc(quartieri*sizeof(char));
    fscanf(ptr,"%s",linea);
    edifici=atoi(strtok(linea,";"));

    for(int j=0;j<quartieri;j++)
    {
        num_edifici[j]=edifici;//per ogni quartiere viene letto il numero di edifici
        edifici=atoi(strtok(NULL,";"));
    }
    
    /* La città è vista come una matrice aventi come righe il numero di quartierie come colonne il numero di edifici per quartiere.
    Ogni riga può quindi essere maggiore, minore o uguale alle altre; ogni cella rappresenta un edificio che ha come valori il massimo di energia che può consumare 
    e l'energia che sta consumando attualmente*/
    Edificio**mappa=(Edificio**)malloc(quartieri*sizeof(Edificio*));
        for(int i=0;i<quartieri;i++)
            mappa[i]=(Edificio*)malloc(num_edifici[i]*sizeof(Edificio));

    double somma=0;
    double *max_corrente_per_quartiere=(double*)malloc(quartieri*sizeof(double));//array con i massimi consumi di ogni contatore per quartiere

    for(int i=0;i<quartieri;i++)
    {
        fscanf(ptr,"%s",linea);
        mappa[i][0].max_corrente=atoi(strtok(linea,";"));
        max_corrente_per_quartiere[i]=mappa[i][0].max_corrente;
        for(int j=1;j<num_edifici[i];j++)
        {
            mappa[i][j].max_corrente=atoi(strtok(NULL,";"));//legge i Volt massimi che può utilizzare ogni casa
            max_corrente_per_quartiere[i]+=mappa[i][j].max_corrente;
        }

        somma+=max_corrente_per_quartiere[i];
    }
    free(linea);
    linea=NULL;
    fclose(ptr);//chiusura del file

    cont_principale=ENERGIA_MIN+(rand()%(ENERGIA_MAX-ENERGIA_MIN));//num_minimo+rand()%range(massimo)
    double nuovo_principale=cont_principale;
    assegnazionevalore(cont_principale,somma,contatori,quartieri,max_corrente_per_quartiere);//assegna i valori in base al loro consumo

   size_t time1,time2;
   time(&time1);
   int cont=0;
   double consumo_energia_totale=0;

    while(true)//il programma funziona all'infinito perchè deve monitorare sempre la situazione
    {
        double* energie_cons=(double*)calloc(quartieri,sizeof(double));
        if(cont_principale!=nuovo_principale)//se l'energia della centrale varia bisognerà associare nuovi valori ad ogni contatore del quartiere
        {
            assegnazionevalore(cont_principale,somma,contatori,quartieri,max_corrente_per_quartiere);
            cont_principale=nuovo_principale;
        }
        recuperodati(mappa,quartieri,num_edifici,energie_cons);//simula la raccolta dei dati raccolti dai sensori

        for(int i=0;i<quartieri;i++)
        {
            if(energie_cons[i]>contatori[i])//l'energia consumata è maggiore di quello che dovrebbe e quindi bisognerà regolare il consumo per evitare ulteriori danni 
            {
                printf("WARNING\n");
                energie_cons[i]=regolazioneconsumo(mappa,i,num_edifici[i],max_corrente_per_quartiere[i],contatori[i],energie_cons[i]-contatori[i]);//viene regolato il consumo delle case che stanno consumando più del dovuto
                printf("\n");
                
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
            nuovo_principale=ENERGIA_MIN+(rand()%(ENERGIA_MAX-ENERGIA_MIN));
            time(&time1);

        }

        free(energie_cons);
        energie_cons=NULL;

        


    }

    free(max_corrente_per_quartiere);
    max_corrente_per_quartiere=NULL;
    free(contatori);
    contatori=NULL;
    free(num_edifici);
    num_edifici=NULL;
    free(mappa);
    mappa=NULL;
    return 0;
}