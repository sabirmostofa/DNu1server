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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


char *bit_receive(char *buffer, int cnt){
    
    char *buffer1, *ptr, c, c1=0;
    int i = 0;
    if(cnt % 8 != 0) return 0;  //we must have n whole bytes!
    
    ptr = buffer1 = (char*) malloc(cnt/8);

    while(cnt != 0){   
        cnt--;    
        c = *buffer;           

        if(c != '0'){
            c1 |= 0x80 >> i;    
        }
        i++;                  
        if(i>7){                
            i=0;
            *ptr = c1;          
            c1 = 0;             
            ptr++;              
        }
        buffer++;             
    }
    return buffer1; 
}

int main(int argc, char *argv[])
{
 	int rc;
	int s, s1;
    char key[] = "exit";
    char key1[] = "stop";
    char line[1024];

	struct sockaddr_in addr;
	s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;

    // set SO_REUSEADDR on a socket to true (1):
	int optval = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
	addr.sin_port = htons(3334);	// little endian -> big endian
	rc = bind( s, (struct sockaddr *) &addr, sizeof(addr) );
	if(rc < 0){
		printf("bind failed\n");
		return 0;
	}
	do{
		rc = listen( s, 10 ); // warte auf ankommendes connect
		if(rc < 0){
			printf("listen failed\n");
			return 0;
		}
		printf("listen\n");
		s1 = accept( s, NULL, NULL );
		printf("accepted\n");
		//connected1 = 1;
		do{
			usleep(30000);
			rc = recv( s1, line, sizeof(line), 0 );
			if(rc > 0){
                            printf("size: %d  bits\n", rc);
                            char *received = bit_receive(line, rc);
                            printf("%s \n", received);
//				line[rc] = '\0';
//				printf("%s \n",line);
			}
		}while ((strcmp (key,line)) != 0 && (strcmp (key1,line)) != 0 && (rc > 0) );
	}while((strcmp (key1,line)) != 0);
	shutdown(s, SHUT_RDWR);
    return 0;
}
