#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){

	if(argc != 3){
		std::cerr << "Usage: ./Ejercicio2 <IP> <puerto>\n";
		return -1;
	}
	
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
	
	bool active = true;
	time_t rawtime;
	size_t tam;
	char buffer[80];
	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];

	while(active){
		
		struct sockaddr cliente;
		socklen_t 	clienteLen = sizeof(struct sockaddr);

		int bytes = recvfrom(sd, (void*) buffer, 80, '\0',
			 &cliente, &clienteLen);
		buffer[bytes] = '\0';

		if(bytes == -1){
			std::cerr << "bytes recibidos erróneos\n";
			return -1;
		}

		getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, 
			NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
		std::cout << "\nHost:Port\n";
		std::cout << host << ":" << serv << std::endl;
		std::cout << "\tData: " << buffer << std::endl;
		
		switch(buffer[0]){
		//Muestra la hora actual
		case 't':
			time(&rawtime);
			tam = strftime(buffer, 79, "%T %p\n", localtime(&rawtime));
			sendto(sd, buffer, tam, '\0', &cliente, clienteLen);
			buffer[tam] = '\0';	
			break;
		//Muestra la fecha actual
		case 'd':
			time(&rawtime);
			tam = strftime(buffer, 79, "%F\n", localtime(&rawtime));
			sendto(sd, buffer, tam, '\0', &cliente, clienteLen);
			buffer[tam] = '\0';
			break;
		//Cierra el servidor
		case 'q':
			std::cout << "Cerrando servidor...\n";
			active = false;
			sendto(sd, "Hasta la vista :D\n", 19, 0, &cliente, clienteLen);
			break;
		default:
			sendto(sd, buffer, bytes, 0, &cliente, clienteLen);
			break;
		}
	}
			
	close(sd);
	return 0;
}


