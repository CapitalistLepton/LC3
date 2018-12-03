;
; Zane Littrell
;
; Tests the LD, JMP, AND, ADD, NOT, and ST instructions
; Should have 9 stored at address x000D at the end
;
	.orig x0000
	AND	R0, R0, #0	; Zero out registers
	AND	R1, R1, #0
	AND	R2, R2, #0
	LD	R0, ADDR	; Load x0007 in R0
	JMP	R0		; Jump to x0007
	AND	R0, R0, #0	; Should not be run
	ADD	R1, R1, #5
	ADD	R2, R2, #-9	; Put -9 into R2
	NOT	R2, R2
	ADD	R2, R2, #1	; 2's complement on R2
	ST	R2, DATA	; 9 stored into DATA
	HALT

	ADDR	.fill 7		; address to jump to
	DATA	.blkw 1
	.end
