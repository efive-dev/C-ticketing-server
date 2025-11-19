#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024
#define SERVER_IP "127.0.0.1"

// Funzione per inviare un comando al server e ricevere la risposta
void invia_comando(int client_socket, const char *comando, char *risposta, size_t risposta_size) {
    // Viene inviato il comando al server
    send(client_socket, comando, strlen(comando), 0);
    
    // Riceviamo la risposta dal server
    memset(risposta, 0, risposta_size);
    int bytes_received = recv(client_socket, risposta, risposta_size - 1, 0);
    
    if (bytes_received < 0) {
        perror("Errore nella ricezione della risposta");
        strcpy(risposta, "Errore di comunicazione con il server");
    } else {
        risposta[bytes_received] = '\0';
    }
}

// Funzione per visualizzare il menu principale
void mostra_menu() {
    printf("\n===== SISTEMA DI TICKETING - MENU PRINCIPALE =====\n");
    printf("1. Login\n");
    printf("2. Crea un nuovo ticket\n");
    printf("3. Visualizza i tuoi ticket\n");
    printf("4. Cerca un ticket per ID\n");
    printf("5. Funzioni agente (richiede login come agente)\n");
    printf("0. Esci\n");
    printf("Scelta: ");
}

// Funzione per visualizzare il menu dell'agente
void mostra_menu_agente() {
    printf("\n===== MENU AGENTE =====\n");
    printf("1. Visualizza tutti i ticket\n");
    printf("2. Assegna un ticket a un agente\n");
    printf("3. Modifica lo stato di un ticket\n");
    printf("4. Modifica la priorità di un ticket\n");
    printf("5. Elimina un ticket\n");
    printf("6. Torna al menu principale\n");
    printf("Scelta: ");
}

