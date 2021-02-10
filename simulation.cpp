#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <map>
#include <chrono>
#include <thread>
using namespace std;
pthread_mutex_t mutex;
pthread_mutex_t mutexSeller;
int priorityA = 3;
int priorityB = 2;
int priorityC = 1;
int theNumberOfClients = 0;
int k = 0;
int sell(int index);
pair<int, pair<string, pair<int, int>>> serviceCLients();
void *Seller(void *args);
void *client(void *args);
string *split(const std::string &s, char delim);
long long idofA, idofB, idofC;
string seats[201];
int thenumberofseats;
string seatsForReservation[201];
fstream input,output;
queue<pair</*arrival time*/ int, pair</*name*/ string, pair</*process time */ int, /*seat number*/ int>>>> clientQueue;
int main(int argc, char *argv[])
{
    for (int i = 0; i < 201; i++)
        seats[i] = "";
    int error;
    input.open(argv[1]);
    output.open(argv[2]);
    string theNameofLounge;
    int thenumberofClients;
    string temp;
    getline(input, theNameofLounge);
    stringstream kk(theNameofLounge);
    kk >> theNameofLounge;
    getline(input, temp);
    thenumberofClients = stoi(temp);
    output<<"Welcome to the Sync-Ticket!"<<endl;
    output<<"Teller A has arrived."<<endl<<"Teller B has arrived."<<endl<<"Teller C has arrived."<<endl;
    if (theNameofLounge == "OdaTiyatrosu")
        thenumberofseats = 60;
    else if (theNameofLounge == "UskudarStudyoSahne")
        thenumberofseats = 80;
    else if (theNameofLounge == "KucukSahne")
        thenumberofseats = 200;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexSeller, NULL);

    pthread_t *tids = (pthread_t *)calloc(thenumberofClients, sizeof(pthread_t));

    string *lineCommand = (string *)calloc(4 * thenumberofClients, sizeof(string));

    string A = "A";

    pthread_t SellerThreadA;

    if (error = pthread_create(&SellerThreadA, NULL, &Seller, (void *)&A))
    {
        fprintf(stderr, "Failed to create thread:%s\n", strerror(error));
        return 1;
    }
    idofA = SellerThreadA;
    string B = "B";

    pthread_t SellerThreadB;
    if (error = pthread_create(&SellerThreadB, NULL, &Seller, (void *)&B))
    {
        fprintf(stderr, "Failed to create thread:%s\n", strerror(error));
        return 1;
    }
    idofB = SellerThreadB;
    string C = "C";
    pthread_t SellerThreadC;
    if (error = pthread_create(&SellerThreadC, NULL, &Seller, (void *)&C))
    {
        fprintf(stderr, "Failed to create thread:%s\n", strerror(error));
        return 1;
    }
    idofC = SellerThreadC;
    for (int i = 0; i < thenumberofClients; i++)
    {
        theNumberOfClients++;
        getline(input, temp);
        string *tt = split(temp, ',');
        if (error = pthread_create(tids + i, NULL, client, (void *)tt))
        {
            fprintf(stderr, "Failed to create thread:%s\n", strerror(error));
            return 1;
        }
    }

    pthread_join(SellerThreadA, NULL);
    pthread_join(SellerThreadB, NULL);
    pthread_join(SellerThreadC, NULL);
    for (int i = 0; i < thenumberofClients; i++)
        pthread_join(tids[i], NULL);

    sleep(0.5);
    output<<"All clients received service."<<endl;
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexSeller);
    input.close();
    output.close();
    return 0;
}

string *split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    string *elems = (string *)calloc(4, sizeof(string));
    int i = 0;
    while (std::getline(ss, item, delim))
    {
        elems[i] = item;
        i++;
    }
    return elems;
}
void *client(void *args)
{
    string *params = (string *)args;
    string name = params[0];
    int arrivalTime = stoi(params[1]);
    int processTime = stoi(params[2]);
    int seat = stoi(params[3]);
    std::this_thread::sleep_for(std::chrono::milliseconds(arrivalTime));
    pthread_mutex_lock(&mutex);
    clientQueue.push({arrivalTime, {name, {processTime, seat}}});
    pthread_mutex_unlock(&mutex);
    std::this_thread::sleep_for(std::chrono::milliseconds(processTime));
    pthread_exit(0);
}
void *Seller(void *args)
{
    string ch = *(string *)args;
    //sem_t sellerA = *(sem_t *)args;
    while (clientQueue.empty())
        ;
    while (k < theNumberOfClients)
    {
         if (clientQueue.empty())
                continue;
        pair<int, pair<string, pair<int, int>>> getClient=serviceCLients();
       
        if (getClient == pair<int, pair<string, pair<int, int>>>())
            continue;
            
        clientQueue.pop();
        k++;
        
        
        int index=sell(getClient.second.second.second);

        ch == "A" ? priorityA = 0 : ch == "B" ? priorityB = 0
                                              : priorityC = 0;

        std::this_thread::sleep_for(std::chrono::milliseconds(getClient.second.second.first));

        pthread_mutex_lock(&mutexSeller);

         output << getClient.second.first << " requests seat " << getClient.second.second.second
                     << ", reserves ";
                     if(index==0)
                        output <<"None";
                    else
                        output<<"seat "<<index;
                    output  << "."
                     << " Signed by Teller " << ch <<"."<< endl;

        pthread_mutex_unlock(&mutexSeller);

        ch == "A" ? priorityA = 3 : ch == "B" ? priorityB = 2
                                              : priorityC = 1;
    }
    pthread_exit(0);
}
int sell(int index)
{
    int k=0;
    if (seatsForReservation[index] == "" && thenumberofseats>=index && index>0)
    {
        seatsForReservation[index] = "sold";
        k=index;
    }
    else
    {
        for (int i = 1; i < thenumberofseats; i++)
        {
            
            if (seatsForReservation[i] == "")
            {
                seatsForReservation[i] = "sold";
                k=i;
                break;
            }
            
        }
    }
    return k;
}
pair<int, pair<string, pair<int, int>>> serviceCLients(){
        
        if (pthread_self() == idofA)
        
            return clientQueue.front();
        
        else if (pthread_self() == idofB && priorityB > priorityA)
        
            return clientQueue.front();
        
        else if (pthread_self() == idofC && priorityC > priorityA && priorityC > priorityB)
        
            return clientQueue.front();
        else 
            return pair<int, pair<string, pair<int, int>>>();
}