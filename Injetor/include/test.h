#ifndef MAIN_H
#define MAIN_H



#define DLL_DIR_STR "C:\\Users\\13105\\Documents\\DEV\\C++\\DLL Injection com LoadLibrary\\DLL\\build\\saida.dll"
#define NOME_DO_PROCESSO "a.exe"
#define DIRETORIO_DO_PROCESSO "C:\\Users\\13105\\Documents\\DEV\\C++\\DLL Injection com LoadLibrary\\Processo Alvo\\build\\a.exe"

#define SINAL_INJETADO_SUCESSO 0x01
#define SINAL_ESPERANDO_PONTEIROS 0x02

//SINAIS

#define SINAL_DE_COMUNICACAO SharedMemory[0]
#define SHARED_MEMORY_SEM_FLAG (SharedMemory + 1)
#define	DELAY_VERIFICAR_FLAG 100

//Array para guardar os ponteiros que serão usados como argumento ao ler memoria remota
	DWORD ArrPonteirosRef[PONTEIROS_REF_COUNT];

#endif