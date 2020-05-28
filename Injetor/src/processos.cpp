

#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

#include "processos.h"
#include "errors.h"

using namespace std;

bool getProcesso(HANDLE &ProcHandle, TCHAR * ProcAtualDir, char * proc_nome, char * proc_dir){
	
	
	PROCESSENTRY32 processo_struct;
	char * diretorio_do_processo = new char[MAX_PATH];
		
		processo_struct.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(
			TH32CS_SNAPPROCESS | TH32CS_SNAPMODULE,
			0
		);

		//primeiro processo é sempre [system], logo é irrelevante
		if (	Process32First(hSnapshot, &processo_struct)){
			
			
			
			while( Process32Next(hSnapshot, &processo_struct) ){
				
				//evita injetar dll em si mesmo
				if (	strcmp(ProcAtualDir, proc_dir) == 0	) continue;
				
				
				//compara o nome do processo na lista de processos com o processo alvo
				if (
					strcmp(proc_nome, processo_struct.szExeFile) == 0
				){
					
					
					
					
					//processo com o mesmo nome
					HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processo_struct.th32ProcessID);
					if(pHandle){
						
											
						//verifica diretório
						if( 
							!GetModuleFileNameExA(
								pHandle, 
								NULL,
								diretorio_do_processo,
								MAX_PATH
							) 
								
						){
							cout << "Erro ao tentar validar processo." << endl;
							cerr_lastError();
						}

						
						if ( strcmp(proc_dir, diretorio_do_processo) != 0 ){
							//verifica diretorio, caso não seja igual ao especificado, valida o proximo processo	
							continue;
							
						}
						
						
					
						ProcHandle = pHandle;
						CloseHandle(hSnapshot);

						delete(diretorio_do_processo);
						return true;

					}else{
						//erro ao tentar obter o handle do processo
						cerr << "Erro ao tentar abrir o processo '" << processo_struct.szExeFile << "'." << endl;
						cerr_lastError();
						continue;
					}

	
					}

			}


			}

		
		CloseHandle(hSnapshot);
		delete(diretorio_do_processo);
		return false;
}