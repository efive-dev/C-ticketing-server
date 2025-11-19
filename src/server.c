#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_TICKETS 100
#define MAX_BUFFER 1024
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define TICKETS_FILE "tickets.txt" // File per la persistenza dei ticket

// Definizione degli stati del ticket
typedef enum {
    APERTO,
    IN_CORSO,
    CHIUSO
} StatoTicket;

// Definizione delle priorità del ticket
typedef enum {
    BASSA,
    MEDIA,
    ALTA,
    CRITICA,
    SENZA_PRIORITA
} PrioritaTicket;

// Struttura per rappresentare un ticket
typedef struct {
    int id;
    char titolo[100];
    char descrizione[500];
    char data_creazione[20];
    PrioritaTicket priorita;
    StatoTicket stato;
    char agente_assegnato[MAX_USERNAME_LENGTH];
    char cliente[MAX_USERNAME_LENGTH];
} Ticket;

// Struttura per rappresentare un utente
typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int is_agent; // 0 per cliente standard, 1 per agente di supporto
} Utente;

// Array globale di ticket
Ticket tickets[MAX_TICKETS];
int num_tickets = 0;

// Array di utenti (username e password)
Utente utenti[] = {
    {"cliente1", "password1", 0},
    {"cliente2", "password2", 0},
    {"agente1", "password1", 1},
    {"agente2", "password2", 1}
};
int num_utenti = 4;

/*
    FUNZIONI PER MANTENERE UN SALVATAGGIO PERSISTENTE DEI TICKET SU UN FILE .txt
*/

// Funzione per salvare i tickets su file
void salva_tickets() {
    FILE *file = fopen(TICKETS_FILE, "w");
    if (file == NULL) {
        perror("Errore nell'apertura del file per il salvataggio");
        return;
    }
    
    for (int i = 0; i < num_tickets; i++) {
        fprintf(file, "%d|%s|%s|%s|%d|%d|%s|%s\n",
                tickets[i].id,
                tickets[i].titolo,
                tickets[i].descrizione,
                tickets[i].data_creazione,
                tickets[i].priorita,
                tickets[i].stato,
                tickets[i].agente_assegnato,
                tickets[i].cliente);
    }
    
    fclose(file);
    printf("Tickets salvati su file\n");
}

// Funzione per caricare i ticket da file
void carica_tickets() {
    FILE *file = fopen(TICKETS_FILE, "r");
    
    char line[MAX_BUFFER];
    num_tickets = 0;
    
    if (file == NULL) {
        perror("Errore nell'apertura del file per il caricamento potrebbe non esistere");
        return;
    }

    // I tickets sono salvati su file in questo formato:
    // ID|Titolo|Descrizione|Data Creazione|Priorità|Stato|Agente Assegnato|Cliente
    // | è il separatore
    while (fgets(line, sizeof(line), file) && num_tickets < MAX_TICKETS) {
        Ticket t;
        char *token;
        
        // ID
        token = strtok(line, "|");
        if (token == NULL) continue;
        t.id = atoi(token);
        
        // Titolo
        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(t.titolo, token, sizeof(t.titolo) - 1);
        
        // Descrizione
        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(t.descrizione, token, sizeof(t.descrizione) - 1);
        
        // Data creazione
        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(t.data_creazione, token, sizeof(t.data_creazione) - 1);
        
        // Priorità
        token = strtok(NULL, "|");
        if (token == NULL) continue;
        t.priorita = (PrioritaTicket)atoi(token);
        
        // Stato
        token = strtok(NULL, "|");
        if (token == NULL) continue;
        t.stato = (StatoTicket)atoi(token);
        
        // Agente assegnato
        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(t.agente_assegnato, token, sizeof(t.agente_assegnato) - 1);
        
        // Cliente
        token = strtok(NULL, "\n");
        if (token == NULL) continue;
        strncpy(t.cliente, token, sizeof(t.cliente) - 1);
        
        tickets[num_tickets++] = t;
    }
    
    fclose(file);
    printf("Caricati %d tickets da file\n", num_tickets);
}

/*
    FUNZIONI PER GESTIRE LE RICHIESTE
*/

// Funzione per ottenere la data corrente formattata
void data_corrente_format(char *date_str) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(date_str, 20, "%d/%m/%Y %H:%M", t); // metodo specifico per creare una stringa di una data
}

