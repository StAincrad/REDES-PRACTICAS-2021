#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include <iostream>

int main(int argc, char** argv){
	struct addrinfo hints;
	struct addrinfo *res;

	memset((void*) &hints, '\0', sizeof(struct addrinfo));
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	
	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	
	if(rc != 0)
	{
		std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
		return -1;
	}

	int sd = socket(res->ai_family, res->ai_socktype, '\0');

	if(sd == -1){
		std::cerr << "[socker]: creación socket\n";
		return -1;
	}

	bind(sd, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	while(true){
		char buffer[80];
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		
		struct sockaddr cliente;
		socklen_t 	clienteLen = sizeof(struct sockaddr);

		int bytes = recvfrom(sd, (void*) buffer, 80, 0,
			 &cliente, &clienteLen);

		if(bytes == -1){
			return -1;
		}

		getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, 
			NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
		std::cout << "Host: " << host << " Port: " << serv << std::endl;
		std::cout << "\tData: " << buffer << std::endl;
	
		sendto(sd, buffer, bytes, 0, &cliente, clienteLen);
	}
			
	close(sd);
	return 0;
}


