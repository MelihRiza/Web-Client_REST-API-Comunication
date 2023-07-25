#include <iostream>
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#define MAX_COOKIE_SIZE 350
#define MAX_JWT_SIZE 350
#define SIZE_BOOKS_RECEIVED 200000
#define SIZE_BOOK 2000
#define SERVER_RESPONSE_SIZE_NON_JSON 2000
#define SERVER_RESPONSE_TOKEN_JWT 2000


std :: string construireCerereHTTPInregistrare(int lungimeJson) {
    std :: string cerere = "POST /api/v1/tema/auth/register\r\n";
    cerere += "Host: 34.254.242.81:8080\r\n";
    cerere += "Content-Type: application/json\r\n";
    cerere += "Content-Length: " + std :: to_string(lungimeJson) + "\r\n\r\n";
    return cerere;
} 

std :: string construireCerereHTTPLogare(int lungimeJson) {
    std :: string cerere = "POST /api/v1/tema/auth/login\r\n";
    cerere += "Host: 34.254.242.81.8080\r\n";
    cerere += "Content-Type: application/json\r\n";
    cerere += "Content-Length: " + std :: to_string(lungimeJson) + "\r\n\r\n";
    return cerere;
}


std :: string construireCerereAcces(char* cookie) {
    std :: string cerere = "GET /api/v1/tema/library/access\r\n";
    cerere += "Host: 34.254.242.81:8080\r\n";
    cerere += "Cookie: ";
    cerere.append(cookie);
    cerere += "\r\n\r\n";
    return cerere;
}

std :: string construireCerereCarti(char* tokenJWT) {
    std :: string cerere = "GET /api/v1/tema/library/books\r\n";
    cerere += "Host: 34.254.242.81:8080\r\n";
    cerere += "Authorization: Bearer ";
    cerere.append(tokenJWT);
    cerere += "\r\n\r\n";
    return cerere;
}

std :: string construireCerereCarte(char* tokenJWT, int ID) {
    std :: string cerere = "GET /api/v1/tema/library/books/";
    std :: string IDstring = std :: to_string(ID);
    cerere += IDstring + "\r\n";
    cerere += "Host: 34.254.242.81:8080\r\n";
    cerere += "Authorization: Bearer ";
    cerere.append(tokenJWT);
    cerere += "\r\n\r\n";
    return cerere;
}


std :: string construireCerereAdaugareCarte(char* tokenJWT, int lungimeJson) {
    std :: string cerere = "POST /api/v1/tema/library/books\r\n";
    cerere += "Host: 34.254.242.81:8080\r\n";
    cerere += "Authorization: Bearer ";
    cerere.append(tokenJWT);
    cerere += "\r\n";
    cerere += "Content-Type: application/json\r\n";
    cerere += "Content-Length: " + std :: to_string(lungimeJson) + "\r\n\r\n";
    return cerere;
}

std :: string construireCerereStergereCarte(char* tokenJWT, int ID) {
    std :: string cerere = "DELETE /api/v1/tema/library/books/";
    std :: string IDstring = std :: to_string(ID);
    cerere += IDstring + "\r\n";
    cerere += "Host: 34.254.242.81:8080\r\n";
    cerere += "Authorization: Bearer ";
    cerere.append(tokenJWT);
    cerere += "\r\n\r\n";
    return cerere;
}

