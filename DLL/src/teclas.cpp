#include <windows.h>
#include <iostream>

#include "teclas.h"

using namespace std;

bool MainLoopRodando = false;

void PararMainLoop(){
	
	cout << "Parando mainloop..." << endl;
	MainLoopRodando = false;
	
}

void testar2(){
	
	cout << "ATIVADO 2!" << endl;

}

void testar3(){
	
	cout << "ATIVADO 3!" << endl;

}


const int arr_teclas[] = {
	
	VK_END,
	VK_UP,
	VK_RIGHT
	
};

void (*arr_functions[3])(void) = {
	
	PararMainLoop,
	testar2,
	testar3
	
};

#define arr_teclas_size size(arr_teclas)

short EstadoDaTecla;
void ProcessarTeclas(void){
	
	for(unsigned int i = 0; i < arr_teclas_size; i++){

		
		
		
		EstadoDaTecla = GetAsyncKeyState(	arr_teclas[i]	);
		
		if( 	(EstadoDaTecla & 0x00FF)		)
			
		{
			
			(*arr_functions[i])();
			
			
		}
		
		
		
		
		
		
		
	}
	
	
	
	
}