// Funzione per autenticare un utente
int autenticazione(char *username, char *password, int *is_agent) {
    for (int i = 0; i < num_utenti; i++) {
        if (strcmp(utenti[i].username, username) == 0 && 
            strcmp(utenti[i].password, password) == 0) {
            *is_agent = utenti[i].is_agent;
            return 1; // Autenticazione riuscita
        }
    }
    return 0; // Autenticazione fallita
}

// Funzione per creare un nuovo ticket
int crea_ticket(char *titolo, char *descrizione, char *cliente, PrioritaTicket priorita) {
    carica_tickets();
    if (num_tickets >= MAX_TICKETS) {
        return -1; // Limite di ticket raggiunto
    }
    priorita = SENZA_PRIORITA;
    Ticket nuovo_ticket;
    nuovo_ticket.id = num_tickets > 0 ? tickets[num_tickets - 1].id + 1 : 1;
    strncpy(nuovo_ticket.titolo, titolo, sizeof(nuovo_ticket.titolo) - 1);
    strncpy(nuovo_ticket.descrizione, descrizione, sizeof(nuovo_ticket.descrizione) - 1);
    data_corrente_format(nuovo_ticket.data_creazione);
    nuovo_ticket.priorita = priorita; // Priorità iniziale
    nuovo_ticket.stato = APERTO;
    strcpy(nuovo_ticket.agente_assegnato, "Non assegnato");
    strncpy(nuovo_ticket.cliente, cliente, sizeof(nuovo_ticket.cliente) - 1);
    tickets[num_tickets] = nuovo_ticket;
    num_tickets++;
    
    // Salvo i ticket su file dopo ogni creazione
    salva_tickets();
    
    return nuovo_ticket.id;
}

// Funzione per cercare ticket per ID
Ticket* trova_ticket_per_id(int id) {
    carica_tickets();
    for (int i = 0; i < num_tickets; i++) {
        if (tickets[i].id == id) {
            return &tickets[i];
        }
    }
    return NULL;
}

// Funzione per cercare ticket per cliente (ossia permette di visualizzare solo i ticket di un cliente)
void trova_ticket_per_cliente(char *cliente, Ticket *risultati[], int *num_risultati) {
    carica_tickets();
    *num_risultati = 0;
    for (int i = 0; i < num_tickets; i++) {
        if (strcmp(tickets[i].cliente, cliente) == 0) {
            risultati[*num_risultati] = &tickets[i];
            (*num_risultati)++;
        }
    }
}

// Funzione per assegnare un ticket a un agente
int assegna_ticket(int id, char *agente) {
    carica_tickets();
    Ticket *ticket = trova_ticket_per_id(id);
    if (ticket == NULL) {
        return 0; // Ticket non trovato
    }
    
    // Se il ticket è aperto, lo si assegna all'agente e diventa in corso
    // Se il ticket è già in corso o chiuso, non può essere assegnato
    strncpy(ticket->agente_assegnato, agente, sizeof(ticket->agente_assegnato) - 1);
    if (ticket->stato == APERTO) {
        ticket->stato = IN_CORSO;
    }
    
    // Salva i ticket su file dopo ogni modifica
    salva_tickets();
    
    return 1;
}

// Funzione per modificare lo stato di un ticket
int modifica_stato_ticket(int id, StatoTicket nuovo_stato) {
    Ticket *ticket = trova_ticket_per_id(id);
    if (ticket == NULL) {
        return 0; // Ticket non trovato
    }
    
    ticket->stato = nuovo_stato;
    
    // Salva i ticket su file dopo ogni modifica
    salva_tickets();
    
    return 1;
}

// Funzione per modificare la priorità di un ticket
int modifica_priorita_ticket(int id, PrioritaTicket nuova_priorita) {
    Ticket *ticket = trova_ticket_per_id(id);
    if (ticket == NULL) {
        return 0; // Ticket non trovato
    }
    
    ticket->priorita = nuova_priorita;
    
    // Salva i ticket su file dopo ogni modifica
    salva_tickets();
    
    return 1;
}

// Funzione per convertire lo stato del ticket in stringa
const char* stato_to_string(StatoTicket stato) {
    switch (stato) {
        case APERTO: return "Aperto";
        case IN_CORSO: return "In Corso";
        case CHIUSO: return "Chiuso";
        default: return "Sconosciuto"; // Teoricamente non dovrebbe mai succedere
    }
}

