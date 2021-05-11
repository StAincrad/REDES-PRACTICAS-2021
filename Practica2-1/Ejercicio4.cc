#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){

	if(argc != 3){
		std::cerr << "Usage: ./Ejercicio4 <IP> <puerto>\n";
		return -1;
	}
	
	struct addrinfo hints;
	struct addrinfo *res;
	
	
	memset((void*) &hints, '\0', sizeof(struct addrinfo));
	
	//IPv4 / TCP
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	
	if(rc != 0)
	{
		std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
		return -1;
	}
	
	//Se abre el socket
	int sd = socket(res->ai_family, res->ai_socktype, '\0');

	if(sd == -1){
		std::cerr << "[socker]: creación socket\n";
		return -1;
	}
	
	//Se establece la unión entre el socket y la dirección ip
	int b = bind(sd, res->ai_addr, res->ai_addrlen);

	if(b == -1){
		std::cerr << "[bind] " << bind << " Imposible realizar la unión\n";
		return -1;
	}
	
	freeaddrinfo(res);
	
	//Se pone el server en modo escucha
	int l = listen(sd, 1);
	if(l == -1){
		std::cerr << "Fallo al poner en escucha [listen]\n";
		return -1;
	}

	//El primer cliente que entre, se acepta
	struct sockaddr client;
	socklen_t clientLen = sizeof(struct sockaddr);
	int sdClient = accept(sd, (struct sockaddr *)&client, &clientLen);
	
	if(sdClient == -1){
		std::cerr << "Imposible aceptar la conexión TCP con el cliente\n";
		return -1;
	}
	
	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];

	getnameinfo((struct sockaddr *) &client, clientLen, host, NI_MAXHOST, serv,
			NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
	std::cout << "Conexión desde " << host << ":" << serv << std::endl;

	bool active = true;

	while(active){
		char buffer[80];

		size_t bytes = recv(sdClient, (void *) buffer, 79, '\0');
		buffer[80] = '\0';
		std::cout << buffer << std::endl;

		if(bytes <= 0){
			std::cout << "Conexión terminada\n";
			active = false;
			continue;
		}	

		send(sdClient, (void *) buffer, bytes, '\0');
	}
	
	//Cierre de socket
	close(sd);

	return 0;
}


