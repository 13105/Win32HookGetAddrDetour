BITS 32



pushfd
pushad

;------------------------------------------
;TRAP

mov eax, [ebp-0x0C]

;patch para pegar o endereço aqui
mov [0x0], eax

;-----------------------------------------
popad
popfd

;patch chama função para remover hook aqui
call 0x00000000

;trampolim