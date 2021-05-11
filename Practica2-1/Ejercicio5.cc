#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char** argv){

	if(argc != 3){
		std::cerr << "Usage: ./Ejercicio5 <IP> <puerto>\n";
		return -1;
	}
	
	struct addrinfo hints;
	struct addrinfo *res;
	
	
	memset((void*) &hints, '\0', sizeof(struct addrinfo));
	
	//IPv4 / TCP
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

	freeaddrinfo(res);
	
	//Se pone el server en modo escucha
	int server = connect(sd, (struct sockaddr *) res->ai_addr, res->ai_addrlen);
	if(server == -1){
		std::cerr << "Fallo al conectar al servidor [connect]\n";
		return -1;
	}

	bool active = true;	
	char buffer[80];
	
	while(active){
		std::cin >> buffer;
		int bytes = send(server, buffer, 79, '\0');
		
		if(buffer[0] == 'Q'){
			active = false;
			continue;
		}

		recv(server, buffer, 79, '\0');
		std::cout << buffer << std::endl;
	}

	close(sd);
	return 0;
}


