
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <string>
#include <Shlobj.h>

#include "errors.h"
#include "processos.h"
#include "memoria.h"
#include "test.h"

using namespace std;


int main(int argc, char * argv[]){;
	
	cout << "Verificando privilégios..." << endl;
	
	if (!IsUserAnAdmin()){
		
		cerr << "Erro: O programa requer privilégios administrativos." << endl;
		return 1;
		
	}
	
	char * ProcessoNome;
	char * ProcessoDir;
	
	//TODO: buffer disso aqui deve ser deletado depois
	
	if(argc > 1){
	
		string diretorio = argv[1];
		//vo ve e te splito
		string arqNome = diretorio.substr(
			diretorio.find_last_of("/\\")+1
		);
		
		
		char * arqNome_c = new char [arqNome.length()+1];
		
		
		//transforma string em string C
		strcpy(arqNome_c, arqNome.c_str());
		ProcessoNome = arqNome_c;
		
		
		ProcessoDir = argv[1];
		
		
	}else{
		
		//arg não especificado, utiliza os padrões
		ProcessoNome = new char [sizeof(NOME_DO_PROCESSO)];
		ProcessoDir = new char [sizeof(DIRETORIO_DO_PROCESSO)];
		
		strcpy(ProcessoNome, NOME_DO_PROCESSO);
		strcpy(ProcessoDir, DIRETORIO_DO_PROCESSO);
		
		
		
	}
	
	
	TCHAR DiretorioDoProcessoAtual[MAX_PATH];
	
	if (
		!GetModuleFileName (
			NULL,
			DiretorioDoProcessoAtual,
			MAX_PATH
		)
	
	){
		
		cerr_lastError();
		return 1;
	}

	
	//Handle do processo alvo
	HANDLE ProcHandle;
	
	cout << "Esperando pelo processo " << ProcessoNome << "..."<< endl;
	
	while ( !getProcesso(ProcHandle, DiretorioDoProcessoAtual, ProcessoNome, ProcessoDir) ){
		Sleep(1000);
		
	}
	

	cout << "Processo detectado !" << endl;
	
	//processo encontrado, não há necessidade de manter os buffers já usados
	if(argc < 2) delete(ProcessoDir);
	delete(ProcessoNome);

	
	HMODULE Kernel32Addr = GetModuleHandle("kernel32.dll");
	LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(Kernel32Addr, "LoadLibraryA");

	cout << "LoadLibraryA Addr: "<< LoadLibraryAddr << endl;

	const char dll_dir[] = DLL_DIR_STR;
	const int MemoriaRemotaSize = sizeof(dll_dir);

		
	
	LPVOID MemoriaRemota = VirtualAllocEx(
		ProcHandle,
		NULL,
		MemoriaRemotaSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE

	);


	if (MemoriaRemota){
		cout << "Memória alocada: " << MemoriaRemota << endl;

		cout
		<< "Tamanho total da memória alocada: " << MemoriaRemotaSize << " bytes" << endl
		<< "Escrevendo na memória alocada: " << endl << endl;

		unsigned int VerificarNovaLinha = 32;
		for (unsigned int i=0; i < sizeof(dll_dir); i++){

			if (i == VerificarNovaLinha ){
				VerificarNovaLinha += 32;
				cout << endl;
			}

			cout << dll_dir[i] << " ";
		}

		cout << endl << flush;


		

		//DLL_DIR_STR
		bool escreveu_na_mem_str_dir = WriteProcessMemory(
			ProcHandle,
			MemoriaRemota,
			dll_dir,
			sizeof(dll_dir),
			NULL
		);



		
		if(escreveu_na_mem_str_dir){
			cout << endl << "Memória Escrita com sucesso !" << endl;


			//-------------------------------------------------------------------------------
			cout << "Compartilhando uma área na memória para referenciar os ponteiros..." << endl;

			HANDLE hArquivoMapeado;
			DWORD * SharedMemory;

			if (!CompartilharMemoria(hArquivoMapeado, SharedMemory)	){
				CloseHandle(ProcHandle);
				return 1;
			}

			cout << "Área na memória compartilhada com sucesso." << endl;
			//-------------------------------------------------------------------------------



			cout << "Criando thread..." << endl;
			
				
			
			HANDLE tHandle = CreateRemoteThread(
				ProcHandle,
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)LoadLibraryAddr,

				//diretorio da dll aqui
				(LPVOID)MemoriaRemota,
				0,
				NULL
			);

			if(tHandle){

				
				cout << "Thread Iniciado !" << endl;

				//THREAD FIM ?
				WaitForSingleObject(
				  tHandle,
				  INFINITE
				);
				
				DWORD tExitCode = 0;
				
				
				GetExitCodeThread(
						tHandle,
						&tExitCode
				);
				
				cout << "Thread remoto terminou." << endl;
				
				//bloqueia até receber uma flag diferente
				while(	SINAL_DE_COMUNICACAO != SINAL_INJETADO_SUCESSO	)
					Sleep(DELAY_VERIFICAR_FLAG);
				
				cout << "DLL injetada com sucesso !" << endl;
				
				
				
				SINAL_DE_COMUNICACAO = SINAL_ESPERANDO_PONTEIROS;
				
				cout << "Esperando o thread enviar os endereços dos ponteiros..." << endl;
				
				while(SINAL_DE_COMUNICACAO == SINAL_ESPERANDO_PONTEIROS)
					Sleep(DELAY_VERIFICAR_FLAG);
				
				cout << "Endereços dos ponteiros referenciadores recebidos !" << endl;

				unsigned int vnl = 4;
				for (unsigned int i=0; i < PONTEIROS_REF_COUNT -1 ; i++){

					if (i == vnl){
						cout << endl;
						vnl += 4;
					}

					ArrPonteirosRef[i] = *(SHARED_MEMORY_SEM_FLAG + i);
					cout << hex << setw(sizeof(DWORD) * 2) << setfill('0') << *(SHARED_MEMORY_SEM_FLAG + i) << "\t";

				}

				
				
				//ArrPonteirosRef agora contém todos os ponteiros que a dll passou



				//lê memoria remota				
				void * Buffer;
				unsigned int valor_hookado = 0;
				
				
				
				while(true){
					
					ReadRemoteMemory(ProcHandle, (LPVOID)ArrPonteirosRef[0], &Buffer, sizeof(Buffer) );
					
					//buffer contem o endereço que aponta para o valor hookado
					//ainda não foi hookado
					if(!Buffer) continue;
					
					cout << "\n" <<"Bytes no endereço " << (LPVOID)ArrPonteirosRef[0] << ":" << endl;
					//PrintBufferBytes(Buffer, sizeof(Buffer));
					
					
					ReadRemoteMemory(ProcHandle, Buffer, &valor_hookado, sizeof(int *) );
					cout << Buffer << "->" << valor_hookado <<
					endl;
					
					Sleep(500);
				}
				
				cout << endl;
				
				/*
				unsigned char BufferComparar[] = {0xc9, 0xc3, 0x55, 0x89};
				
				
				cout << "Assert: " << AssertBytes(
					Buffer,
					BufferComparar,
					sizeof(Buffer),
					sizeof(BufferComparar)
				) << endl;
				*/
				
				
				//THREAD FIM ?
				WaitForSingleObject(
				  tHandle,
				  INFINITE
				);
				
				cout << "Thread remoto terminou." << endl;
				
				//fecha handle MapViewOfFile
				CloseHandle(SharedMemory);

				//fecha handle CreateFileMappingA
				CloseHandle(hArquivoMapeado);

				//fecha o handle do thread que já terminou
				CloseHandle(tHandle);
			}


			//TODO: FreeLibrary na outra memória aqui
		}else{
			cerr_lastError();
			
		}



	}else{

		cerr_lastError();
		
	}

	CloseHandle(ProcHandle);
	return 0;
}