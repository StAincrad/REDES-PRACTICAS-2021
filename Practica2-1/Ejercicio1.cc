#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

int main(int argc, char* argv[]){

	struct addrinfo  hints;
	struct addrinfo* result;

	//Obtain addess(es) matching host/port/
	
	memset((void*) &hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;	//Allow IPv4 or IPv6
	//hints.ai_socktype = SOCK_STREAM;//Datagram socket
	//hints.ai_flags = 0;
	//hints.ai_protocol = 0;	//Any protocol
	
	int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
	if(rc != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
		exit(EXIT_FAILURE);
	}

	for(addrinfo* i = result; i != NULL; i = i->ai_next) {
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		
		getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv,
				NI_MAXSERV, NI_NUMERICHOST);
		std::cout << "Host: " << host << " Port: " << i->ai_family << 
			" Socket Type: " << i->ai_socktype << std::endl;
	}

	freeaddrinfo(result);	//No longer needed

	return 0;
}
