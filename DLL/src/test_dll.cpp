#include <windows.h>
#include <tchar.h>
#include <iostream>

#include "test_dll.h"
#include "teclas.h"
#include "hooks.h"
	using namespace std;
	
	HANDLE hArquivoMapeado;
	DWORD * SharedMemory;
	
	
	
	void sinalizar(){
		
		MessageBoxW(
				0,
				L"!",
				L"SUCESSO !.",
				MB_OK
			);
			
	}
	
	void * addr_hookado;
	
	
	BOOL WINAPI DllMain(	HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved	){

		MessageBoxW(
						0,
						L"!",
						L"SUCESSO !.",
						MB_OK
				);
				
		if(fdwReason == DLL_PROCESS_ATTACH){
			//attach
			
			
			
			hArquivoMapeado = OpenFileMapping(
				FILE_MAP_ALL_ACCESS,
				FALSE,
				SHARED_MEMORY_NAME
			);
			
			
			
			if (!hArquivoMapeado){					
					MessageBoxW(
						0,
						L"Erro ao tentar acessar a região de compartilhamento de memória.",
						L"Error",
						MB_OK | MB_ICONERROR
					);
					
					
					return 1;
			}
				
			
			
			
			SharedMemory = (DWORD *)MapViewOfFile(
					hArquivoMapeado,
					FILE_MAP_ALL_ACCESS,
					0,
					0,
					sizeof(ArrPonteirosRef)
				);	
				
			
			if (!SharedMemory){
					MessageBoxW(
						0,
						L"Erro ao tentar acessar a região de compartilhamento de memória.",
						L"Error",
						MB_OK | MB_ICONERROR
					);
					
					CloseHandle(hArquivoMapeado);
					return 1;
			}
			
			
			
			for (int j = 0; j < 3; j++)
				Sleep(DELAY_VERIFICAR_FLAG);
			
			
			
			//Primeiro envia um sinal booleano que a injeção foi efetuada com sucesso
			SINAL_DE_COMUNICACAO = SINAL_INJETADO_SUCESSO;
			
			//bloqueia até receber o sinal para enviar enviar ponteiros			
			while (SINAL_DE_COMUNICACAO != SINAL_ESPERANDO_PONTEIROS)
				Sleep(DELAY_VERIFICAR_FLAG);
			
			
			
			//----------------------------------------------------------------------
			// insere hooks nos endereços especificados na array
			
			//TODO: CONSEGUIR LER O PONTEIRO PELO PROCESSO INJETOR
			
			unsigned char buffer_addr_test[4];
			
			
			void * arr_arg[] = {
				&addr_hookado
			};
			
			
			
			HOOK_32 * HookGetAddr = CreateHook(
			
				TEMPLATE_HOOK_GET_ADDR,
				arr_arg,
				size(arr_arg)
			
			);
			
			
			
			if(!HookGetAddr){
				
				
				MessageBoxW(
						0,
						L"Error",
						L"Falha ao tentar criar hook.",
						MB_OK
				);
			
			}
			
			
			//só injetar
			
			
			
			
			
			
			
			HOOK_32_ANEXADO * hookAnexado = AnexarHook(
				HookGetAddr,
				(LPVOID)0x005B33E9,
				10
			);
			
			if(!hookAnexado){
				
				MessageBoxW(
						0,
						L"Falha ao tentar anexar hook.",
						L"Error",
						MB_OK
				);
				
			
			}
			
			
			//		NO FIM ISSO AQUI DEVE SER EXECUTADO
			//-----------------------------------
			//cout << valor_hookado << endl;
			//DeleteHook(HookGetAddr);
			//----------------------------------
			
			
			
			ArrPonteirosRef[0] = (DWORD)&addr_hookado;
			
		
			
			
			
			
			
			
			
			
			//deleta hook e libera memoria
			//DesanexarHook(hookAnexado);
			
			
			
			
			//-----------------------------------------------------------------------
			//Envia endereços para o injetor daqui
			
			
			SINAL_DE_COMUNICACAO = SINAL_PONTEIROS_ENVIADOS;
			
			
			
			
			CopyMemory(
				SHARED_MEMORY_SEM_FLAG,
				ArrPonteirosRef, 
				sizeof(ArrPonteirosRef)
			 );
			 
			
			
			
			MessageBoxW(
						0,
						L"Injetado !",
						L"Sucesso",
						MB_OK
			);
			
			
			
			return 0;
			//Sucesso !!!
			
		}else{
			
			
		
		
		//Depois da injeção executa isso aqui
		
		
		
		
		
		
		MainLoopRodando = true;
			while(MainLoopRodando){
				//processa teclas
				ProcessarTeclas();
				
				//bloqueia thread por alguns segundos
				
				
				Sleep(50);
				
			}
			
		
		//------------------- DETACH ------------------------
		//fecha todos os handles abertos aqui
		if (SharedMemory){
			UnmapViewOfFile(
					SharedMemory
			);
		}
			
			
		if (hArquivoMapeado)
			CloseHandle(hArquivoMapeado);
			
		
			
		

		//detach
		
		
		return 0;
	}
}