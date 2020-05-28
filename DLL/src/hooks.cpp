#include "hooks/hook_get_addr.h"


#include "hooks.h"
#include <iostream>
#include <windows.h>



using namespace std;



bool DeleteHook(
	HOOK_32 * hook_src
){
	if(!hook_src)return false;
	
	delete(hook_src->hook_buffer);
	delete(hook_src);
	return true;
	
}


/*
formato: 
	{
		{endereço,	valor}	
							}

*/
HOOK_32 * PatchHookTemplate(
	unsigned char * hook_teamplate,
	unsigned int template_len,
	Patch_Arg * patch_args,
	unsigned int argumentos_count
){ 
	
	
			
			
			//copia array template
			unsigned char * hook_buffer = new unsigned char(template_len);
			CopyMemory(hook_buffer, hook_teamplate, template_len);
				
			
			//substitui valores aqui
			
			uintptr_t hook_buffer_uintptr = (uintptr_t)hook_buffer;
			
			for (unsigned int i=0; i < argumentos_count; i++){
				
				//offset maior que o limite do buffer ? erro
				if (patch_args[i].ByteOffset > template_len-1) {
					delete(hook_buffer);
					return NULL;
				
				}
					
				CopyMemory(
					(LPVOID)(hook_buffer_uintptr + (patch_args[i].ByteOffset)	),
					&patch_args[i].val,
					sizeof(void *)
				);
		
				
				
			
			}
		
			//endereço para salvar
			
			HOOK_32 * hook_ret;
			
			hook_ret = new HOOK_32;
	
			hook_ret->hook_buffer = hook_buffer;
			hook_ret->tamanho_do_buffer = template_len; 
			
			//sucesso !
			return hook_ret;
	
	
}
HOOK_32 * CreateHook(
	const unsigned int hook_template_id, 
	void ** arr_patch_argumentos, 
	unsigned int argumentos_count
){
	
	
	
	switch(hook_template_id){
		
		case TEMPLATE_HOOK_GET_ADDR:
		
			
			if(argumentos_count != 1) return NULL;
			
			
			
			Patch_Arg patch_args[] = {
				{6, NULL}
			};
			
			for(unsigned int i=0; i < argumentos_count; i++){
				patch_args[i].val = arr_patch_argumentos[i];
			}
			
			return PatchHookTemplate(
						hook_get_addr,
						hook_get_addr_len,
						patch_args,
						argumentos_count
					);
				
			 
			
			
			
			
			
		break;
		
	}
	
	return NULL;
}

bool DesanexarHook(
	
	HOOK_32_ANEXADO * hook_anexado
	
){
	
	if(!hook_anexado) return false;

	cout
	<< "BytesRoubados " << hook_anexado->BytesRoubados << endl
	<< "BufferCodigoSize "<<hook_anexado->BufferCodigoSize << endl
	<< "AddrOndeFoiInserido "<<hex << hook_anexado->AddrOndeFoiInserido << endl
	<< "BufferCodigo "<<hex << hook_anexado->BufferCodigo << endl
	<< "BytesRoubadosSize "<<hook_anexado->BytesRoubadosSize << endl;
	
	
	//modifica as permissões no jmp para o buffer
	DWORD AddrOndeFoiInserido_velha_protecao;
	
	VirtualProtect(
				hook_anexado->AddrOndeFoiInserido,
				hook_anexado->BytesRoubadosSize,
				PAGE_EXECUTE_READWRITE,
				&AddrOndeFoiInserido_velha_protecao
	);
	
		
		
	
	//afirma se os bytes no endereço do jmp para o hook foram mesmo modificados
	if(
		memcmp(
			hook_anexado->AddrOndeFoiInserido,
			hook_anexado->BytesRoubados,
			hook_anexado->BytesRoubadosSize
		) == 0
	
	) return false;
	
	
	//bytes são diferentes, hook anexado ok
	
	
	//copia os bytes roubados de volta para o endereço de origem
	CopyMemory(
		hook_anexado->AddrOndeFoiInserido,
		hook_anexado->BytesRoubados,
		hook_anexado->BytesRoubadosSize
	);
	
	
	//libera a memória do buffer de código
	VirtualFree(
			hook_anexado->BufferCodigo,
			0,
			MEM_RELEASE
	);
	
	//deleta a estrutura do hook
	delete(hook_anexado);
	
	
	//restaura as permisões
	VirtualProtect(
				hook_anexado->AddrOndeFoiInserido,
				hook_anexado->BytesRoubadosSize,
				AddrOndeFoiInserido_velha_protecao,
				&AddrOndeFoiInserido_velha_protecao
	);
	
	
	
	return true;
}

