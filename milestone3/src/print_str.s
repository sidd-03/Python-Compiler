print_str:
    pushq   %rbp           # Save base pointer
    mov     %rsp, %rbp     # Set up new base pointer

    mov     $0, %rax       # Set rax to 0 for printf's return value
    call    printf@PLT     # Call printf to print the string

    popq    %rbp           # Restore base pointer
    ret                     # Return

    