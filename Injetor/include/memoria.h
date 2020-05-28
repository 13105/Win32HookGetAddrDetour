#ifndef MEMORIA_H
#define MEMORIA_H
	
	//1 Reservado para flag
	#define PONTEIROS_REF_COUNT 1 + 8
	
	
	//lê memoria remota e escreve o resultado em um buffer
	bool ReadRemoteMemory(
	 HANDLE &ProcHandle,
	 void * RemoteAddr,
	 void * BufferOut,
	 unsigned int BufferOut_size 
 );
	
	
	
	//Compartilha um buffer para comunicar-se com a DLL no processo alvo
	bool CompartilharMemoria(

		HANDLE &hArquivoMapeado,
		LPDWORD &SharedMemory 
		
	);
	
	//printa todos os bytes contidos num buffer
	void PrintBufferBytes( unsigned char Buffer[], unsigned long buffer_size);
	
	
	//compara os bytes de dois buffers e retorna se ambos são iguais ou não
	bool AssertBytes(
		unsigned char * Buffer,
		unsigned char * Buffer_2,
		unsigned long buffer_size,
		unsigned long buffer_2_size
	);
	
	
#endif