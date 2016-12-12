#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <stdarg.h>
#include <errno.h>

void error_fatal(char *format, ...);

const char* host = "localhost";
const char* user = "root";
const char* password = "";
const char* db = "online_kursevi";

int main() {
    int quit = 0;
    MYSQL *conn = mysql_init(NULL);
    
    if(mysql_real_connect(conn, host, user, password, db, 0, NULL, 0) == NULL)
        error_fatal("Greska u konekciji: %s\n", mysql_error(conn));
    
    while(!quit) {
        int opcija;
        
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
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                quit = 1;
                break;
            default:
                printf("Ova opcija ne postoji\n");
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