// Funzione per eliminare un ticket
int elimina_ticket(int id) {
    carica_tickets();
    int found = 0;
    for (int i = 0; i < num_tickets; i++) {
        if (tickets[i].id == id) {
            // Sposta tutti i ticket successivi indietro di una posizione
            for (int j = i; j < num_tickets - 1; j++) {
                tickets[j] = tickets[j + 1];
            }
            num_tickets--;
            found = 1;
            break;
        }
    }
    if (found) {
        salva_tickets();
        return 1;
    }
    return 0;
}

// Funzione per convertire la priorità del ticket in stringa
const char* priorita_to_string(PrioritaTicket priorita) {
    switch (priorita) {
        case BASSA: return "Bassa";
        case MEDIA: return "Media";
        case ALTA: return "Alta";
        case CRITICA: return "Critica";
        case SENZA_PRIORITA: return "SENZA PRIORITA";
        default: return "Sconosciuta"; // Teoricamente non dovrebbe mai succedere
    }
}

// Funzione per formattare un ticket come stringa
void ticket_to_string(Ticket *ticket, char *buffer, size_t size) {
    snprintf(buffer, size,
             "ID: %d\nTitolo: %s\nDescrizione: %s\nData: %s\nPriorità: %s\nStato: %s\nAgente: %s\nCliente: %s\n",
             ticket->id, ticket->titolo, ticket->descrizione, ticket->data_creazione,
             priorita_to_string(ticket->priorita), stato_to_string(ticket->stato),
             ticket->agente_assegnato, ticket->cliente);
}

