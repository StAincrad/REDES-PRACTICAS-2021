#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <string>

#include <thread>
#define MAX_THREADS 5
char buff = '\0';

class MsgThread{
private:
	int sd;
	int id;
public:
	MsgThread(int sd_, int id_) : sd(sd_), id(id_){}

	//Realiza los threads correspondientes
	void haz_mensaje(){
		time_t rawtime;
		size_t tam;
		char buffer[80];
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		buffer[0] = '\0';	
		struct sockaddr cliente;
		socklen_t 	clienteLen = sizeof(struct sockaddr);
		
		while(true){
			int bytes = recvfrom(sd, (void*) buffer, 80, '\0',
				 &cliente, &clienteLen);
		
			if(bytes == -1){
				std::cerr << "bytes recibidos erróneos\n";
				return;
			}
	
			for(int i = 0; i < 3; i++){
				std::cout << "SLEEP: " << i + 1 << std::endl;
				sleep(1);	
			}
					
			buffer[bytes] = '\0';
				
			getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, 
				NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
			std::cout << "\nID:Host:Port\n";
			std::cout << id << ":" << host << ":" << serv << std::endl;
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
			default:
				sendto(sd, buffer, bytes, 0, &cliente, clienteLen);
				break;
			}
		}
	}
};

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
		std::cerr << "[socket]: creación socket\n";
		return -1;
	}

	bind(sd, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	for(int i = 0; i < MAX_THREADS; i++){
		MsgThread * th = new MsgThread(sd, i);
		std::thread([&th](){
			th->haz_mensaje();
			delete th;
		}).detach();
	}
	
	std::string q;
	while(q!= "q"){
		std::cin >> q;
	}
	
	std::cout << "Cerando servidor..." << std::endl;
	close(sd);
	return 0;
}


