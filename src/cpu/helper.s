global gdt_flush
gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov fs, ax
    jmp 0x08:._gdt_flush

._gdt_flush:
    ret

global tss_flush
tss_flush:
    mov ax, 0x2B
    ltr ax
    ret

global idt_flush
idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

global copy_page_physical
copy_page_physical:
    push ebx

    pushf

    cli

    mov ebx, [esp + 12]
    mov ecx, [esp + 16]

    mov edx, cr0
    and edx, 0x7fffffff
    mov cr0, edx

    mov edx, 1024

.loop:
    mov eax, [ebx]
    mov [ecx], eax
    add ebx, 4
    add ecx, 4
    dec edx
    jnz .loop

    mov edx, cr0
    or edx, 0x80000000
    mov cr0, edx

    popf

    pop ebx
    ret

global read_eip
read_eip:
    pop eax
    jmp eax


global perform_thread_switch
perform_thread_switch:
     cli;
     mov ecx, [esp+4]   ; EIP
     mov eax, [esp+8]   ; physical address of current directory
     mov ebp, [esp+12]  ; EBP
     mov esp, [esp+16]  ; ESP
     mov cr3, eax       ; set the page directory
     mov eax, 0x12345   ; magic number to detect a task switch
     sti;
     jmp ecx