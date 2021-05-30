#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME 20

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    //Serialización
    void to_bin()
    {
	//Tamaño de la clase (name + x + y)
        int32_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
	alloc_data(data_size);
	char * tmp = _data;
	
	//Serialización del nombre
	memcpy(tmp, name, MAX_NAME * sizeof(char));
	tmp += MAX_NAME * sizeof(char);
	
	//Serialización de x
	memcpy(tmp, &x, sizeof(int16_t));
	tmp += sizeof(int16_t);

	//Serialización de y
	memcpy(tmp, &y, sizeof(int16_t)); 
    }

    //Deserialización
    int from_bin(char * data)
    {
        char* tmp = data;
	//Des del nombre
	memcpy(name, tmp, MAX_NAME * sizeof(char));
	if(name < 0) return -1;
	tmp += MAX_NAME * sizeof(char);
	
	//Des de x
	memcpy(&x, tmp, sizeof(int16_t));
	if(&x < 0) return -1;
	tmp += sizeof(int16_t);
	
	//Des de y
	memcpy(&y, tmp, sizeof(int16_t));
	if(&y < 0) return -1; 	

        return 0;
    }


public:
    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    	Jugador one_r("", 0, 0);
    	Jugador one_w("Player_ONE", 123, 987);

   	 // 1. Serializar el objeto one_w
    	one_w.to_bin();
	// 2. Escribir la serialización en un fichero
	int fd = open("./jugador.txt", O_CREAT | O_TRUNC | O_RDWR);
	
	if(fd == -1){
		std::cout << "Ha ocurrido un error al abrir el archivo jugador.txt\n";
		return -1;
	}
	
	ssize_t size = write(fd, one_w.data(), one_w.size());
	if(size == -1){
		std::cout << "Ha ocurrido un error al escribir en el archivo jugador.txt\n";
		return -1;
	}
	close(fd);
	
	// 3. Leer el fichero
   	fd = open("./jugador.txt", O_RDONLY);
	char buffer[size];
	int r = read(fd, buffer, size);	
	if(r == -1){
		std::cout << "Error al leer el buffer\n";
		return -1;
	}
	close(fd);
	// 4. "Deserializar" en one_r
    	int des = one_r.from_bin(buffer);
	if(des < 0){
		std::cout << "Fallo en deserialización\n";
		return des;
	}

	// 5. Mostrar el contenido de one_r
	std::cout << "NOMBRE: " << one_r.name << "\nx: " << one_r.x <<
		"\ny: " << one_r.y << std::endl;

    return 0;
}

