# Ticketing System

## Description

This project implements a client-server ticketing system in C. The client allows users to create and manage tickets, while the server handles persistence and request processing.

## Project Structure

- `src/`: Contains source files  
  - `client.c`: Client implementation  
  - `server.c`: Server implementation  
- `Makefile`: Build file  
- `tickets.txt`: Ticket persistence file  
- `README.md`: Project documentation

## Requirements

- C compiler (gcc)  
- Make

## Compilation and Execution

1. Compile the project:
   ```
   make
   ```
2. Start the server:
   ```
   ./server
   ```
3. Start the client:
   ```
   ./client
   ```
4. Clean output files:
   ```
   make clean
   ```
5. Optional - Clear the `tickets.txt` file:
   ```
   make delete_all_tickets
   ```

## Execution

1. Start the server first:

```
./server
```

2. Start one or more clients in separate terminals:

```
./client
```

## Usage

### Standard Clients

Standard clients can:
- Log in
- Create new tickets
- View their own tickets
- Search tickets by ID

### Support Agents

Support agents can:
- Perform all actions of standard clients
- View all tickets in the system
- Assign tickets to agents
- Change the status of tickets
- Change the priority of tickets
- Delete a ticket

## Default Users

The system includes the following predefined users:

**Standard Clients**:
- Username: `cliente1`, Password: `password1`  
- Username: `cliente2`, Password: `password2`

**Support Agents**:
- Username: `agente1`, Password: `password1`  
- Username: `agente2`, Password: `password2`

## Client Interface

### User Commands
- `Login <username> <password>`  
- `Create a new ticket <title> <description>`  
- `View your tickets`  
- `Search ticket by ID <id>`

### Agent Commands
- `View your tickets`  
- `Assign a ticket to an agent <id> <agent>`  
- `Change the status of a ticket <id> <new_status>`  
- `Change the priority of a ticket <id> <new_priority>`  
- `Delete a ticket <id>`

# Sistema di Ticketing - italian

## Descrizione

Questo progetto implementa un sistema di ticketing client-server in C. Il client permette agli utenti di creare e gestire ticket, mentre il server gestisce la persistenza e l'elaborazione delle richieste.

## Struttura del progetto

- `src/`: Contiene i file sorgenti
  - `client.c`: Implementazione del client
  - `server.c`: Implementazione del server
- `Makefile`: File per la compilazione
- `tickets.txt`: File di persistenza dei ticket
- `README.md`: Documentazione del progetto

## Requisiti

- Compilatore C (gcc)
- Make

## Compilazione ed esecuzione

1. Compilare il progetto:
   ```
   make
   ```
2. Avviare il server:
   ```
   ./server
   ```
3. Avviare il client:
   ```
   ./client
   ```
4. Pulire i file di ouput
   ```
   make clean
   ```
5. Opzionale - pulire il file tickets.txt
   ```   
   make delete_all_tickets
   ```

## Esecuzione

1. Avviare prima il server:

```
./server
```

2. Avviare uno o più client in terminali separati:

```
./client
```

## Utilizzo

### Client Standard

I client standard possono:
- Effettuare login
- Creare nuovi ticket
- Visualizzare i propri ticket
- Cercare ticket per ID

### Agenti di Supporto

Gli agenti di supporto possono:
- Effettuare tutte le operazioni dei client standard
- Visualizzare tutti i ticket nel sistema
- Assegnare ticket ad agenti
- Modificare lo stato dei ticket
- Modificare la priorità dei ticket
- Eliminare un ticket

## Utenti Predefiniti

Il sistema include i seguenti utenti predefiniti:

**Client Standard**:
- Username: `cliente1`, Password: `password1`
- Username: `cliente2`, Password: `password2`

**Agenti di Supporto**:
- Username: `agente1`, Password: `password1`
- Username: `agente2`, Password: `password2`

## Interfaccia Client

### Comandi Utente
- `Login <username> <password>`
- `Crea un nuovo ticket <titolo> <descrizione>`
- `Visualizza i tuoi ticket`
- `Cerca un ticket per ID <id>`

### Comandi Agente
- `Visualizza i tuoi ticket`
- `Assegna un ticket a un agente <id> <agente>`
- `Modifica lo stato di un ticket <id> <nuovo_stato>`
- `Modifica la priorità di un ticket <id> <nuova_priorita>`
- `Elimina un ticket <id>`
