
.text
.globl loader_syscall
.type loader_syscall, @function
loader_syscall:
.cfi_startproc

  mov     %edi, %eax
  mov     %rsi, %rdi
  mov     %rdx, %rsi
  mov     %rcx, %rdx
  mov     %r8,  %r10
  mov     %r9,  %r8
  mov     8(%rsp), %r9

  syscall

  ret

.cfi_endproc;
.size loader_syscall, .-loader_syscall;


.text
.globl _jump_to_start
.type _jump_to_start, @function
_jump_to_start:
.cfi_startproc
  
  movq $0, (%rsp)
  movq $0, 8(%rsp)
  movq $0, 16(%rsp)
  jmp *%rdi

.cfi_endproc;
.size _jump_to_start, .-_jump_to_start;