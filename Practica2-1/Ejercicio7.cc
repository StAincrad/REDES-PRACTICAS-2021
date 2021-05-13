#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

#include <thread>

//MUTEX Y CONDITION SIRVEN PARA EVITAR DDoS
#include <mutex>
#include <condition_variable>

#define MAX_CLIENTS 3

std::condition_variable num_cv;
std::mutex num_mutex;
int num_clientes = 0;

class MsgThread{
private:
	int sd;
	int id;
public:
	MsgThread(int sd_, int i_) : sd(sd_), id(i_) {}

	//GESTIÓN DEL THREAD CONECTADO
	void haz_conexion(){
		bool active = true;

		while(active){
			char buffer[80];
	
			int bytes = recv(sd, (void *) buffer, 80, '\0');
			buffer[bytes] = '\0';
	
			if(bytes <= 0){
				std::cout << "Conexión terminada\n";
				active = false;
				break;
			}	
			
			std::cout << "\tTHREAD ID: " << id << "\n";
			std::cout << "\tData: " << buffer << "\n";
	
			send(sd, (void *) buffer, bytes, '\0');
		}
		
		//Cierre de socket
		close(sd);

		num_mutex.lock();
		num_clientes--;
		std::cout << "NUM CLIENTES: " << num_clientes << "\n";
		//Se notifica a la variable de condicion
		if(num_clientes < MAX_CLIENTS){
			num_cv.notify_all();
		}
		num_mutex.unlock();
	}
};

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
	int l = listen(sd, MAX_CLIENTS);
	if(l == -1){
		std::cerr << "Fallo al poner en escucha [listen]\n";
		return -1;
	}

	//-----------------------------------------------------------------//
	//	GESTIÓN DE LAS CONEXIONES ENTRANTES			   //
	//-----------------------------------------------------------------//
	int id = 0;
	while(true){	
		struct sockaddr client;
		socklen_t clientLen = sizeof(struct sockaddr);
		
		//Variables de condicion para no aceptar mas clientes
		{
		    std::unique_lock<std::mutex> lck(num_mutex);
		    while(num_clientes >= MAX_CLIENTS){
			   std::cout << "SERVER LLENO...\n";
			   num_cv.wait(lck);
		    }
		}

		int sdClient = accept(sd, (struct sockaddr *)&client, &clientLen);
		
		if(sdClient == -1){
			std::cerr <<
		 "Imposible aceptar la conexión TCP con el cliente\n";
		}
		
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
	
		getnameinfo((struct sockaddr *) &client, clientLen, host, NI_MAXHOST,
			serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
		std::cout << "Conexión desde " << host << ":" << serv << std::endl;
		
		num_mutex.lock();
		num_clientes++;

		std::cout << "CLIENTES: " << num_clientes << std::endl;
		num_mutex.unlock();		
		
		MsgThread * th = new MsgThread(sdClient, num_clientes);
		std::thread([&th](){
			th->haz_conexion();
			delete th;
		}).detach();
	}

	//Cierre de socket
	close(sd);

	return 0;
}


