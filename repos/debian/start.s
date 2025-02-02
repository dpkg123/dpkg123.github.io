# start.s
.global _start
_start:
    call main
    mov $60, %rax  # exit syscall
    xor %rdi, %rdi # exit code 0
    syscall

