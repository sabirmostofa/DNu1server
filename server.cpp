//http://www.linuxhowtos.org/C_C++/socket.htm
//http://www2.cs.uidaho.edu/~krings/CS270/Notes.S10/270-F10-28.pdf
//http://gnosis.cx/publish/programming/sockets.html
//
//      server.cpp
//


/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <math.h>
#include <iostream>

using namespace std;

unsigned int hackers_delight_crc32(unsigned char *message) {
    int i, j;
    unsigned int byte, crc, mask;
    i = 0;
    crc = 0xFFFFFFFF;
    while (message[i] != 0) {
        byte = message[i]; // Get next byte.
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--) { // Do eight times.
            mask = -(int) (crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        i = i + 1;
    }
    return ~crc;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}



uint32_t lcg_seed = 1;

uint32_t lcg_rand() {
    //lcg_seed = ((uint64_t)lcg_seed * 279470273UL) % 4294967291UL;
    lcg_seed = rand();
    return lcg_seed;
}
// pseudo-random in [0,1]

double randf() {
    return lcg_rand() / (double) 0xFFFFFFFFUL;
}
const double PI = 3.14159265;
// pseudo-Rauschen gaussian

double randf_gauss() {
    double mu = 0.0; // mittelwert
    // bei größer sigma mehrere Veränderungen
    double sigma = 0.55; //!! varianz 0.45; try a values from 0.25 to 0.55
    return mu + sigma * sqrt(-2.0 * log(randf())) * cos(2 * PI * randf());
}

/****************** Analog Kanal Modell mit Rauschen *******************
 *                am Eingang ein originelles Bit aus den Datastream
 *                das Bit ist in analoge Grösse umgewandelt, gedämmt
 *                und mit dem Rauschen zusammengemischt.
 *                Das Ergebnis wird wieder als ein Bit dargestellt
 *                Am Ausgang ein Bit mit möglicher Störung             
 ***********************************************************************/
char analog_kanal_modell(char inputbit) { //add noise to the bit stream
    //je schwächer signal ist,desto mehr Veränderung
    double input_signal_level = 0.08;
    char outputbit;
    double in, noise, out;
    /////////////////////  Digital to Analog conversion //////////////////
    if (inputbit != '0') {
        in = +input_signal_level;
    } else {
        in = -input_signal_level;
    }
    ///////////////////  Störungen im Kanal //////////////////    
    noise = 0.1 * randf_gauss(); // pseudo-zufällige Zahlen, Gauss Verteilung
    out = in + noise; // Analogsignal mit Rauschen
    /////////////////// Analog to Digital conversion //////////////////
    if (out > 0.0)
        outputbit = '1';
    else
        outputbit = '0';
    return outputbit;
}

char *bit_receive(char *buffer, int cnt) {

    char *buffer1, *ptr, c, c1 = 0;
    int i = 0;
    if (cnt % 8 != 0) return 0; //we must have n whole bytes!

    ptr = buffer1 = (char*) malloc(cnt / 8);

    while (cnt != 0) {
        cnt--;
        c = *buffer;

        if (c != '0') {
            c1 |= 0x80 >> i;
        }
        i++;
        if (i > 7) {
            i = 0;
            *ptr = c1;
            c1 = 0;
            ptr++;
        }
        buffer++;
    }
    return buffer1;
}

int main(int argc, char *argv[]) {
    int rc;
    int s, s1;
    char key[] = "exit";
    char key1[] = "stop";
    char file[1024 * 8];

    struct sockaddr_in addr;
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    // set SO_REUSEADDR on a socket to true (1):
    int optval = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    addr.sin_port = htons(3334); // little endian -> big endian
    rc = bind(s, (struct sockaddr *) &addr, sizeof (addr));
    if (rc < 0) {
        printf("bind failed\n");
        return 0;
    }
    do {
        rc = listen(s, 10); // warte auf ankommendes connect
        if (rc < 0) {
            printf("listen failed\n");
            return 0;
        }
        printf("listen\n");
        s1 = accept(s, NULL, NULL);
        printf("accepted\n");
        unsigned int fileSize;
        unsigned int crcVal;

        recv(s1, &fileSize, sizeof (unsigned int), 0);
        recv(s1, &crcVal, sizeof (unsigned int), 0);
        printf(" File Size : %d Bytes \n", fileSize);
        printf(" CRC Value : %d\n", crcVal);
        //connected1 = 1;

        int count = 0;
        do {
            usleep(30000);
            rc = recv(s1, file, fileSize, 0);




            //if received
            if (rc > 0) {


                char fileModified[1024 * 8];

                for (int i = 0; i < fileSize; i++) {
                    fileModified[i] = analog_kanal_modell(file[i]);
                }

                char *buffer = fileModified;

                char *zBuffer = bit_receive(buffer, fileSize);

                //std::cout << line;

                FILE *File;
                File = fopen("received.txt", "wb");

                fwrite(zBuffer, 1, fileSize / 8, File);
                fclose(File);

                printf("size: %d  bytes\n", rc);
                char *received = bit_receive(file, rc);
                
                unsigned int crcval_server = hackers_delight_crc32((unsigned char*) zBuffer);

                if (crcval_server == crcVal)
                    cout << "Nachrict unverändert" << endl;
                else

                    cout << "Nachrict verändert" << endl;

                //printf("%s \n", received);
                //				line[rc] = '\0';
                //				printf("%s \n",line);
            }
        } while ((strcmp(key, file)) != 0 && (strcmp(key1, file)) != 0 && (rc > 0));

    } while ((strcmp(key1, file)) != 0);
    shutdown(s, SHUT_RDWR);
    return 0;
}
