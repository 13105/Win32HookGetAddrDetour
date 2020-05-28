#ifndef HOOKS_H
#define HOOKS_H


#define TEMPLATE_HOOK_GET_ADDR 0

struct Patch_Arg{
	
	unsigned int ByteOffset;
	void * val;
	
};
struct HOOK_32_ANEXADO {
	
	// template + bytes roubados + jmp retorno
	void * BufferCodigo;
	unsigned int BufferCodigoSize;
	
	
	//onde onde o jmp para o buffer de código foi inserido
	void * AddrOndeFoiInserido;
	
	//quantidade de bytes que foram roubados
	unsigned int BytesRoubadosSize;
	
	// endereço dos bytes roubados no buffer de código
	void * BytesRoubados;
	
	
	
	
};

struct HOOK_32 {
	
	unsigned char * hook_buffer;
	unsigned int tamanho_do_buffer;
	
};


#define HOOK_32_SIZE sizeof(HOOK_32)
#define HOOK_32_ANEXADO_SIZE sizeof(HOOK_32_ANEXADO)

HOOK_32 * CreateHook(
	const unsigned int hook_template_id, 
	void ** arr_patch_argumentos, 
	unsigned int argumentos_count
);


bool DeleteHook(
	HOOK_32 * hook_src
);

HOOK_32_ANEXADO * AnexarHook(

	HOOK_32 * hook_buffer,
	void * AddrOndeInserirHook,
	unsigned int BytesRoubarSize
	
);

HOOK_32 * PatchHookTemplate(
	unsigned char * hook_teamplate,
	unsigned int template_len,
	Patch_Arg * patch_args,
	unsigned int argumentos_count
);

bool DesanexarHook(
	
	HOOK_32_ANEXADO * hook_anexado
	
);
#endif