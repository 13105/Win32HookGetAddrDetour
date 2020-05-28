#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <iomanip>

#include "memoria.h"
#include "errors.h"


using namespace std;


	//DEFINIÇÃOES PARA COMPARTILHAR O BUFFER
	
	LPDWORD buffPonteirosRef;
	TCHAR SHARED_MEMORY_NAME[] = TEXT("Global\\PONTEIROS_REF");
	



bool AssertBytes(
	unsigned char * Buffer,
	unsigned char * Buffer_2,
	unsigned long buffer_size,
	unsigned long buffer_2_size
 ){
	 
	//diferente se o comprimento do buffer for diferente
	if (buffer_size != buffer_2_size) return false;
	
	//comprimento do buffer igual
	
	//verdadeira comparação aqui
	return (
		memcmp(Buffer, Buffer_2, buffer_size) == 0
	);
	
}	
	
void PrintBufferBytes( unsigned char * Buffer, unsigned long buffer_size){
	
			unsigned long vnl = 15;

			cout << "\t";
			
			for (unsigned long i=0; i < buffer_size; i++){
				
				if (i==vnl){
						//nova linha
						cout << endl << "\t";
						vnl += 15;
				}
					cout << hex << setw(2) << setfill('0') << (unsigned int)Buffer[i] << " ";
					
			}
			
			cout << endl;
				
}	
	
	
bool CompartilharMemoria( HANDLE &hArquivoMapeado, LPDWORD &SharedMemory ){

	//Mapeia memoria para a dll acessar e receber os ponteiros


				const unsigned int SHARED_MEMORY_SIZE = PONTEIROS_REF_COUNT * sizeof(DWORD);


				hArquivoMapeado = CreateFileMappingA(
					  INVALID_HANDLE_VALUE,
					  NULL,
					  PAGE_READWRITE,
					  0,
					  SHARED_MEMORY_SIZE,
					  SHARED_MEMORY_NAME
				);



				if (!hArquivoMapeado){
					cerr << "Erro ao tentar compartilhar memória na área " << SHARED_MEMORY_NAME << endl;
					cerr_lastError();
					return false;
					//error
				}


				SharedMemory = (DWORD *)MapViewOfFile(
					hArquivoMapeado,
					FILE_MAP_ALL_ACCESS,
					0,
					0,
					SHARED_MEMORY_SIZE

				);


				if (!SharedMemory){
					cerr << "Erro: Não foi possível criar o objeto de compartilhamento de memória." << endl;
					cerr_lastError();

					CloseHandle(hArquivoMapeado);
					return false;
					//error
				}


				// memoria compartilhada ok
				return true;

}


bool ReadRemoteMemory(
	 HANDLE &ProcHandle,
	 void * RemoteAddr,
	 void * BufferOut,
	 unsigned int BufferOut_size 
 ){
	
	

	
	if (!ReadProcessMemory(
			ProcHandle,
			RemoteAddr,
			BufferOut,
			BufferOut_size,
			NULL
	)){
		
			cerr_lastError();
			return false;
	}


	

	//TODO: PROCURAR UMA LIB PARA DISSASSEMBLAR ISSO AQUI
	//exibe byte por byte
	
	
	
	
	return true;
}