int main() {
    int client_socket = 0;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER] = {0};
    char comando[MAX_BUFFER] = {0};
    int scelta = -1;
    int logged_in = 0;
    
    // Creazione del socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nErrore nella creazione del socket\n");
        return -1;
    }
    
    // Configurazione dell'indirizzo del server
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    
    // Conversione dell'indirizzo IP da stringa a formato binario
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        printf("\nIndirizzo non valido / non supportato\n");
        return -1;
    }
    
    // Connessione al server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("\nConnessione fallita\n");
        return -1;
    }
    
    printf("Connesso al server %s:%d\n", SERVER_IP, PORT);
    
    // Ricevi il messaggio di benvenuto dal server
    memset(buffer, 0, MAX_BUFFER);
    int bytes_received = recv(client_socket, buffer, MAX_BUFFER - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    } 
    
    // Loop principale del client
    while (1) {
        mostra_menu();
        // pulisce il buffer nel caso venga inserito un numero (o una stringa) non valida
        if (scanf("%d", &scelta) != 1) {
            // Pulisci il buffer di input in caso di input non valido
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Input non valido. Riprova.\n");
            continue;
        }
        
        // Pulisci il buffer di input (scanf non consuma \n e questo lo toglie)
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        switch (scelta) {
            case 0: // Esci
                printf("Disconnessione dal server...\n");
                close(client_socket);
                return 0;
                
            case 1: { // Login
                char username[50];
                char password[50];

                // Leggo username e password da stdin (riga di comando)
                printf("Username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0; // Rimuovo il newline
                
                printf("Password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0; // Rimuovo il newline
                
                // Invio al server la stringa di comango "login username password"
                sprintf(comando, "login %s %s", username, password);
                invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                printf("\n%s\n", buffer);
                
                // Verifica se il login è riuscito
                // strstr() cerca in buffer la sottostringa "Login effettuato"
                // Se la trova, significa che il login è andato a buon fine
                if (strstr(buffer, "Login effettuato") != NULL) {
                    logged_in = 1;
                }
                break;
            }
                
            case 2: { // Crea un nuovo ticket
                char titolo[100];
                char descrizione[500];
                
                printf("Titolo del ticket: ");
                fgets(titolo, sizeof(titolo), stdin);
                titolo[strcspn(titolo, "\n")] = 0; // Rimuovo il newline
                
                printf("Descrizione del problema:\n");
                fgets(descrizione, sizeof(descrizione), stdin);
                descrizione[strcspn(descrizione, "\n")] = 0; // Rimuovo il newline
                
                sprintf(comando, "crea %s\n%s", titolo, descrizione);
                invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                printf("\n%s\n", buffer);
                break;
            }
                
            case 3: { // Visualizza i tuoi ticket
                strcpy(comando, "lista");
                invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                printf("\n%s\n", buffer);
                break;
            }
                
            case 4: { // Cerca un ticket per ID
                int id;
                printf("Inserisci l'ID del ticket: ");
                scanf("%d", &id);
                while ((c = getchar()) != '\n' && c != EOF); // Pulisci il buffer di input
                
                sprintf(comando, "cerca %d", id);
                invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                printf("\n%s\n", buffer);
                break;
            }
                
            case 5: { // Funzioni agente
                if (!logged_in) {
                    printf("\nDevi effettuare il login per accedere alle funzioni agente\n");
                    break;
                }
                
                int scelta_agente = -1;
                do {
                    mostra_menu_agente();
                    if (scanf("%d", &scelta_agente) != 1) {
                        // Pulisci il buffer di input in caso di input non valido
                        while ((c = getchar()) != '\n' && c != EOF);
                        printf("Input non valido. Riprova.\n");
                        continue;
                    }
                    
                    // Pulisci il buffer di input
                    while ((c = getchar()) != '\n' && c != EOF);
                    
                    switch (scelta_agente) {
                        case 1: { // Visualizza tutti i ticket
                            strcpy(comando, "listatutti");
                            invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                            printf("\n%s\n", buffer);
                            break;
                        }
                            
                        case 2: { // Assegna un ticket a un agente
                            int id;
                            char agente[50];
                            
                            printf("Inserisci l'ID del ticket: ");
                            scanf("%d", &id);
                            while ((c = getchar()) != '\n' && c != EOF); // Pulisci il buffer di input
                            
                            printf("Inserisci il nome dell'agente: ");
                            fgets(agente, sizeof(agente), stdin);
                            agente[strcspn(agente, "\n")] = 0; // Rimuovo il newline
                            
                            sprintf(comando, "assegna %d %s", id, agente);
                            invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                            printf("\n%s\n", buffer);
                            break;
                        }
                            
                        case 3: { // Modifica lo stato di un ticket
                            int id, stato;
                            
                            printf("Inserisci l'ID del ticket: ");
                            scanf("%d", &id);
                            while ((c = getchar()) != '\n' && c != EOF); // Pulisci il buffer di input
                            
                            printf("Inserisci il nuovo stato (0=Aperto, 1=In Corso, 2=Chiuso): ");
                            scanf("%d", &stato);
                            while ((c = getchar()) != '\n' && c != EOF); // Pulisci il buffer di input
                            
                            sprintf(comando, "stato %d %d", id, stato);
                            invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                            printf("\n%s\n", buffer);
                            break;
                        }
                            
                        case 4: { // Modifica la priorità di un ticket
                            int id, priorita;
                            
                            printf("Inserisci l'ID del ticket: ");
                            scanf("%d", &id);
                            while ((c = getchar()) != '\n' && c != EOF); // Pulisci il buffer di input
                            
                            printf("Inserisci la nuova priorità (0=Bassa, 1=Media, 2=Alta, 3=Critica): ");
                            scanf("%d", &priorita);
                            while ((c = getchar()) != '\n' && c != EOF); // Pulisci il buffer di input
                            
                            sprintf(comando, "priorita %d %d", id, priorita);
                            invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                            printf("\n%s\n", buffer);
                            break;
                        }
                            
                        case 5: { // Elimina un ticket
                            int id;
                            printf("Inserisci l'ID del ticket da eliminare: ");
                            scanf("%d", &id);
                            while ((c = getchar()) != '\n' && c != EOF); // Pulisci il buffer di input
                            
                            sprintf(comando, "elimina %d", id);
                            invia_comando(client_socket, comando, buffer, MAX_BUFFER);
                            printf("\n%s\n", buffer);
                            break;
                        }
                            
                        case 6: // Torna al menu principale
                            break;
                            
                        default:
                            printf("Scelta non valida\n");
                    }
                } while (scelta_agente != 6);
                break;
            }
                
            default:
                printf("Scelta non valida\n");
        }
    }

    close(client_socket);
    return 0;
}
