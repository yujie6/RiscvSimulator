	.file	"sample.c"
	.text
	.globl	judgeResult
	.bss
	.align 4
	.type	judgeResult, @object
	.size	judgeResult, 4
judgeResult:
	.zero	4
	.section	.rodata
	.align 4
	.type	_ZL3Mod, @object
	.size	_ZL3Mod, 4
_ZL3Mod:
	.long	253
	.text
	.globl	_Z8printInti
	.type	_Z8printInti, @function
_Z8printInti:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	judgeResult(%rip), %eax
	xorl	-4(%rbp), %eax
	movl	%eax, judgeResult(%rip)
	movl	judgeResult(%rip), %eax
	addl	$173, %eax
	movl	%eax, judgeResult(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	_Z8printInti, .-_Z8printInti
	.globl	_Z8printStrPKc
	.type	_Z8printStrPKc, @function
_Z8printStrPKc:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
.L4:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L5
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %edx
	movl	judgeResult(%rip), %eax
	xorl	%edx, %eax
	movl	%eax, judgeResult(%rip)
	movl	judgeResult(%rip), %eax
	addl	$521, %eax
	movl	%eax, judgeResult(%rip)
	addq	$1, -8(%rbp)
	jmp	.L4
.L5:
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	_Z8printStrPKc, .-_Z8printStrPKc
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$177, %edi
	call	_Z8printInti
	movl	judgeResult(%rip), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 8.3.0-6ubuntu1) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