HOOK_32_ANEXADO * AnexarHook(

	HOOK_32 * hook_src,
	void * AddrOndeHookar,
	unsigned int BytesRoubarSize
	
){
	/*
	
					----------------
						TEMPLATE
					----------------
					  Stolen Bytes
					----------------
		JMP(E9) AddrOndeHookar + Bytes Roubados
		
	
	*/
	
	// quantidade de bytes que serão roubados
	// acima de 5(tamanho da instrução jmp) serão substituidos por NOP's
	if (BytesRoubarSize < 5) return NULL;
	
	DWORD AddrOndeHookar_velha_protecao;
	
	if(
		//concede permissões de escrita, execução e leitura ao local onde será inserido o jmp para o BufferCodigo
		!VirtualProtect(
			AddrOndeHookar,
			BytesRoubarSize,
			PAGE_EXECUTE_READWRITE,
			&AddrOndeHookar_velha_protecao
		)
		
	)
		//erro ao tentar conceder permissão
		return NULL;
		
	
		
		
		
	//buffer para colocar todo o código que será executado
	
	const unsigned int BufferCodigoSize = 
		hook_src->tamanho_do_buffer + BytesRoubarSize + 5;
		//5 bytes reservados para jump de retorno
		
		
	
	
	void * BufferCodigo = VirtualAlloc(
		NULL,
		BufferCodigoSize,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
	);
	
	if (!BufferCodigo) return NULL;
	
	
	// copia o template para o buffer
	
	CopyMemory(
		BufferCodigo,
		hook_src->hook_buffer,
		hook_src->tamanho_do_buffer
	);
	
	
	

	//-------- 		BYTES ROUBADOS 		----------
	//lê e copia os bytes que serão sequestrados no endereço onde hookar
	
	
	//inicio dos bytes roubados
	void * offset_bytes_roubados = 
		(void *) (
			(intptr_t)(BufferCodigo) + hook_src->tamanho_do_buffer
		);
	
	
	
	//inicio do jmp, fim do offset "bytes roubados"
	void * offset_jmp_retorno = 
		(void *) (
			(intptr_t)(offset_bytes_roubados) + BytesRoubarSize
		);
		

	
	
	unsigned char * BufferCodigo_Conteudo = 
		((unsigned char *)BufferCodigo);
	
	

	CopyMemory(
		
		//após offset template
		offset_bytes_roubados,
		AddrOndeHookar,
		BytesRoubarSize
		
	);
	
	
	

	//-------- JMP RETORNA A FUNÇÃO ORIGINAL ---------
	
	
	// E9 = JMP OPCODE
	* (unsigned char *) offset_jmp_retorno = 0xE9;
	
	//endereço relativo do jump
 
	

	
	
	
	//(dst - src) -5 bytes do jump porque o pulo é relacionado com a ultima instrução
	//		+ BytesRoubarSize
	
	const intptr_t jmp_retorno_addr = 
		 (intptr_t)AddrOndeHookar - (intptr_t)offset_jmp_retorno
		 -5 + BytesRoubarSize;
		

	
	CopyMemory(
		(void *)(intptr_t)(	(intptr_t)offset_jmp_retorno + 1),
		&jmp_retorno_addr,
		sizeof(void *)
	);
	
	
	
	
	
	
	
	
	
	// modifica a proteção do endereço onde será inserido o jmp para o BufferCodigo
	
	
	
	
	
	// dados para o jump, 5 bytes
	
	unsigned char jmp_to_hook_bytes[5];
	//opcode
	jmp_to_hook_bytes[0] = 0xE9;
	
	
	//src = AddrOndeHookar, dst = BufferCodigo
	// endereço relativo
	const intptr_t jmp_to_hook_addr =
		(intptr_t)BufferCodigo - (intptr_t)AddrOndeHookar - 5;
	
	//endereço
	CopyMemory(
		(LPVOID)((intptr_t)jmp_to_hook_bytes + 1),
		&jmp_to_hook_addr,
		sizeof(void *)
	);
	
	
	
	
	//desvia o código para o hook
	CopyMemory(
		AddrOndeHookar,
		jmp_to_hook_bytes,
		BytesRoubarSize
	);
	
	
	//preenche o resto dos bytes com NOP's
	if(BytesRoubarSize > 5)
		memset(
			// +5 dos bytes do jump
			(void *)(	(intptr_t)AddrOndeHookar + 5) ,
			0x90,
			BytesRoubarSize - 5
			
		);	
	
	
	//recupera a velha proteção na memória onde foi inserido o hook 
	
	VirtualProtect(
			AddrOndeHookar,
			BytesRoubarSize,
			AddrOndeHookar_velha_protecao,
			&AddrOndeHookar_velha_protecao
	);
	
	
	
	
	//verifica se escreveu
	if(
		memcmp(
			AddrOndeHookar,
			jmp_to_hook_bytes,
			5
		) != 0
	){
		//não escreveu, erro
		
		//libera memória do buffer antes de falhar
		VirtualFree(
			BufferCodigo,
			0,
			MEM_RELEASE
		);
		
		return NULL;
		
	}
	
	HOOK_32_ANEXADO * hook_anexado = new HOOK_32_ANEXADO;
		
		hook_anexado->BufferCodigo = BufferCodigo;
		hook_anexado->BufferCodigoSize = BufferCodigoSize;
		hook_anexado->AddrOndeFoiInserido = AddrOndeHookar;
		hook_anexado->BytesRoubadosSize = BytesRoubarSize;
		hook_anexado->BytesRoubados = offset_bytes_roubados;
		
	
	//sucesso !
	return hook_anexado;
}