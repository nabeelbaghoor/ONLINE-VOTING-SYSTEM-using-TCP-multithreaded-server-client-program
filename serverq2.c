/*
        TCP_Server. This Program will will create the Server side for TCP_Socket Programming.
        It will receive the data from the client and then send the same data back to client.
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <pthread.h>
#include <semaphore.h>

sem_t mutex;
int numberOfClients = 0;
int numberOfClientsAllowed = 5;
struct candidates _candidates;
struct voters _registeredVoters;
struct votes _votes;
//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
struct candidates
{
    char **candidateNames;
    char **candidatePollSymbols;
    int numberOfCandidates;
};
void allocateCandidates(struct candidates *_candidate, int totalCandidates)
{
    _candidate->candidateNames = (char **)malloc(totalCandidates * sizeof(char *));
    _candidate->candidatePollSymbols = (char **)malloc(totalCandidates * sizeof(char *));
    _candidate->numberOfCandidates = 0;
}
void addCandidate(struct candidates *_candidate, char *Name, char *symbol)
{
    _candidate->candidateNames[_candidate->numberOfCandidates] = (char *)malloc((strlen(Name) + 1) * sizeof(char));
    strcpy(_candidate->candidateNames[_candidate->numberOfCandidates], Name);
    _candidate->candidatePollSymbols[_candidate->numberOfCandidates] = (char *)malloc((strlen(symbol) + 1) * sizeof(char));
    strcpy(_candidate->candidatePollSymbols[_candidate->numberOfCandidates], symbol);
    _candidate->numberOfCandidates++;
}
void loadCandidates(struct candidates *_candidates)
{
    char *data = (char *)malloc(500 * sizeof(char));
    readStringFromFile(data, "Candidates_List.txt");
    char *curr, *parser, *name, *symbol, *prevSpace;
    curr = data;
    parser = data;
    while ((*curr) != 0)
    {
        while (*(curr) != ' ' && (*curr++) != 0)
            ;
        prevSpace = curr;
        name = parser;
        parser = ++curr;
        while (*(curr) != ' ' && *(curr) != '\n' && (*curr++) != 0)
            ;
        if ((*curr) == ' ')
        {
            *curr = 0;
            parser = ++curr;
            while (*(curr) != '\n' && (*curr++) != 0)
                ;
            *curr = 0;
            symbol = parser;
            parser = ++curr;
            symbol[strlen(symbol) - 1] = 0;
        }
        else
        {
            *prevSpace = 0;
            *curr = 0;
            symbol = parser;
            parser = ++curr;
        }
        addVoter(_candidates, name, symbol);
    }
}
int verifyCandidateSymbol(struct candidates *_candidate, char *symbol)
{
    for (int i = 0; i < _candidate->numberOfCandidates; i++)
    {
        if (!strcmp(_candidate->candidatePollSymbols[i], symbol))
            return 1;
    }
    return 0;
}
char *getCandidateName(struct candidates *_candidate, char *symbol)
{
    for (int i = 0; i < _candidate->numberOfCandidates; i++)
    {
        if (!strcmp(_candidate->candidatePollSymbols[i], symbol))
            return _candidate->candidateNames[i];
    }
    return "";
}
void printCandidates(struct candidates *_candidate)
{
    printf("Name\t\tSymbol\n");
    for (int i = 0; i < _candidate->numberOfCandidates; i++)
    {
        printf("%s\t%s\n", _candidate->candidateNames[i], _candidate->candidatePollSymbols[i]);
    }
}
//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
struct voters
{
    char **voterNames;
    char **voterCNICs;
    int numberOfVoters;
};
void allocateVoters(struct voters *_voters, int totalVoters)
{
    _voters->voterNames = (char **)malloc(totalVoters * sizeof(char *));
    _voters->voterCNICs = (char **)malloc(totalVoters * sizeof(char *));
    _voters->numberOfVoters = 0;
}
void addVoter(struct voters *_voters, char *Name, char *CNIC)
{
    _voters->voterNames[_voters->numberOfVoters] = (char *)malloc((strlen(Name) + 1) * sizeof(char));
    strcpy(_voters->voterNames[_voters->numberOfVoters], Name);
    _voters->voterCNICs[_voters->numberOfVoters] = (char *)malloc((strlen(CNIC) + 1) * sizeof(char));
    strcpy(_voters->voterCNICs[_voters->numberOfVoters], CNIC);
    _voters->numberOfVoters++;
}

void loadVoters(struct voters *_voters)
{
    char *data = (char *)malloc(500 * sizeof(char));
    readStringFromFile(data, "Voters_List.txt");
    char *curr, *parser, *name, *cnic;
    curr = data;
    parser = data;
    while ((*curr) != 0)
    {
        while (*(curr) != '/' && (*curr++) != 0)
            ;
        *curr = 0;
        name = parser;
        parser = ++curr;
        while (*(curr) != '\n' && (*curr++) != 0)
            ;
        *curr = 0;
        cnic = parser;
        parser = ++curr;
        cnic[strlen(cnic) - 1] = 0;
        addVoter(_voters, name, cnic);
    }
}
int verifyVoter(struct voters *_voters, char *Name, char *CNIC)
{
    int isVerified = 0;
    for (int i = 0; i < _voters->numberOfVoters; i++)
    {
        if (!strcmp(_voters->voterNames[i], Name) && !strcmp(_voters->voterCNICs[i], CNIC))
        {
            isVerified = 1;
            break;
        }
    }
    return isVerified;
}
void printVoters(struct voters *_voters)
{
    printf("Name\t\tCNIC\n");
    for (int i = 0; i < _voters->numberOfVoters; i++)
    {
        printf("%s\t%s\n", _voters->voterNames[i], _voters->voterCNICs[i]);
    }
}

//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
struct votes
{
    char **voterNames;
    char **voterCNICs;
    char **candidateNames;
    char **candidatePollSymbols;
    int numberOfVotes;
};
void allocateVotes(struct votes *_votes, int totalVotes)
{
    _votes->voterNames = (char **)malloc(totalVotes * sizeof(char *));
    _votes->voterCNICs = (char **)malloc(totalVotes * sizeof(char *));
    _votes->candidateNames = (char **)malloc(totalVotes * sizeof(char *));
    _votes->candidatePollSymbols = (char **)malloc(totalVotes * sizeof(char *));
    _votes->numberOfVotes = 0;
}
void addVote(struct votes *_votes, char *Name, char *CNIC, char *CandidateName, char *Symbol)
{
    _votes->voterNames[_votes->numberOfVotes] = (char *)malloc((strlen(Name) + 1) * sizeof(char));
    strcpy(_votes->voterNames[_votes->numberOfVotes], Name);
    _votes->voterCNICs[_votes->numberOfVotes] = (char *)malloc((strlen(CNIC) + 1) * sizeof(char));
    strcpy(_votes->voterCNICs[_votes->numberOfVotes], CNIC);
    _votes->candidateNames[_votes->numberOfVotes] = (char *)malloc((strlen(Symbol) + 1) * sizeof(char));
    strcpy(_votes->candidateNames[_votes->numberOfVotes], CandidateName);
    _votes->candidatePollSymbols[_votes->numberOfVotes] = (char *)malloc((strlen(Symbol) + 1) * sizeof(char));
    strcpy(_votes->candidatePollSymbols[_votes->numberOfVotes], Symbol);
    _votes->numberOfVotes++;
}
void saveVotes(struct votes *_votes, char *filename)
{
    FILE *file;
    file = fopen(filename, "w");
    if (file != NULL)
    {
        printf("File opened successfully.\n");
        for (int i = 0; i < _votes->numberOfVotes; i++)
        {
            fprintf(file, "%s\t%s\t%s\t%s", _votes->voterNames[i], _votes->voterCNICs[i], _votes->candidateNames[i], _votes->candidatePollSymbols[i]);
            if (i != _votes->numberOfVotes - 1)
                fprintf(file, "\n");
        }
        fclose(file);
    }
    else
    {
        printf("Unable to open file.\n");
    }
}
int verifyVote(struct votes *_votes, char *Name, char *CNIC)
{
    for (int i = 0; i < _votes->numberOfVotes; i++)
    {
        if (!strcmp(_votes->voterNames[i], Name) && !strcmp(_votes->voterCNICs[i], CNIC))
        {
            return 0;
        }
    }
    return 1;
}
void printVotes(struct votes *_votes)
{
    printf("Name\t\tCNIC\t\tPollSymbol\n");
    for (int i = 0; i < _votes->numberOfVotes; i++)
    {
        printf("%s\t%s\t%s\n", _votes->voterNames[i], _votes->voterCNICs[i], _votes->candidatePollSymbols[i]);
    }
}
//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
void readStringFromFile(char *str, char *filename)
{
    FILE *file;
    file = fopen(filename, "r");
    if (file != NULL)
    {
        char character;
        int index = 0;
        //printf("File opened successfully.\n");
        character = fgetc(file);
        while (character != EOF)
        {
            str[index++] = character;
            character = fgetc(file);
        }
        fclose(file);

        str[index] = 0;
    }

    else
    {
        printf("Unable to open file.\n");
    }
}
void writeStringToFile(char *str, char *filename)
{
    FILE *file;
    file = fopen(filename, "w");
    if (file != NULL)
    {
        //printf("File opened successfully.\n");
        fprintf(file, "%s", str);
        fclose(file);
    }
    else
    {
        printf("Unable to open file.\n");
    }
}

//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
void *newThread(void *param)
{
    int client_sock = *((int *)param);
    sem_wait(&mutex);
    numberOfClients++;
    sem_post(&mutex);
    char server_message[2000], client_message[2000]; // Sending values from the server and receive from the server we need this
    char VoterName[50];
    char VoterCNIC[50];
    char CandidateSymbol[50];
    char CandidateName[50];
    memset(VoterName, '\0', sizeof(VoterName));
    memset(VoterCNIC, '\0', sizeof(VoterCNIC));
    memset(CandidateSymbol, '\0', sizeof(CandidateSymbol));
    memset(CandidateName, '\0', sizeof(CandidateName));
    SendMessageToClient(client_sock, "Enter your Name:");
    //Receive the message from the client
    if (recv(client_sock, VoterName, sizeof(VoterName), 0) < 0)
    {
        printf("Receive Failed. Error!!!!!\n");
        return -1;
    }
    SendMessageToClient(client_sock, "Enter your CNIC:");
    //Receive the message from the client
    if (recv(client_sock, VoterCNIC, sizeof(VoterCNIC), 0) < 0)
    {
        printf("Receive Failed. Error!!!!!\n");
        return -1;
    }
    if (verifyVoter(&_registeredVoters, VoterName, VoterCNIC))
    {
        if (verifyVote(&_votes, VoterName, VoterCNIC))
        {
            //welcomes the voter,ask to enter symbol and display the names and poll symbols of the candidates
            memset(server_message, '\0', sizeof(server_message));
            strcpy(server_message,
                   "Welcome Voter!!\nPlease Enter the Poll Symbol:\n:::::::::::::::Candidates List::::::::::::::::\n");
            char msg[2000];
            readStringFromFile(msg, "Candidates_List.txt");
            strcat(server_message, msg);
            int isSymbolVerified = 0;
            //prompt and verify poll symbol
            do
            {
                SendMessageToClient(client_sock, server_message);
                //Receive the message from the client
                if (recv(client_sock, CandidateSymbol, sizeof(CandidateSymbol), 0) < 0)
                {
                    printf("Receive Failed. Error!!!!!\n");
                    return -1;
                }
                isSymbolVerified = verifyCandidateSymbol(&_candidates, CandidateSymbol);
                if (!isSymbolVerified)
                {
                    memset(server_message, '\0', sizeof(server_message));
                    strcpy(server_message, "Entered Poll Symbol is not in the List!!");
                }
            } while (!isSymbolVerified);
            SendMessageToClient(client_sock, "Thank You!!");
            strcpy(CandidateName, getCandidateName(&_candidates, CandidateSymbol));
            addVote(&_votes, VoterName, VoterCNIC, CandidateName, CandidateSymbol);
        }
        else
        {
            SendMessageToClient(client_sock, "You have already casted your Vote!!");
        }
    }
    else
    {
        SendMessageToClient(client_sock, "Sorry, You're not Registred!!");
    }

    sem_wait(&mutex);
    numberOfClients--;
    sem_post(&mutex);
    //Closing the Socket
    close(client_sock);
    saveVotes(&_votes, "output.txt");
    pthread_exit(NULL);
}
void SendMessageToClient(int client_sock, char *server_message)
{
    if (send(client_sock, server_message, strlen(server_message), 0) < 0)
    {
        printf("Send Failed. Error!!!!!\n");
        return -1;
    }
}
//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************
int main(void)
{
    allocateVoters(&_registeredVoters, 10);
    loadVoters(&_registeredVoters);

    allocateCandidates(&_candidates, 3);
    loadCandidates(&_candidates);

    allocateVotes(&_votes, 10);

    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr; //SERVER ADDR will have all the server address
    int isServevrunning = 1;
    char server_message[20];
    int isClientAllowed = 0;
    //Creating Socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Could Not Create Socket. Error!!!!!\n");
        return -1;
    }

    printf("Socket Created\n");

    //Binding IP and Port to socket

    server_addr.sin_family = AF_INET;                     /* Address family = Internet */
    server_addr.sin_port = htons(2000);                   // Set port number, using htons function to use proper byte order */
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); /* Set IP address to localhost */

    // BINDING FUNCTION

    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) // Bind the address struct to the socket.  /
                                                                                     //bind() passes file descriptor, the address structure,and the length of the address structure
    {
        printf("Bind Failed. Error!!!!!\n");
        return -1;
    }

    printf("Bind Done\n");

    sem_init(&mutex, 1, 1); //1 means semaphore is used for process synchronization
    while (isServevrunning)
    {
        //Put the socket into Listening State
        if (listen(socket_desc, 1) < 0) //This listen() call tells the socket to listen to the incoming connections.
                                        // The list en() function places all incoming connection into a "backlog queue" until accept() call accepts the connection.
        {
            printf("Listening Failed. Error!!!!!\n");
            return -1;
        }

        printf("Listening for Incoming Connections.....\n");

        //Accept the incoming Connections

        client_size = sizeof(client_addr);

        client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size); // heree particular client k liye new socket create kr rhaa ha

        if (client_sock < 0)
        {
            printf("Accept Failed. Error!!!!!!\n");
            return -1;
        }
        printf("Client Connected with IP: %s and Port No: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        //inet_ntoa() function converts the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation

        //we have added semaphores for mutual exclusion of value of numbeOfClients
        sem_wait(&mutex);
        isClientAllowed = (numberOfClients < numberOfClientsAllowed);
        sem_post(&mutex);

        if (isClientAllowed)
        {
            pthread_t thread;
            if (pthread_create(&thread, NULL, &newThread, &client_sock) == -1)
            {
                printf("Thread Creation Failed!!!\n");
                return 0;
            }
        }
        else
        {
            SendMessageToClient(client_sock, "Server Full!!");
        }

        //pthread_detach(thread);
    }
    close(socket_desc);
    sem_destroy(&mutex);
    return 0;
}
//doneServerQ2Lab5