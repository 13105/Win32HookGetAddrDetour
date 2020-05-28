

#ifndef TEST_DLL_H
#define TEST_DLL_H



#define SINAL_INJETADO_SUCESSO 0x01
#define SINAL_ESPERANDO_PONTEIROS 0x02
#define SINAL_PONTEIROS_ENVIADOS 0x03


#define SINAL_DE_COMUNICACAO SharedMemory[0]

//quantidade de ponteiros para alocar
	//1 reservado para flag
#define PONTEIROS_REF_COUNT 1 + 8
#define SHARED_MEMORY_SEM_FLAG (SharedMemory + 1)
#define	DELAY_VERIFICAR_FLAG 100

TCHAR SHARED_MEMORY_NAME[] = TEXT("Global\\PONTEIROS_REF");

//array de ponteiros

DWORD ArrPonteirosRef[PONTEIROS_REF_COUNT];



extern "C" {
	void sinalizar();
	
	BOOL DllMain(
	
		HINSTANCE hinstDLL,  // handle to DLL module
		DWORD fdwReason,     // reason for calling function
		LPVOID lpReserved 	// reserved
		
	);

	
}



	
#endif