// REALIZEAZA CONECTAREA LA SERVER SI INTOARCE SOCKET-UL
int conectareLaServer() {
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServer < 0) {
        std :: cout << "Eroare socket!" << std :: endl;
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_aton("34.254.242.81", &serv_addr.sin_addr);
    connect(socketServer, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    return socketServer;
}

// STRUCTURA FOLOSITA PENTRU DESERIALIZAREA ELEMENTELOR JSON-ULUI 
// PRIMIT LA COMANDA "get_books"
struct Carti {
    int id;
    std :: string title;
};

// DESERIALIZARE
void from_json(const nlohmann :: json& j, Carti& c) {
        j.at("id").get_to(c.id);
        j.at("title").get_to(c.title);
}


// STRUCTURA FOLOSITA PENTRU DESERIALIZAREA JS0N-ULUI
// PRIMIT LA COMANDA "get_book" 
struct Carte {
    std :: string title;
    std :: string author;
    std :: string publisher;
    std :: string genre;
    int page_count;
};


// DESERIALIZARE
void from_json(const nlohmann :: json& j, Carte& C) {
        j.at("title").get_to(C.title);
        j.at("author").get_to(C.author);
        j.at("publisher").get_to(C.publisher);
        j.at("genre").get_to(C.genre);
        j.at("page_count").get_to(C.page_count);
}

// PRINTARE CARTI "get_books"
void afisareCarti(char* raspunsServer) {
    char* carti = (char*) malloc(sizeof(char) * SIZE_BOOKS_RECEIVED);
    char* pointerStart = strstr(raspunsServer, "[");
    char* pointerStop = strchr(pointerStart, ']');
    *(pointerStop + 1) = '\0';

    if (strlen(pointerStart) < 3) {
        std :: cout << "Nu exista carti in biblioteca!" << std :: endl;
    }

    strcpy(carti, pointerStart);

    int nrCarti = 0;
    for (size_t i = 0; i < strlen(carti); i++) {
        if (carti[i] == '}') {
            nrCarti++;
        }
    }

    nlohmann :: json Json;
    Json = nlohmann :: json :: parse(carti);
    struct Carti carte;

    for (int i = 0; i < nrCarti; i++) {
        carte = Json[i].get<Carti>();
        std :: cout << "Cartea nr " << i + 1 << ":" << std ::  endl; 
        std :: cout << "id = " << carte.id << std :: endl;
        std :: cout << "title = " << carte.title << std :: endl;
        std :: cout << std :: endl;
    }

    free(carti);
}

// PRINTARE CARTE "get_book"
void afisareCarte(char* raspunsServer) {
    char* cartePrimita = (char*) malloc(sizeof(char) * SIZE_BOOK);
    char* pointerStart = strstr(raspunsServer, "{");
    char* pointerStop = strchr(pointerStart, '}');
    *(pointerStop + 1) = '\0';

    strcpy(cartePrimita, "[");
    strcat(cartePrimita, pointerStart);
    strcat(cartePrimita, "]");

    nlohmann :: json Json;
    Json = nlohmann :: json :: parse(cartePrimita);

    struct Carte carte;

    carte = Json[0].get<Carte>();
    std :: cout << std :: endl;
    std :: cout << "Cartea este:" << std :: endl; 
    std :: cout << "title = " << carte.title << std :: endl;
    std :: cout << "author = " << carte.author << std :: endl;
    std :: cout << "publisher = " << carte.publisher << std :: endl;
    std :: cout << "genre = " << carte.genre << std :: endl;
    std :: cout << "page_count = " << carte.page_count << std :: endl;
    std :: cout << std :: endl;

    free(cartePrimita);
}

// FUNCTIE CE TRIMITE CEREREA CATRE SERVER PENTRU INTERSITRAREA UNUI CONT "register"
// INTOARCE CODUL DE RASPUNS AL SERVER-ULUI
std :: string cerereInregistrare(nlohmann :: json JSON) {
    int socketServer = conectareLaServer();

    std :: string jsonString = JSON.dump(4); 
    const char* pentruLungime = jsonString.c_str();
    int lungimeJSON = strlen(pentruLungime);

    std :: string header = construireCerereHTTPInregistrare(lungimeJSON);

    header.append(jsonString);
    const char* headerChar = header.c_str();

    send(socketServer, headerChar, strlen(headerChar), 0);
    char* raspunsServer = (char*) malloc(sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON);
    recv(socketServer, raspunsServer, sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON, 0);
    char* token = strtok(raspunsServer, " ");
    token = strtok(NULL, " ");

    std :: string codRaspuns(token);
    free(raspunsServer);
    return codRaspuns;
}

// FUNCTIE CE TRIMITE CEREREA CATRE SERVER PENTRU LOGAREA LA UN CONT "login"
// INTOARCE COOKIE-UL SESIUNII
char* cerereLogare(nlohmann :: json JSON) {
    int socketServer = conectareLaServer();

    std :: string jsonString = JSON.dump(4); 
    const char* pentruLungime = jsonString.c_str();
    int lungimeJSON = strlen(pentruLungime);

    std :: string header = construireCerereHTTPLogare(lungimeJSON);

    header.append(jsonString);
    const char* headerChar = header.c_str();

    send(socketServer, headerChar, strlen(headerChar), 0);

    char* raspunsServer = (char*) malloc(sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON);
    recv(socketServer, raspunsServer, sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON, 0);
    char* raspunsServerCopie = (char*) malloc(sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON);
    strcpy(raspunsServerCopie, raspunsServer);

    char* token = strtok(raspunsServer, " ");
    token = strtok(NULL, " ");
    std :: string codRaspuns(token);
    char* cookie = NULL;

    if (codRaspuns == "200") {
        std :: cout << "SUCCES: Ai fost conectat cu succes!" << std :: endl;

        cookie = (char*) malloc(sizeof(char) * MAX_COOKIE_SIZE);
        char* pointerStart = strstr(raspunsServerCopie, "Set-Cookie: ");
        char* pointerStop = strchr(pointerStart, ';');
        *pointerStop = '\0';

        strcpy(cookie, pointerStart + strlen("Set-Cookie: "));
        return cookie;

    } else if (codRaspuns == "400") {
        std :: cout << "EROARE: Nu exista niciun cont cu acest username si parola!" << std :: endl;
    }  

    free(raspunsServer);
    free(raspunsServerCopie);

    return cookie;
}

// FUNCTIE CE TRIMITE CEREREA CATRE SERVER PENTRU PERMISIUNEA DE ACCES LA BIBLIOTECA "enter_library"
// INTOARCE TOKEN-UL JWT
char* cerereAcces(char* cookie) {
    int socketServer = conectareLaServer();
    std :: string header = construireCerereAcces(cookie);

    const char* headerChar = header.c_str();
    send(socketServer, headerChar, strlen(headerChar), 0);

    char* raspunsServer = (char*) malloc(sizeof(char) * SERVER_RESPONSE_TOKEN_JWT);
    recv(socketServer, raspunsServer, sizeof(char) * SERVER_RESPONSE_TOKEN_JWT, 0);
    char* raspunsServerCopie = (char*) malloc(sizeof(char) * SERVER_RESPONSE_TOKEN_JWT);
    strcpy(raspunsServerCopie, raspunsServer);

    char* token = strtok(raspunsServer, " ");
    token = strtok(NULL, " ");
    std :: string codRaspuns(token);
    char* tokenJWT = NULL;
    

    if (codRaspuns == "200") {
        std :: cout << "SUCCES: Accesul permis in biblioteca!" << std :: endl;

        tokenJWT = (char*) malloc(sizeof(char) * MAX_JWT_SIZE);
        char* pointerStart = strstr(raspunsServerCopie, "token");
        char* pointerStop = strchr(pointerStart + strlen("token") + 4, '"');
        *pointerStop = '\0';

        strcpy(tokenJWT, pointerStart + strlen("token") + 3);
        return tokenJWT;

    } else {
        std :: cout << "EROARE: Nu s-a prezentat cookie-ul!" << std :: endl;
    }

    free(raspunsServer);
    free(raspunsServerCopie);

    return tokenJWT;
}


// FUNCTIE CE TRIMITE CEREREA CATRE SERVER PENTRU OBTINEREA CARTILOR DIN BIBLIOTECA "get_books"
void cerereCarti(char* tokenJWT) {
    int socketServer = conectareLaServer();
    std :: string header = construireCerereCarti(tokenJWT);

    const char* headerChar = header.c_str();
    send(socketServer, headerChar, strlen(headerChar), 0);

    char* raspunsServer = (char*) malloc(sizeof(char) * SIZE_BOOKS_RECEIVED);
    recv(socketServer, raspunsServer, sizeof(char) * SIZE_BOOKS_RECEIVED, 0);
    char* raspunsServerCopie = (char*) malloc(sizeof(char) * SIZE_BOOKS_RECEIVED);
    strcpy(raspunsServerCopie, raspunsServer);

    char* token = strtok(raspunsServer, " ");
    token = strtok(NULL, " ");
    std :: string codRaspuns(token);

    if (codRaspuns == "200") {
        std :: cout << "SUCCES: Se primesc caritle din biblioteca daca exista!" << std :: endl;
        std :: cout << std :: endl;
        afisareCarti(raspunsServerCopie);
    } else {
        std :: cout << "EROARE: Nu aveti acces la biblioteca!" << std :: endl;
    }

    free(raspunsServer);
    free(raspunsServerCopie);
}

// FUNCTIE CE TRIMITE CEREREA CATRE SERVER PENTRU OBTINEREA UNEI CARTI DUPA ID DIN BIBLIOTECA "get_book"
void cerereCarte(char* tokenJWT, int ID) {
    int socketServer = conectareLaServer();
    std :: string header = construireCerereCarte(tokenJWT, ID);

    const char* headerChar = header.c_str();
    send(socketServer, headerChar, strlen(headerChar), 0);

    char* raspunsServer = (char*) malloc(sizeof(char) * SIZE_BOOK);
    recv(socketServer, raspunsServer, sizeof(char) * SIZE_BOOK, 0);
    char* raspunsServerCopie = (char*) malloc(sizeof(char) * SIZE_BOOK);
    strcpy(raspunsServerCopie, raspunsServer);

    char* token = strtok(raspunsServer, " ");
    token = strtok(NULL, " ");
    std :: string codRaspuns(token); 

    if (codRaspuns == "200") {
        std :: cout << "SUCCES: Se primeste cartea din biblioteca!" << std :: endl;
        afisareCarte(raspunsServerCopie);
    } else if (codRaspuns == "404") {
        std :: cout << "EROARE: Nu s-a gasit nicio carte care sa corespunda id-ului!" << std :: endl;
    } else {
        std :: cout << "EROARE: Nu aveti acces la biblioteca!" << std :: endl;
    }

    free(raspunsServer);
    free(raspunsServerCopie);
}

// FUNCTIE CE TRIMITE CEREREA CATRE SERVER PENTRU ADAUGAREA UNEI CARTI IN BIBLIOTECA "add_book"
// INTOARCE CODUL DE RASPUNS AL SERVER-ULUI
std :: string adaugareCarte(char* tokenJWT, nlohmann :: json JSON) {
    int socketServer = conectareLaServer();

    std :: string jsonString = JSON.dump(4); 
    const char* pentruLungime = jsonString.c_str();
    int lungimeJSON = strlen(pentruLungime);

    std :: string header = construireCerereAdaugareCarte(tokenJWT, lungimeJSON);

    header.append(jsonString);
    const char* headerChar = header.c_str();
    send(socketServer, headerChar, strlen(headerChar), 0);

    char* raspunsServer = (char*) malloc(sizeof(char) * SIZE_BOOK);
    recv(socketServer, raspunsServer, sizeof(char) * SIZE_BOOK, 0);

    char* token = strtok(raspunsServer, " ");
    token = strtok(NULL, " ");

    std :: string codRaspuns(token);
    free(raspunsServer);
    return codRaspuns;
}

// FUNCTIE CE TRIMITE CEREREA CATRE SERVER PENTRU STERGEREA UNEI CARTI DIN BIBLIOTECA "delete_book"
void stergereCarte(char* tokenJWT, int ID) {
    int socketServer = conectareLaServer();
    std :: string header = construireCerereStergereCarte(tokenJWT, ID);

    const char* headerChar = header.c_str();
    send(socketServer, headerChar, strlen(headerChar), 0);

    char* raspunsServer = (char*) malloc(sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON);
    recv(socketServer, raspunsServer, sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON, 0);
    char* raspunsServerCopie = (char*) malloc(sizeof(char) * SERVER_RESPONSE_SIZE_NON_JSON);
    strcpy(raspunsServerCopie, raspunsServer);

    char* token = strtok(raspunsServer, " ");
    token = strtok(NULL, " ");

    std :: string codRaspuns(token);
    

    if (codRaspuns == "200") {
        std :: cout << "SUCCES: Cartea a fost stearsa!" << std :: endl;

    } else if (codRaspuns == "404") {
        std :: cout << "EROARE: Nu s-a gasit nicio carte care sa corespunda id-ului pentru a fi stearsa!" << std :: endl;
    } else {
        std :: cout << "EROARE: Nu aveti acces la biblioteca!" << std :: endl;
    }

    free(raspunsServer);
    free(raspunsServerCopie);
}

// VERIFICA DACA UN STRING CONTINE SPATIU, FOLOSIT PENTRU DETERMINAREA 
// UNUI INPUT CORECT AL CREDENTIALELOR
bool areSpatiu(std :: string credential) {
    for (size_t i = 0; i < credential.size(); i++) {
        if (credential[i] == ' ') {
            return true;
        }
    }
    return false;
}

// FUNCTIE CE PREIA COMENZILE PRIMITE LA STDIN SI REALIZEAZA ACTIUNILE SPECIFICE
void parsareComanda(std :: string comanda, char* tokenJWT) {
    if (comanda == "register") {
        std :: string username, password;
        std :: string temp;
        getline(std :: cin, temp);
        std :: cout << "username=";
        getline(std :: cin, username);
        std :: cout << "password=";
        getline(std :: cin, password);

        if (areSpatiu(username) || areSpatiu(password)) {
            std :: cout << "Credentialele nu pot avea spatii libere!" << std :: endl;
            return;
        }

        nlohmann :: json JSON;
        JSON["username"] = username;
        JSON["password"] = password;


        std :: string raspunsServer = cerereInregistrare(JSON);

        if (raspunsServer == "201") {
            std :: cout << "SUCCES: Inregistrea a avut loc cu succes!" << std :: endl;
        } else if (raspunsServer == "400") {
            std :: cout << "EROARE: Username-ul este deja folosit!" << std :: endl;
        }

    } else if (comanda == "add_book") {
        if (tokenJWT == NULL) {
            std :: cout << "EROARE: Nu se pot adaugati carti, conecteaza-te si obtine acces!" << std :: endl;
            return;
        }
        std :: string title, author, genre, page_count, publisher;
        std :: string temp;
        getline(std :: cin, temp);
        std :: cout << "title=";
        getline(std :: cin, title);
        std :: cout << "author=";
        getline(std :: cin, author);
        std :: cout << "genre=";
        getline(std :: cin, genre);
        std :: cout << "page_count=";
        getline(std :: cin, page_count);
        std :: cout << "publisher=";
        getline(std :: cin, publisher);

        if (tokenJWT == NULL) {
            std :: cout << "EROARE: Nu se poate adauga cartea, conecteaza-te si obtine acces!" << std :: endl;
            return;
        }

        stoi(page_count);
        for (size_t i = 0; i < page_count.size(); i++) {
            if (page_count[i] > 57 || page_count[i] < 48) {
                std :: cout << "Numarul de pagini este invalid, adaugati o valoare numar interg!" << std :: endl;
                return;
            }
        }

        nlohmann :: json JSON;
        JSON["title"] = title;
        JSON["author"] = author;
        JSON["genre"] = genre;
        JSON["page_count"] = page_count;
        JSON["publisher"] = publisher;

        std :: string raspunsServer = adaugareCarte(tokenJWT, JSON);

        if (raspunsServer == "200") {
            std :: cout << "SUCCES: Cartea a fost adaugata in biblioteca!" << std :: endl;
        } else {
            std :: cout << "EROARE: Nu aveti acces la biblioteca!" << std :: endl;
        }

    } else if (comanda == "delete_book") {
        if (tokenJWT == NULL) {
            std :: cout << "EROARE: Nu se poate sterge cartea, conecteaza-te si obtine acces la biblioteca!" << std :: endl;
            return;
        }

        int ID;
        std :: cout << "id=";
        std :: cin >> ID;

        stergereCarte(tokenJWT, ID); 
    } else if (comanda == "get_book") {
        if (tokenJWT == NULL) {
            std :: cout << "EROARE: Nu se poate primi cartea, conecteaza-te si obtine acces la biblioteca!" << std :: endl;
            return;
        }
        int ID;
        std :: cout << "id=";
        std :: cin >> ID;

        cerereCarte(tokenJWT, ID); 
    } else if (comanda == "get_books") {
        if (tokenJWT == NULL) {
            std :: cout << "EROARE: Nu se pot primi cartile, conecteaza-te si obtine acces la biblioteca!" << std :: endl;
            return;
        }
        cerereCarti(tokenJWT);
    } else if (comanda == "logout") {
        std :: cout << "avem logout" << std :: endl;
    } else {
        std :: cout << "Comanda inexistenta!" << std :: endl;
    }
}   

int main() {
    
    // COOKIES SI TOKENJWT SPECIFICE UNEI SESIUNI
    // SUNT INITIALIZATE CU NULL, URMAND SA LE FIE ATRIBUITE VALORI CAND
    // SE REALIZEAZA LOGAREA RESPECTIV "enter_library", CA MAI APOI LA DELOGARE SA FIE REINITIALIZATE CU NULL
    char* cookies = NULL;
    char* tokenJWT = NULL;

    while (1) {
        std :: cout << "Se asteapta comanda:" << std :: endl;
        std :: string comanda;
        std :: cin >> comanda;
        if (comanda == "exit") {
            break;
        } else if (comanda == "login") {
            cookies = NULL;
            tokenJWT = NULL;
            std :: string username, password;
            std :: string temp;
            getline(std :: cin, temp);
            std :: cout << "username=";
            getline(std :: cin, username);
            std :: cout << "password=";
            getline(std :: cin, password);

            if (areSpatiu(username) || areSpatiu(password)) {
                std :: cout << "Credentialele nu pot avea spatii libere!" << std :: endl;
                continue;
            }

            nlohmann :: json JSON;
            JSON["username"] = username;
            JSON["password"] = password;

            cookies = (char*) malloc(sizeof(char) * MAX_COOKIE_SIZE);
            cookies = cerereLogare(JSON);

        } else if (comanda == "enter_library") {
            if (cookies == NULL) {
                std :: cout << "EROARE: Nu poti obtine acces la biblioteca, mai intai conecteaza-te!" << std :: endl;
                continue;
            }
            tokenJWT = cerereAcces(cookies);
        } else if (comanda == "logout") {
            if (cookies == NULL) {
                std :: cout << "EROARE: Nu esti conectat, pentru a te deconecta!" << std :: endl;
                continue;
            }
            if (cookies != NULL) {
                free(cookies);
                free(tokenJWT);
                cookies = NULL;
                tokenJWT = NULL;
            }
            std :: cout << "SUCCES: Te-ai deconectat din cont!" << std :: endl;
            continue;
        } 
        else {
            parsareComanda(comanda, tokenJWT);
        }
    
    }

    return 0;
}