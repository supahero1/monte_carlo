section .data
align 16
float_one:	dd	0x3F800000		; 1.0
abs_mask:	dd	0x7FFFFFFF
hundred:	dd	0x42C80000		; 100.0


section .text
global monteCarlo

monteCarlo:
	push		rbp
	mov		rbp, rsp

	; paint the background, the square, and the circle

	xorps		xmm0, xmm0		; f x
	xorps		xmm1, xmm1		; f y
	movss		xmm2, [rel float_one]	; f 1.0
	movss		xmm3, [rdx + 0]		; square x
	movss		xmm4, [rdx + 4]		; square y
	movss		xmm5, [rdx + 8]		; square r
	movss		xmm6, [rcx + 0]		; circle x
	movss		xmm7, [rcx + 4]		; circle y
	movss		xmm8, [rcx + 8]		; circle r
	movss		xmm9, [rel abs_mask]	; ~sign bit
	mov		rax, [rbp + 16]		; pixel ptr
	xor		r10, r10		; x, r8 = width
	xor		r11, r11		; y, r9 = height

do_loop:
	mov		rcx, 0x00FFFFFF

	; check the square

	movss		xmm10, xmm1
	subss		xmm10, xmm4
	andps		xmm10, xmm9

	comiss		xmm10, xmm5
	ja		not_in_square

	movss		xmm10, xmm0
	subss		xmm10, xmm3
	andps		xmm10, xmm9

	comiss		xmm10, xmm5
	ja		not_in_square

	; within the square

	or		rcx, 0x7F000000

not_in_square:

	; check the circle

	movss		xmm10, xmm1
	subss		xmm10, xmm7
	mulss		xmm10, xmm10

	movss		xmm11, xmm0
	subss		xmm11, xmm6
	mulss		xmm11, xmm11

	addss		xmm10, xmm11
	movss		xmm11, xmm8
	mulss		xmm11, xmm11

	comiss		xmm10, xmm11
	ja		not_in_circle

	; within the circle

	add		rcx, 0x80000000

not_in_circle:

	; paint the pixel

	mov		[rax], ecx
	add		rax, 4

	; go next

	inc		r10
	cmp		r10, r8
	je		loop_x_end
	addss		xmm0, xmm2
	jmp		do_loop
loop_x_end:
	inc		r11
	cmp		r11, r9
	je		loop_y_end
	xor		r10, r10
	xorps		xmm0, xmm0
	addss		xmm1, xmm2
	jmp		do_loop
loop_y_end:

	; paint the random points

	xor		rcx, rcx
	xor		r9, r9
rand_loop:
	cmp		rcx, rdi
	je		rand_loop_end
	movss		xmm0, [rsi + rcx * 8 + 0]
	movss		xmm1, [rsi + rcx * 8 + 4]

	; count the points : check the square

	movss		xmm10, xmm1
	subss		xmm10, xmm4
	andps		xmm10, xmm9

	comiss		xmm10, xmm5
	ja		not_within

	movss		xmm10, xmm0
	subss		xmm10, xmm3
	andps		xmm10, xmm9

	comiss		xmm10, xmm5
	ja		not_within

	; count the points : within the square, check the circle

	movss		xmm10, xmm1
	subss		xmm10, xmm7
	mulss		xmm10, xmm10

	movss		xmm11, xmm0
	subss		xmm11, xmm6
	mulss		xmm11, xmm11

	addss		xmm10, xmm11
	movss		xmm11, xmm8
	mulss		xmm11, xmm11

	comiss		xmm10, xmm11
	ja		not_within

	; count the points : within the square and the circle

	inc		r9

not_within:

	cvtss2si	r10, xmm0
	cvtss2si	rax, xmm1
	mul		r8
	add		rax, r10
	shl		rax, 2
	add		rax, [rbp + 16]
	mov		dword[rax], 0xFF201080
	inc		rcx
	jmp		rand_loop
rand_loop_end:

	cvtsi2ss	xmm0, r9
	cvtsi2ss	xmm1, rdi
	divss		xmm0, xmm1		; within / total
	movss		xmm1, [rel hundred]
	mulss		xmm0, xmm1		; * 100.0


	mov		rsp, rbp
	pop		rbp
	ret
