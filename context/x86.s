.globl get_context
get_context:
  movq (%rsp), %r8
  movq %r8, 8*0(%rdi)
  leaq 8(%rsp), %r8
  movq %r8, 8*1(%rdi)
  movq %rbx, 8*2(%rdi)
  movq %rbp, 8*3(%rdi)
  movq %r12, 8*4(%rdi)
  movq %r13, 8*5(%rdi)
  movq %r14, 8*6(%rdi)
  movq %r15, 8*7(%rdi)
  movq %rdi, 8*8(%rdi)
  xorl %eax, %eax
  ret

.globl set_context
set_context:
  movq 8*0(%rdi), %r8
  movq 8*1(%rdi), %rsp
  movq 8*2(%rdi), %rbx
  movq 8*3(%rdi), %rbp
  movq 8*4(%rdi), %r12
  movq 8*5(%rdi), %r13
  movq 8*6(%rdi), %r14
  movq 8*7(%rdi), %r15
  movq 8*8(%rdi), %rdi
  pushq %r8
  xorl %eax, %eax
  ret

.globl swap_context
swap_context:
  movq (%rsp), %r8
  movq %r8, 8*0(%rdi)
  leaq 8(%rsp), %r8
  movq %r8, 8*1(%rdi)
  movq %rbx, 8*2(%rdi)
  movq %rbp, 8*3(%rdi)
  movq %r12, 8*4(%rdi)
  movq %r13, 8*5(%rdi)
  movq %r14, 8*6(%rdi)
  movq %r15, 8*7(%rdi)
  movq %rdi, 8*8(%rdi)
  movq 8*0(%rsi), %r8
  movq 8*1(%rsi), %rsp
  movq 8*2(%rsi), %rbx
  movq 8*3(%rsi), %rbp
  movq 8*4(%rsi), %r12
  movq 8*5(%rsi), %r13
  movq 8*6(%rsi), %r14
  movq 8*7(%rsi), %r15
  movq 8*8(%rdi), %rdi
  pushq %r8
  xorl %eax, %eax
  ret

.globl link_handler
link_handler:
  movq %rbp, %rdi
  call set_context
