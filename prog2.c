//Jakub Kowalczyk 304168 Zadanie programistyczne nr.6
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
typedef struct Client
{
    struct Client *nextClient;
    struct Client *prevClient;
    int timeInQueue;
    int currentServiceTime;
    int serviceTime;
}Client;
typedef struct Window
{
    struct Queue *queue;
    struct Client *firstClient;
    int ID;
    int peopleServed;
    int sumServiceTime;
}Window;
typedef struct Queue
{
    Client *firstClient;
    Client *lastClient;
    Window *window;
    unsigned int numberOfClients;
}Queue;
void printClients(Window **window,int numberOfWindows)   //Wypisanie kolejek
{
    Window **windows=window;
    for(int j=0;j<numberOfWindows;j++)
    {
        printf("---------------------\n");
        printf("Window %d Clients Left: %d Clients served: %d AVG %.3lf: \n",windows[j]->ID,windows[j]->queue->numberOfClients,windows[j]->peopleServed,(double)windows[j]->sumServiceTime/(double)windows[j]->peopleServed);
        Client *temp=windows[j]->queue->firstClient;
        if(temp!=NULL)
        {
            while(temp->prevClient!=NULL)
            {
                printf("Client %p: Service Time: %d Current Service Time:%d Time in Queue: %d\n",temp,temp->serviceTime,temp->currentServiceTime,temp->timeInQueue);
                temp=temp->prevClient;
            }
            printf("Client %p: Service Time: %d Current Service Time:%d Time in Queue: %d\n",temp,temp->serviceTime,temp->currentServiceTime,temp->timeInQueue);
        }
    }
    printf("---------------------\n");
}
void timeOfService(Client *client,gsl_rng *r)   //Losowanie czasu obslugi klienta
{
    int time=round((4+gsl_ran_gaussian(r,2)));
    if(time<1)
    {
        client->serviceTime=1;
    }
    else
    {
        client->serviceTime=time;
    }    
}
void createAndDistributeClients(int numberOfClients,Window **windows,int numberOfWindows,gsl_rng *r)
{
    if(numberOfClients>0)
    {
        Window **tempWindow=windows;
        Queue *minQueue=tempWindow[0]->queue;
        int min=tempWindow[0]->queue->numberOfClients;
        for(int i=0;i<numberOfClients;i++)   // Szukanie najkrotszej kolejki
        {
            for(int j=0;j<numberOfWindows;j++)
            {
                if(tempWindow[j]->queue->numberOfClients<min)
                {
                    min=tempWindow[j]->queue->numberOfClients;
                    minQueue=tempWindow[j]->queue;
                }
            }
            Client *client=(Client*)malloc(sizeof(Client)); //tworzenie klientow
            client->timeInQueue=0;
            client->currentServiceTime=0;
            client->serviceTime=0;
            client->prevClient=NULL;
            client->nextClient=NULL;
            timeOfService(client,r);
            //printf("service time:%d \n",client->serviceTime);
            client->currentServiceTime=0;
            if(minQueue->lastClient!=NULL)
            {
                minQueue->lastClient->prevClient=client;
                client->nextClient=minQueue->lastClient;
            }
            else
            {
                minQueue->firstClient=client;
            }    
            minQueue->lastClient=client;
            minQueue->numberOfClients++;
            min=minQueue->numberOfClients;
            //printf("client entered window: %d \n",minQueue->window->ID);
            //printClients(windows,numberOfWindows);
        }
    }
}
void deQueueClients(Queue *queue) //wypuszczenie obsluzonego klienta z systemu
{
    if(queue->firstClient!=NULL)
	{
	Client *tempClient;
    queue->window->sumServiceTime+=(queue->firstClient->serviceTime)+(queue->firstClient->timeInQueue);
    //printf("deQuequing client %p SumTime:%d\n",queue->firstClient,(queue->firstClient->serviceTime)+(queue->firstClient->timeInQueue));
    tempClient=queue->firstClient;    
    if(queue->firstClient->prevClient!=NULL)
    {
        queue->firstClient=queue->firstClient->prevClient;
        queue->firstClient->nextClient=NULL;
    }
    else
    {
        queue->firstClient=NULL;
        queue->lastClient=NULL;
    }
    free(tempClient);
    queue->numberOfClients--;
	}
}
void incrementTimeOfService(Window **window,int numberOfWindows) //inkrementacja czasu symulacji
{
    Client *tempClient;
    for(int i=0;i<numberOfWindows;i++)
    {
        if(window[i]->queue->firstClient!=NULL)
        {
            window[i]->queue->firstClient->currentServiceTime++;
            //printf("%p: currentServiceTime: %d \n",window[i]->queue->firstClient,window[i]->queue->firstClient->currentServiceTime);
            if(window[i]->queue->firstClient->prevClient!=NULL)
            {
                tempClient=window[i]->queue->firstClient->prevClient;
            }
            if(window[i]->queue->firstClient->currentServiceTime>=window[i]->queue->firstClient->serviceTime) //sprawdzenie czy ktorys z klientow nie zostal obsluzony
            {
                deQueueClients(window[i]->queue);
                window[i]->peopleServed++;
                //printClients(window,numberOfWindows);
            }
            if(window[i]->queue->firstClient!=NULL)
            {
                tempClient=window[i]->queue->firstClient;
                if(tempClient->prevClient!=NULL)
                {
                    tempClient=tempClient->prevClient;
                    while(tempClient->prevClient!=NULL)
                    {
                        tempClient->timeInQueue++;
                        tempClient=tempClient->prevClient;
                    }
                    tempClient->timeInQueue++;
                }
            }
        }
    }
}
void createWindows(int numberOfWindows,Window *windows[]) //tworzenie wybranej liczby okienek
{
    int j=0;
    for(int i=0;i<numberOfWindows;i++)
    {
        Window *window;
        window=(Window*)malloc(sizeof(Window));
        windows[i]=window;
        Queue *queue;
        queue=(Queue*)malloc(sizeof(Queue));
        queue->numberOfClients=0;
        queue->window=window;
        window->ID=++j;
        window->queue=queue;
        window->peopleServed=0;
        window->sumServiceTime=0;
    }
}
void destruktor(int numberOfwindows,Window *windows[]) //dealokacja pamieci
{
    Client *tempClient;
    for(int i=0;i<numberOfwindows;i++)
    {
        if(windows[i]->queue->firstClient!=NULL)
        {
        while(windows[i]->queue->firstClient!=windows[i]->queue->lastClient)
        {
            tempClient=windows[i]->queue->firstClient->prevClient;
            free(windows[i]->queue->firstClient);
            windows[i]->queue->firstClient=tempClient;
        }
        free(windows[i]->queue->firstClient);
        }
        free(windows[i]->queue);
        free(windows[i]);
    }
}
int main()
{
    int simulationTime,numberOfWindows,sigma,average,currentSimulationTime=0,checkState;
    FILE *file; 
    file=fopen("WindowsTimeAverageSigmaCheck","r"); //wyczytanie danych sterujacych z pliku
    if (file==NULL)
    {
        printf("Error! Can't open file to read.");
    }
    else
    {
        fscanf(file,"%d",&numberOfWindows);
        fscanf(file,"%d",&simulationTime);
        fscanf(file,"%d",&average);
        fscanf(file,"%d",&sigma);
        fscanf(file,"%d",&checkState);
        printf("Number of windows: %d , Simulation Time: %d , Average: %d , Sigma: %d, Check the state of windows every: %d\n",numberOfWindows,simulationTime,average,sigma,checkState);
    }
    srand(time(NULL));
    Window *windows[numberOfWindows];
    gsl_rng *r=gsl_rng_alloc(gsl_rng_taus);
    createWindows(numberOfWindows,windows);
    while(currentSimulationTime<simulationTime)
    {
        if(currentSimulationTime%checkState==0)
        {
            printClients(windows,numberOfWindows);
        }
        double clientsEntered=(average+gsl_ran_gaussian(r,sigma));
        if(clientsEntered<0) clientsEntered=0;
        //printf("Clients entered:%.lf\n",round(clientsEntered));
        createAndDistributeClients(round(clientsEntered),windows,numberOfWindows,r);
        clientsEntered=0;
        incrementTimeOfService(windows,numberOfWindows);
        currentSimulationTime++;
    }
    printClients(windows,numberOfWindows);
    destruktor(numberOfWindows,windows);
    fclose(file);
    return 0;
}