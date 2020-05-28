BITS 32



pushfd
pushad


mov eax,ecx
;patch para pegar o endereço aqui
mov [0x0], eax

;-----------------------------------------
popad
popfd

; bytes roubados aqui
; jmp retorna ao código original