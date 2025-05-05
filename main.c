
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_FACILITATI 5
#define MAX_CAMERE 10
#define MAX_REZERVARI 10
#define BLUE    "\033[34m"
#define RESET    "\033[0m"
HANDLE hConsole;

typedef struct {
    int numar;
    char tip[20];           // "Single", "Double", "Apartament"
    float pret;
    int esteDisponibila;    // 1 = disponibil, 0 = ocupat
    int numarPersoane;      // Numărul de persoane maxim
    char facilitati[MAX_FACILITATI][50];  // Facilități, separate prin virgulă
} Camera;

typedef struct {
    int numarCamera;
    char tip[20];
    int numarPersoane;
    int perioadaInceput;    // Perioada de început în zile
    int perioadaSfarsit;    // Perioada de sfârșit în zile
    float pretTotal;
    char facilitati[MAX_FACILITATI][50];
    int esteAnulata;        // 0 = activă, 1 = anulată
} Rezervare;

// Funcție pentru curățarea consolei
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Funcție pentru golirea buffer-ului de intrare
void flushInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void salveazaRezervareInFisier(const char *filename, Rezervare rezervare) {
    FILE *f = fopen("rezervari.txt", "a");  // Deschide în mod append
    if (!f) {
        perror("Eroare la deschiderea fisierului rezervari");
        return;
    }
    fprintf(f, "%d %s %d %d %d %.2f ",
            rezervare.numarCamera,
            rezervare.tip,
            rezervare.numarPersoane,
            rezervare.perioadaInceput,
            rezervare.perioadaSfarsit,
            rezervare.pretTotal);
    for (int i = 0; i < MAX_FACILITATI && strlen(rezervare.facilitati[i]) > 0; i++) {
        fprintf(f, "%s,", rezervare.facilitati[i]);
    }
    fprintf(f, "\n");
    fclose(f);
}
// Funcția de citire a camerelor din fișier
int citesteCamere(const char *filename, Camera camere[], int maxCamere) {
    FILE *f = fopen("camsss.txt", "r");
    if (!f) {
        perror("Eroare la deschiderea fisierului");
        return 0;
    }
    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), f) && count < maxCamere) {
        // Formatul unei linii:
        // numar tip pret esteDisponibila numarPersoane facilitati_separate_prin_virgulă
        char facilitatiStr[200] = "";
        int ret = sscanf(line, "%d %s %f %d %d %[^\n]",
                         &camere[count].numar,
                         camere[count].tip,
                         &camere[count].pret,
                         &camere[count].esteDisponibila,
                         &camere[count].numarPersoane,
                         facilitatiStr);
        if (ret >= 5) {
            for (int k = 0; k < MAX_FACILITATI; k++) {
                camere[count].facilitati[k][0] = '\0';
            }
            if (ret == 6) {
                int j = 0;
                char *token = strtok(facilitatiStr, ",");
                while (token != NULL && j < MAX_FACILITATI) {
                    strcpy(camere[count].facilitati[j], token);
                    token = strtok(NULL, ",");
                    j++;
                }
            }
            count++;
        }
    }
    fclose(f);
    return count;
}
void actualizeazaStareCameraInFisier(const char *filename, Camera camere[], int totalCamere) {
    FILE *f = fopen("camsss.txt", "w");  // Rescrie tot fișierul
    if (!f) {
        perror("Eroare la deschiderea fisierului camere");
        return;
    }
    for (int i = 0; i < totalCamere; i++) {
        fprintf(f, "%d %s %.2f %d %d ",
                camere[i].numar,
                camere[i].tip,
                camere[i].pret,
                camere[i].esteDisponibila,
                camere[i].numarPersoane);
        for (int j = 0; j < MAX_FACILITATI && strlen(camere[i].facilitati[j]) > 0; j++) {
            fprintf(f, "%s,", camere[i].facilitati[j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

// Afișează camerele disponibile (excluzând apartamentele)
void afiseazaCamereDisponibile(Camera camere[], int totalCamere) {
    printf("=== CAMERE DISPONIBILE ===\n\n");
    int gasit = 0;
    for (int i = 0; i < totalCamere; i++) {
        if (camere[i].esteDisponibila && strcmp(camere[i].tip, "Apartament") != 0) {
                SetConsoleTextAttribute(hConsole, 1);
            printf("%sCamera%s %d - %s - %.2f RON/noapte (Max %d persoane)\n",
       BLUE, RESET,
       camere[i].numar, camere[i].tip, camere[i].pret, camere[i].numarPersoane);

            printf("Facilitati: ");
            for (int j = 0; j < MAX_FACILITATI && strlen(camere[i].facilitati[j]) > 0; j++) {
                printf("%s; ", camere[i].facilitati[j]);
            }
            printf("\n\n");
            gasit = 1;
        }
    }
    if (!gasit) {
        printf("Nu sunt camere disponibile.\n\n");
    }
}

// Afișează apartamentele disponibile
void afiseazaApartamenteDisponibile(Camera camere[], int totalCamere) {
    printf("=== APARTAMENTE DISPONIBILE ===\n\n");
    int gasit = 0;
    for (int i = 0; i < totalCamere; i++) {
        if (camere[i].esteDisponibila && strcmp(camere[i].tip, "Apartament") == 0) {
            SetConsoleTextAttribute(hConsole, 5);
            printf("%sApartament%s %d - %s - %.2f RON/noapte (Max %d persoane)\n",
       BLUE, RESET,
       camere[i].numar, camere[i].tip, camere[i].pret, camere[i].numarPersoane);

            printf("Facilitati: ");
            for (int j = 0; j < MAX_FACILITATI && strlen(camere[i].facilitati[j]) > 0; j++) {
                printf("%s; ", camere[i].facilitati[j]);
            }
            printf("\n\n");
            gasit = 1;
        }
    }
    if (!gasit) {
        printf("Nu sunt apartamente disponibile.\n\n");
    }
}

// Căutare camere după tip
void cautaCamereDupaTip(Camera camere[], int totalCamere) {
    char tipCamera[20];
    printf("=== CAUTARE DUPA TIP ===\n\n");
    printf("Introduceti tipul camerei (Single/Double/Apartament): ");
    fgets(tipCamera, 20, stdin);
    tipCamera[strcspn(tipCamera, "\n")] = '\0';

    printf("\n=== Rezultate pentru tipul %s ===\n\n", tipCamera);
    int gasit = 0;
    for (int i = 0; i < totalCamere; i++) {
        if (strcmp(camere[i].tip, tipCamera) == 0 && camere[i].esteDisponibila) {
            if (strcmp(camere[i].tip, "Apartament") == 0) {
                printf("Apartament %d - %.2f RON/noapte (Max %d persoane)\n",
                       camere[i].numar, camere[i].pret, camere[i].numarPersoane);
            } else {
                printf("Camera %d - %.2f RON/noapte (Max %d persoane)\n",
                       camere[i].numar, camere[i].pret, camere[i].numarPersoane);
            }
            printf("\n");
            gasit = 1;
        }
    }
    if (!gasit) {
        printf("Nu sunt camere disponibile de tipul %s.\n\n", tipCamera);
    }
}

// Căutare camere după facilități
void cautaCamereDupaFacilitati(Camera camere[], int totalCamere) {
    char facilitatiCautate[200];
    printf("=== CAUTARE DUPA FACILITATI ===\n\n");
    printf("Introduceti facilitati cautate (separate prin virgula, de ex: WiFi,TV,Balcon): ");
    fgets(facilitatiCautate, 200, stdin);
    facilitatiCautate[strcspn(facilitatiCautate, "\n")] = '\0';

    printf("\n=== Rezultate pentru facilitati: %s ===\n\n", facilitatiCautate);
    int gasit = 0;
    for (int i = 0; i < totalCamere; i++) {
        int contineToateFacilitati = 1;
        char facilitatiTemp[200];
        strcpy(facilitatiTemp, facilitatiCautate);
        char *facilitate = strtok(facilitatiTemp, ",");
        while (facilitate != NULL) {
            int gasitFacilitate = 0;
            for (int j = 0; j < MAX_FACILITATI && strlen(camere[i].facilitati[j]) > 0; j++) {
                if (strstr(camere[i].facilitati[j], facilitate) != NULL) {
                    gasitFacilitate = 1;
                    break;
                }
            }
            if (!gasitFacilitate) {
                contineToateFacilitati = 0;
                break;
            }
            facilitate = strtok(NULL, ",");
        }
        if (contineToateFacilitati && camere[i].esteDisponibila) {
            if (strcmp(camere[i].tip, "Apartament") == 0) {
                printf("Apartament %d - %.2f RON/noapte (Max %d persoane)\n",
                       camere[i].numar, camere[i].pret, camere[i].numarPersoane);
            } else {
                printf("Camera %d - %.2f RON/noapte (Max %d persoane)\n",
                       camere[i].numar, camere[i].pret, camere[i].numarPersoane);
            }
            printf("\n");
            gasit = 1;
        }
    }
    if (!gasit) {
        printf("Nu sunt camere disponibile cu aceste facilitati.\n\n");
    }
}

// Rezervă o cameră (doar pentru tipurile "Single" și "Double")
void rezervaCamera(Camera camere[], int totalCamere, Rezervare rezervari[], int *totalRezervari) {
    int optiuneTipCamera;
    char tipCamera[20];

    printf("=== REZERVA CAMERA ===\n\n");
    printf("Alegeti tipul camerei:\n");
    printf("1. Single\n2. Double\n");
    printf("Introduceti optiunea (1-2): ");
    scanf("%d", &optiuneTipCamera);
    flushInput();

    switch (optiuneTipCamera) {
        case 1:
            strcpy(tipCamera, "Single");
            break;
        case 2:
            strcpy(tipCamera, "Double");
            break;
        default:
            printf("Optiune invalida.\n\n");
            return;
    }

    int numarPersoane;
    printf("Introdu numarul de persoane: ");
    scanf("%d", &numarPersoane);
    flushInput();

    printf("\n=== Camere disponibile pentru tipul %s ===\n\n", tipCamera);
    int gasit = 0;
    for (int i = 0; i < totalCamere; i++) {
        if (strcmp(camere[i].tip, tipCamera) == 0 && camere[i].esteDisponibila && camere[i].numarPersoane >= numarPersoane) {
            printf("Camera %d - %.2f RON/noapte (Max %d persoane)\n",
                   camere[i].numar, camere[i].pret, camere[i].numarPersoane);
            printf("\n");
            gasit = 1;
        }
    }

    if (!gasit) {
        printf("Nu sunt camere de tipul %s disponibile pentru %d persoane.\n\n", tipCamera, numarPersoane);
        return;
    }

    int numarCamera;
    printf("Alegeti numarul camerei dorite: ");
    scanf("%d", &numarCamera);
    flushInput();

    int perioadaInceput, perioadaSfarsit;
    printf("Introduceti perioada de inceput a rezervarii (in zile, de ex 1 pentru prima zi): ");
    scanf("%d", &perioadaInceput);
    flushInput();
    printf("Introduceti perioada de sfarsit a rezervarii (in zile,de ex 3 pentru ultima zi): ");
    scanf("%d", &perioadaSfarsit);
    flushInput();

    int cameraGasita = 0;
    for (int i = 0; i < totalCamere; i++) {
        if (camere[i].numar == numarCamera && camere[i].esteDisponibila && strcmp(camere[i].tip, tipCamera) == 0) {
            cameraGasita = 1;
            rezervari[*totalRezervari].numarCamera = camere[i].numar;
            strcpy(rezervari[*totalRezervari].tip, camere[i].tip);
            rezervari[*totalRezervari].numarPersoane = numarPersoane;
            rezervari[*totalRezervari].perioadaInceput = perioadaInceput;
            rezervari[*totalRezervari].perioadaSfarsit = perioadaSfarsit;
            rezervari[*totalRezervari].pretTotal = camere[i].pret * (perioadaSfarsit - perioadaInceput);
            for (int j = 0; j < MAX_FACILITATI && strlen(camere[i].facilitati[j]) > 0; j++) {
                strcpy(rezervari[*totalRezervari].facilitati[j], camere[i].facilitati[j]);
            }
            rezervari[*totalRezervari].esteAnulata = 0;
            camere[i].esteDisponibila = 0;
            (*totalRezervari)++;
            break;
        }
    }

    if (!cameraGasita) {
        printf("Camera cu numarul %d nu este disponibila.\n\n", numarCamera);
    } else {
        printf("Rezervare camera efectuata cu succes!\n\n");
    }
    salveazaRezervareInFisier("rezervari.txt", rezervari[*totalRezervari - 1]);
    actualizeazaStareCameraInFisier("cams.txt", camere, totalCamere);
}

// Rezervă un apartament
void rezervaApartament(Camera camere[], int totalCamere, Rezervare rezervari[], int *totalRezervari) {
    int numarPersoane;
    printf("=== REZERVA APARTAMENT ===\n\n");
    printf("Introduceti numarul de persoane pentru apartament: ");
    scanf("%d", &numarPersoane);
    flushInput();

    printf("\n=== Apartamente disponibile ===\n\n");
    int gasit = 0;
    for (int i = 0; i < totalCamere; i++) {
        if (strcmp(camere[i].tip, "Apartament") == 0 && camere[i].esteDisponibila && camere[i].numarPersoane >= numarPersoane) {
            printf("Apartament %d - %.2f RON/noapte (Max %d persoane)\n",
                   camere[i].numar, camere[i].pret, camere[i].numarPersoane);
            printf("\n");
            gasit = 1;
        }
    }

    if (!gasit) {
        printf("Nu sunt apartamente disponibile pentru %d persoane.\n\n", numarPersoane);
        return;
    }

    int numarCamera;
    printf("Alegeti numarul apartamentului dorit: ");
    scanf("%d", &numarCamera);
    flushInput();

    int perioadaInceput, perioadaSfarsit;
    printf("Introduceti perioada de inceput a rezervarii (in zile): ");
    scanf("%d", &perioadaInceput);
    flushInput();
    printf("Introduceti perioada de sfarsit a rezervarii (in zile): ");
    scanf("%d", &perioadaSfarsit);
    flushInput();

    int apartamentGasit = 0;
    for (int i = 0; i < totalCamere; i++) {
        if (camere[i].numar == numarCamera && camere[i].esteDisponibila && strcmp(camere[i].tip, "Apartament") == 0) {
            apartamentGasit = 1;
            rezervari[*totalRezervari].numarCamera = camere[i].numar;
            strcpy(rezervari[*totalRezervari].tip, camere[i].tip);
            rezervari[*totalRezervari].numarPersoane = numarPersoane;
            rezervari[*totalRezervari].perioadaInceput = perioadaInceput;
            rezervari[*totalRezervari].perioadaSfarsit = perioadaSfarsit;
            rezervari[*totalRezervari].pretTotal = camere[i].pret * (perioadaSfarsit - perioadaInceput);
            for (int j = 0; j < MAX_FACILITATI && strlen(camere[i].facilitati[j]) > 0; j++) {
                strcpy(rezervari[*totalRezervari].facilitati[j], camere[i].facilitati[j]);
            }
            rezervari[*totalRezervari].esteAnulata = 0;
            camere[i].esteDisponibila = 0;
            (*totalRezervari)++;
            break;
        }
    }

    if (!apartamentGasit) {
        printf("Apartamentul cu numarul %d nu este disponibila.\n\n", numarCamera);
    } else {
        printf("Rezervare apartament efectuata cu succes!\n\n");
    }
    salveazaRezervareInFisier("rezervari.txt", rezervari[*totalRezervari - 1]);
    actualizeazaStareCameraInFisier("camerre.txt", camere, totalCamere);
}

// Afișează detaliile rezervărilor
void detaliiRezervare(Rezervare rezervari[], int totalRezervari) {
    printf("=== DETALII REZERVARI ===\n\n");
    if (totalRezervari == 0) {
        printf("Nu aveti nicio rezervare.\n\n");
        return;
    }
    for (int i = 0; i < totalRezervari; i++) {
        printf("Rezervare #%d:\n", i + 1);
        printf("  Camera/Apartament: %d (%s)\n", rezervari[i].numarCamera, rezervari[i].tip);
        printf("  Numar persoane: %d\n", rezervari[i].numarPersoane);
        printf("  Perioada: de la ziua %d pana la ziua %d\n", rezervari[i].perioadaInceput, rezervari[i].perioadaSfarsit);
        printf("  Pret total: %.2f RON\n", rezervari[i].pretTotal);
        printf("  Status: %s\n", (rezervari[i].esteAnulata ? "Anulata" : "Activa"));
        printf("  Facilitati: ");
        for (int j = 0; j < MAX_FACILITATI && strlen(rezervari[i].facilitati[j]) > 0; j++) {
            printf("%s; ", rezervari[i].facilitati[j]);
        }
        printf("\n\n");
    }
}

// Anulează o rezervare
void anuleazaRezervare(Rezervare rezervari[], int *totalRezervari) {
    int numarRezervare;
    printf("=== ANULARE REZERVARE ===\n\n");
    printf("Introduceti numarul rezervarii de anulat: ");
    scanf("%d", &numarRezervare);
    flushInput();

    if (numarRezervare < 1 || numarRezervare > *totalRezervari || rezervari[numarRezervare - 1].esteAnulata) {
        printf("Rezervarea nu poate fi anulata.\n\n");
        return;
    }

    rezervari[numarRezervare - 1].esteAnulata = 1;
    printf("Rezervarea #%d a fost anulata cu succes.\n\n", numarRezervare);
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    Camera camere[MAX_CAMERE];
    int totalCamere = citesteCamere("cams.txt", camere, MAX_CAMERE);
    if (totalCamere == 0) {
        printf("Nu s-au putut citi camerele din fisier.\n");
        return 1;
    }

    Rezervare rezervari[MAX_REZERVARI];
    int totalRezervari = 0;
    int optiune;

    while (1) {
        clearConsole();
        SetConsoleTextAttribute(hConsole, 10);
        printf("===== MENIU PRINCIPAL HOTEL =====\n\n");
        SetConsoleTextAttribute(hConsole, 12);
        printf("1. Afiseaza camere disponibile\n");
        printf("2. Afiseaza apartamente disponibile\n");
        printf("3. Cauta dupa tip\n");
        printf("4. Cauta dupa facilitati\n");
        printf("5. Rezerva camera\n");
        printf("6. Rezerva apartament\n");
        printf("7. Detalii rezervari\n");
        printf("8. Anuleaza rezervare\n");
        printf("9. Iesire\n\n");
        printf("Alegeti o optiune: ");
        scanf("%d", &optiune);
        flushInput();

        clearConsole();
        switch (optiune) {
            case 1:
                afiseazaCamereDisponibile(camere, totalCamere);
                break;
            case 2:
                afiseazaApartamenteDisponibile(camere, totalCamere);
                break;
            case 3:
                cautaCamereDupaTip(camere, totalCamere);
                break;
            case 4:
                cautaCamereDupaFacilitati(camere, totalCamere);
                break;
            case 5:
                rezervaCamera(camere, totalCamere, rezervari, &totalRezervari);
                break;
            case 6:
                rezervaApartament(camere, totalCamere, rezervari, &totalRezervari);
                break;
            case 7:
                detaliiRezervare(rezervari, totalRezervari);
                break;
            case 8:
                anuleazaRezervare(rezervari, &totalRezervari);
                break;
            case 9:
                printf("Iesire.\n");
                return 0;
            default:
                printf("Optiune invalida.\n\n");
        }
        printf("Apasati Enter pentru a reveni la meniul principal...");
        getchar();
    }

    return 0;
}