// Funzione per gestire le richieste dei client
// Rappresenta il cuore del server, qui si gestiscono tutte le richieste del client
// e si inviano le risposte appropriate
// Ogni client ha un proprio processo figlio (del server) che gestisce le richieste
void gestisci_client(int client_socket) {
    char buffer[MAX_BUFFER];
    int logged_in = 0;
    int is_agent = 0;
    char username[MAX_USERNAME_LENGTH] = "Guest"; // Nome utente di default
    
    // Invia messaggio di benvenuto
    strcpy(buffer, "Benvenuto al sistema di ticketing.");
    send(client_socket, buffer, strlen(buffer), 0);
    
    while (1) {
        memset(buffer, 0, MAX_BUFFER); // Pulisce il buffer

        // Riceve il comando dal client
        // MAX_BUFFER - 1 per non contare il terminatore "\0" altrimenti nel buffer si trova sempre almeno un elemento
        int bytes_received = recv(client_socket, buffer, MAX_BUFFER - 1, 0); 
        
        if (bytes_received <= 0) {
            printf("Client disconnesso o errore di ricezione\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Ricevuto: %s\n", buffer);
        
        // Tokenizza il comando
        // ogni richiesta del client è composta da un comando e da eventuali argomenti
        // Esempio: "login <username> <password>"
        // In pratica srtok() divide la stringa in base agli spazi e ai caratteri di nuova linea
        char *comando = strtok(buffer, " \n");
        if (comando == NULL) continue;
        
        char risposta[MAX_BUFFER] = "";
        
        // Comando di login
        if (strcmp(comando, "login") == 0) {
            // Si estrae il nome utente e la password dalla stringa (per questo si inserisce NULL)
            char *user = strtok(NULL, " \n");
            char *pass = strtok(NULL, " \n");
            
            if (user == NULL || pass == NULL) {
                strcpy(risposta, "Errore: Formato corretto 'login <username> <password>'");
            } else {
                if (autenticazione(user, pass, &is_agent)) {
                    logged_in = 1;
                    strncpy(username, user, MAX_USERNAME_LENGTH - 1);
                    sprintf(risposta, "Login effettuato come %s. Ruolo: %s", 
                            username, is_agent ? "Agente di supporto" : "Cliente autenticato");
                } else {
                    strcpy(risposta, "Errore: Credenziali non valide");
                }
            }
        }

        // Comando per creare un ticket
        else if (strcmp(comando, "crea") == 0) { // Creazione ticket (non richiede login, priorità iniziale: SENZA PRIORITA)
            char *titolo = strtok(NULL, "\n");
            char *descrizione = strtok(NULL, "\n");
            if (titolo == NULL) {
                strcpy(risposta, "Errore: Formato corretto 'crea <titolo>\\n<descrizione>'");
            } else {
                // Se la descrizione non è fornita, si usa il titolo come descrizione
                // Non dovrebbe mai succedere, ma è una protezione in più
                if (descrizione == NULL) {
                    descrizione = titolo;
                }

                int id = crea_ticket(titolo, descrizione, username, SENZA_PRIORITA);
                if (id > 0) {
                    sprintf(risposta, "Ticket creato con successo. ID: %d", id);
                } else {
                    strcpy(risposta, "Errore: Impossibile creare il ticket");
                }
            }
        }

        // Comando per visualizzare i propri ticket (non richiede login)
        else if (strcmp(comando, "lista") == 0) {
            Ticket *risultati[MAX_TICKETS];
            int num_risultati = 0;
            trova_ticket_per_cliente(username, risultati, &num_risultati);
            if (num_risultati == 0) {
                strcpy(risposta, "Nessun ticket trovato");
            } else {
                strcpy(risposta, "Ticket trovati:\n");
                for (int i = 0; i < num_risultati; i++) {
                    char ticket_str[MAX_BUFFER];
                    ticket_to_string(risultati[i], ticket_str, sizeof(ticket_str));
                    strcat(risposta, "\n");
                    strcat(risposta, ticket_str);
                    strcat(risposta, "----------------------------\n");
                }
            }
        }

        // Comando per cercare un ticket per ID (non richiede login)
        else if (strcmp(comando, "cerca") == 0) {
            char *id_str = strtok(NULL, " \n");
            if (id_str == NULL) {
                strcpy(risposta, "Errore: Formato corretto 'cerca <id>'");
            } else {
                int id = atoi(id_str); // atoi converte la stringa in un intero
                Ticket *ticket = trova_ticket_per_id(id);
                if (ticket == NULL) {
                    sprintf(risposta, "Errore: Nessun ticket trovato con ID %d", id);
                } else {
                    ticket_to_string(ticket, risposta, sizeof(risposta));
                }
            }
        }

        // Comando per assegnare un ticket a un agente (solo per agenti)
        else if (strcmp(comando, "assegna") == 0) {
            if (!logged_in || !is_agent) {
                strcpy(risposta, "Errore: Devi essere un agente di supporto per assegnare un ticket");
            } else {
                char *id_str = strtok(NULL, " \n");
                char *agente = strtok(NULL, " \n");
                
                if (id_str == NULL || agente == NULL) {
                    strcpy(risposta, "Errore: Formato corretto 'assegna <id> <agente>'");
                } else {
                    int id = atoi(id_str);
                    if (assegna_ticket(id, agente)) {
                        sprintf(risposta, "Ticket %d assegnato a %s con successo", id, agente);
                    } else {
                        sprintf(risposta, "Errore: Impossibile assegnare il ticket %d", id);
                    }
                }
            }
        }

        // Comando per modificare lo stato di un ticket (solo per agenti)
        else if (strcmp(comando, "stato") == 0) {
            if (!logged_in || !is_agent) {
                strcpy(risposta, "Errore: Devi essere un agente di supporto per modificare lo stato di un ticket");
            } else {
                char *id_str = strtok(NULL, " \n");
                char *stato_str = strtok(NULL, " \n");
                
                if (id_str == NULL || stato_str == NULL) {
                    strcpy(risposta, "Errore: Formato corretto 'stato <id> <nuovo_stato>'");
                } else {
                    int id = atoi(id_str);
                    int nuovo_stato = atoi(stato_str);
                    
                    if (nuovo_stato < APERTO || nuovo_stato > CHIUSO) {
                        strcpy(risposta, "Errore: Stato non valido (0=Aperto, 1=In Corso, 2=Chiuso)");
                    } else if (modifica_stato_ticket(id, nuovo_stato)) {
                        sprintf(risposta, "Stato del ticket %d modificato in %s con successo", 
                                id, stato_to_string(nuovo_stato));
                    } else {
                        sprintf(risposta, "Errore: Impossibile modificare lo stato del ticket %d", id);
                    }
                }
            }
        }

        // Comando per modificare la priorità di un ticket (solo per agenti)
        else if (strcmp(comando, "priorita") == 0) {
            if (!logged_in || !is_agent) {
                strcpy(risposta, "Errore: Devi essere un agente di supporto per modificare la priorità di un ticket");
            } else {
                char *id_str = strtok(NULL, " \n");
                char *priorita_str = strtok(NULL, " \n");
                
                if (id_str == NULL || priorita_str == NULL) {
                    strcpy(risposta, "Errore: Formato corretto 'priorita <id> <nuova_priorita>'");
                } else {
                    int id = atoi(id_str);
                    int nuova_priorita = atoi(priorita_str);
                    
                    if (nuova_priorita < BASSA || nuova_priorita > SENZA_PRIORITA) {
                        strcpy(risposta, "Errore: Priorità non valida (0=Bassa, 1=Media, 2=Alta, 3=Critica, 4=SENZA PRIORITA)");
                    } else if (modifica_priorita_ticket(id, nuova_priorita)) {
                        sprintf(risposta, "Priorità del ticket %d modificata in %s con successo", 
                                id, priorita_to_string(nuova_priorita));
                    } else {
                        sprintf(risposta, "Errore: Impossibile modificare la priorità del ticket %d", id);
                    }
                }
            }
        }

        // Comando per eliminare un ticket (solo per agenti)
        else if (strcmp(comando, "elimina") == 0) {
            if (!logged_in || !is_agent) {
                strcpy(risposta, "Errore: Devi essere un agente di supporto per eliminare un ticket");
            } else {
                char *id_str = strtok(NULL, " \n");
                if (id_str == NULL) {
                    strcpy(risposta, "Errore: Formato corretto 'elimina <id>'");
                } else {
                    int id = atoi(id_str);
                    if (elimina_ticket(id)) {
                        sprintf(risposta, "Ticket %d eliminato con successo", id);
                    } else {
                        sprintf(risposta, "Errore: Impossibile eliminare il ticket %d", id);
                    }
                }
            }
        }

        // Comando per visualizzare tutti i ticket (solo per agenti)
        else if (strcmp(comando, "listatutti") == 0) {
            if (!logged_in || !is_agent) {
                strcpy(risposta, "Errore: Devi essere un agente di supporto per visualizzare tutti i ticket");
            } else {
                if (num_tickets == 0) {
                    strcpy(risposta, "Nessun ticket trovato");
                } else {
                    strcpy(risposta, "Tutti i ticket:\n");
                    for (int i = 0; i < num_tickets; i++) {
                        char ticket_str[MAX_BUFFER];
                        ticket_to_string(&tickets[i], ticket_str, sizeof(ticket_str));
                        strcat(risposta, "\n");
                        strcat(risposta, ticket_str);
                        strcat(risposta, "----------------------------\n");
                    }
                }
            }
        }
        // Invia la risposta al client
        send(client_socket, risposta, strlen(risposta), 0);
    } 
    // Chiude il socket del client quando il ciclo termina
    printf("Chiusura della connessione con il client\n");
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    pid_t child_pid;
    
    // Carica i ticket dal file all'avvio del server
    carica_tickets();
    
    // Creazione del socket del server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE); 
    }
    
    // Configurazione dell'indirizzo del server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Binding del socket all'indirizzo specificato
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Errore nel binding del socket");
        exit(EXIT_FAILURE);
    }
    
    // Messa in ascolto del socket del server
    // MAX_CLIENTS è il numero massimo di connessioni in attesa
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Errore nella messa in ascolto del socket");
        exit(EXIT_FAILURE);
    }
    
    printf("Server in ascolto sulla porta %d...\n", PORT);
    printf("Per chiudere il server, premere [Ctrl+C]\n");
    
    // Accettazione delle connessioni in entrata
    while (1) {
        printf("In attesa di connessioni...\n");
        
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len)) < 0) {
            perror("Errore nell'accettazione della connessione");
            continue;
        }
        
        printf("Connessione accettata da %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
        // Crea un processo figlio per gestire il client
        child_pid = fork();
        
        if (child_pid < 0) {
            perror("Errore nella creazione del processo figlio");
            close(client_socket);
        } 
        else if (child_pid == 0) {
            // Processo figlio
            close(server_fd);  // Il figlio non ha bisogno del socket di ascolto
            gestisci_client(client_socket);
            exit(0);  // Termina il processo figlio dopo aver gestito il client
        } 
        else {
            // Processo padre
            close(client_socket);  // Il padre non ha bisogno del socket del client
        }
    }
    return 0;
}