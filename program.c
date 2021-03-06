#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <stdarg.h>
#include <errno.h>
#include <stdint.h>

#define QUERYLEN 1024
#define UINPUTLEN 100

void error_fatal(char *format, ...);

const char* host = "localhost";
const char* user = "root";
const char* password = "";
const char* db = "online_kursevi";

int main() {
    int quit = 0, i, id_kursa, pomeraj;
    char query[QUERYLEN];
    char input[UINPUTLEN], kor_ime[46], ime[46], prezime[46], email[46], datp[11], datk[11];
    MYSQL* conn = mysql_init(NULL);
    MYSQL_RES* result_set;
    MYSQL_FIELD* field;
    MYSQL_ROW row;
    
    if(mysql_real_connect(conn, host, user, password, db, 0, NULL, 0) == NULL)
        error_fatal("Greska u konekciji: %s\n", mysql_error(conn));
    
    while(!quit) {
        int opcija;
        
        system("clear");
        printf("-------------------------------------\n");
        printf("Odaberite jednu od opcija:\n");
        printf("\t[1] - Izlistati informacije o korisniku, kurseve koje slusa i njihove informacije\n");
        printf("\t[2] - Izlistati informacije o ucesnicima koji su dobili sertifikat, i informacije o odgovarajucim kursevima\n");
        printf("\t[3] - Registrovati novog ucesnika\n");
        printf("\t[4] - Dodati novi ciklus\n");
        printf("\t[5] - Prijavljivanje ucesnika za kurs\n");
        printf("\t[6] - Azuriranje progresa\n");
        printf("\t[7] - Izlaz\n");
        
        scanf("%d", &opcija);
        switch(opcija) {
            case 1:
                getchar();
                
                printf("Unesite korisnicko ime: ");
                fgets(input, UINPUTLEN, stdin);
                input[strlen(input) - 1] = '\0';
                
                sprintf(query, "SELECT ime, prezime, poeni, naziv, progres, datum_pocetka, datum_kraja FROM Ucesnik JOIN Korisnik ON Korisnik_kor_ime = kor_ime LEFT JOIN Prijavljuje ON Ucesnik_Korisnik_kor_ime = Korisnik_kor_ime LEFT JOIN Ciklus ON datum_pocetka = Ciklus_datum_pocetka AND Kurs_Sifra = Ciklus_Kurs_sifra LEFT JOIN Kurs ON sifra = Kurs_sifra WHERE Korisnik_kor_ime = '%s'", input);
                
                if(mysql_query(conn, query))
                    error_fatal("Upit 1: %s\n", mysql_error(conn));
                
                result_set = mysql_use_result(conn);
                int num_fields = mysql_num_fields(result_set);
                field = mysql_fetch_fields(result_set);
                
                while((row = mysql_fetch_row(result_set)) != NULL) {
                    printf("--------------------\n");
                    for(i = 0; i < num_fields; i++)
                        printf("%-20s| %s\n", field[i].name, row[i]);
                }
                
                mysql_free_result(result_set);
                break;
            case 2:
                getchar();
                
                sprintf(query, "SELECT Sertifikat.id AS br_sertifikata, datum_dodele, Korisnik.*, poeni, sifra AS sifra_kursa, naziv, trajanje, max_ucesnika FROM Sertifikat JOIN Korisnik ON Prijavljuje_Ucesnik_Korisnik_kor_ime = kor_ime JOIN Ucesnik ON kor_ime = Korisnik_kor_ime JOIN Kurs ON Prijavljuje_Ciklus_Kurs_sifra = sifra; ");
                
                if(mysql_query(conn, query))
                    error_fatal("Upit 2: %s\n", mysql_error(conn));
                
                result_set = mysql_use_result(conn);
                num_fields = mysql_num_fields(result_set);
                field = mysql_fetch_fields(result_set);
                
                while((row = mysql_fetch_row(result_set)) != NULL) {
                    printf("--------------------\n");
                    for(i = 0; i < num_fields; i++)
                        printf("%-20s| %s\n", field[i].name, row[i]);
                }
                
                mysql_free_result(result_set);
                break;
            case 3:
                getchar();
                
                printf("Unesite novo korisnicko ime, ime, prezime i email: ");
                scanf("%s %s %s %s", kor_ime, ime, prezime, email);
                
                sprintf(query, "INSERT INTO Korisnik(kor_ime, ime, prezime, email) VALUES ('%s', '%s', '%s', '%s')", kor_ime, ime, prezime, email);
                if(mysql_query(conn, query))
                    error_fatal("Upit 3: %s\n", mysql_error(conn));
                
                sprintf(query, "INSERT INTO Ucesnik(Korisnik_kor_ime, poeni) VALUES ('%s', 0)", kor_ime);
                if(mysql_query(conn, query))
                    error_fatal("Upit 3: %s\n", mysql_error(conn));
                
                getchar();
                break;
            case 4:
                getchar();
                
                printf("Unesite id kursa, datum pocetka i datum kraja kursa (datum u obliku YYYY-MM-DD): ");
                scanf("%d %s %s", &id_kursa, datp, datk);
                
                sprintf(query, "INSERT INTO Ciklus(Kurs_sifra, datum_pocetka, datum_kraja) VALUES (%d, '%s', '%s')", id_kursa, datp, datk);
                if(mysql_query(conn, query))
                    error_fatal("Upit 4: %s\n", mysql_error(conn));
                
                getchar();
                break;
            case 5:
                getchar();
                
                printf("Unesite id kursa, datum pocetka i korisnicko ime: ");
                scanf("%d %s %s", &id_kursa, datp, kor_ime);
                
                sprintf(query, "INSERT INTO Prijavljuje(Ciklus_Kurs_sifra, Ciklus_datum_pocetka, Ucesnik_Korisnik_kor_ime, progres) VALUES (%d, '%s', '%s', 0)", id_kursa, datp, kor_ime);
                if(mysql_query(conn, query))
                    error_fatal("Upit 5: %s\n", mysql_error(conn));
                
                getchar();
                break;
            case 6:
                getchar();
                
                printf("Unesite korisnicko ime, sifru kursa, datum pocetka i za koliko se uvecava progres: ");
                scanf("%s %d %s %d", kor_ime, &id_kursa, datp, &pomeraj);
                
                sprintf(query, "UPDATE Prijavljuje SET progres = progres + %d WHERE Ciklus_datum_pocetka = '%s' AND Ciklus_Kurs_sifra = %d AND Ucesnik_Korisnik_kor_ime = '%s'", pomeraj, datp, id_kursa, kor_ime);
                if(mysql_query(conn, query))
                    error_fatal("Upit 6: %s\n", mysql_error(conn));
                
                getchar();
                break;
            case 7:
                quit = 1;
                break;
            default:
                printf("Ova opcija ne postoji\n");
        }
        if(!quit) {
            printf("----------------------------------\n");
            printf("Da se vratite na glavni meni, pritisnite enter ");
            getchar();
        }
    }
    
    mysql_close(conn);
    
    return 0;
}

void error_fatal (char* format, ...)
{
  va_list arguments;

  va_start (arguments, format);
  vfprintf (stderr, format, arguments);
  va_end (arguments);

  exit (EXIT_FAILURE);
}