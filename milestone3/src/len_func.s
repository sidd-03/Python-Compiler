len:
    push    %rbp            # Save base pointer
    mov     %rsp, %rbp      # Set up new base pointer
    mov     16(%rbp), %rdi  # Load the address of the array into rdi
    mov     %rdi, %rax      # Copy the array address to rax
    mov     $0, %rcx        # Initialize counter to 0

loop_start:
    mov     (%rax), %rdx    # Load the current element into rdx
    cmp     $1000, %rdx     # Compare the current element with 1000
    jge     loop_end        # If greater than or equal to 1000, exit the loop
    inc     %rcx            # Increment the counter
    add     $8, %rax        # Move to the next element assuming each element is 8 bytes long
    jmp     loop_start      # Repeat the loop

loop_end:
    mov     %rcx, %rax      # Move the length into rax
    pop     %rbp            # Restore base pointer
    ret                     # Return
