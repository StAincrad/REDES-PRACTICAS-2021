#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){

	if(argc != 4){
		std::cerr << "Usage: ./Ejercicio3 <IP> <puerto> <command>\n";
		return -1;
	}
	
	struct addrinfo hints;
	struct addrinfo *res;
	
	
	memset((void*) &hints, '\0', sizeof(struct addrinfo));
	
	//IPv4 / UDP
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

	freeaddrinfo(res);
	
	char buffer[80];
	
	//Envío del mensaje
	int bytes = sendto(sd, argv[3], strlen(argv[3]) + 1, '\0',
				 res->ai_addr, res->ai_addrlen);
	if(bytes == -1){
		std::cerr << "Error en el envío de mensajes al servidor\n";
		return bytes;	
	}
	std::cout << "Enviando: " << argv[3] << std::endl;
	
	bytes = recvfrom(sd, buffer, 79, '\0', res->ai_addr, &res->ai_addrlen);
	std::cout << "BYTES: " << bytes << std::endl;
	if(bytes == -1){
		std::cerr << "Error en la recepción del mensaje del servidor\n";
		return bytes;
	}
	
	buffer[bytes] = '\0';
	std::cout << "Mensaje recibido: " << buffer << std::endl;
	
	//Cierre de socket
	close(sd);
	return 0;
}


