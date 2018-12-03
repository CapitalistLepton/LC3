;
; Zane Littrell
;
; Tests the LD, JMP, AND, ADD, NOT, and ST instructions
; Should have 9 stored at address x000A at the end
;
	.orig x0000
	LD	R0, ADDR	; Load x0004 in R0
	JMP	R0		; Jump to x0004
	AND	R0, R0, #0	; Should not be run
	ADD	R1, R1, #5
	ADD	R2, R2, #-9	; Put -9 into R2
	NOT	R2, R2
	ADD	R2, R2, #1	; 2's complement on R2
	ST	R2, DATA	; 9 stored into DATA
	HALT

	ADDR	.fill 4
	DATA	.blkw 1
	.end
