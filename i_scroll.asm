;Filename    : I_SCROLL.ASM
;Description : scroll bitmap


INCLUDE IMGFUN.INC

.CODE

;----------- BEGIN OF FUNCTION IMGscrollW ------------
;
; Put an non-compressed bitmap on image buffer.
; It does not handle color key transparency.
;
; Syntax : IMGscrollW( imageBuf, pitch, srcX1, srcY1, srcX2, srcY2, destX1, destY1 )
;
; char *imageBuf - the pointer to the display surface buffer
; int srcX1, srcY1, srcX2, srcY2  - window of scrolling
; int destX1, destY1            - destination window
;
;-------------------------------------------------

		PUBLIC    IMGscrollW
IMGscrollW   	PROC   	  imageBuf, pitch, srcX1, srcY1, srcX2, srcY2, destX1, destY1
		STARTPROC

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX

		;------ get the bitmap width and height -----;

		MOV     AX , DS
		MOV	ES , AX

		CALC_ADDR	ESI, srcX1, srcY1, pitch
		CALC_ADDR	EDI, destX1, destY1, pitch
		CMP	ESI, EDI
		JB	@@reverse
		JE	@@end

@@forward:
		MOV	EBX, srcX2
		SUB	EBX, srcX1
		INC	EBX		; width count

		MOV	ECX, srcY2
		SUB	ECX, srcY1
		INC	ECX		; height count

		MOV	EDX, pitch
		SUB	EDX, EBX
		SUB	EDX, EBX	; srclineDiff/destLineDiff

		CLD

		SHR	EBX, 1
		JC	@@forwardOdd

		ALIGN	4
@@forwardEven:
		PUSH	ECX
		MOV	ECX, EBX
		REP MOVSD
		POP	ECX
		ADD	ESI, EDX
		ADD	EDI, EDX
		DEC	ECX
		JNZ	@@forwardEven
		JMP	@@end

		ALIGN	4
@@forwardOdd:
		PUSH	ECX
		MOV	ECX, EBX
		REP MOVSD
		MOVSW
		POP	ECX
		ADD	ESI, EDX
		ADD	EDI, EDX
		DEC	ECX
		JNZ	@@forwardOdd
		JMP	@@end

@@reverse:
		CALC_ADDR	ESI, srcX2, srcY2, pitch
		MOV	EBX, srcX2
		 MOV	ECX, srcY2
		SUB	EBX, srcX1
		 SUB	ECX, srcY1
		ADD	EBX, destX1	; destX2 = destX1 + srcX2 - srcX1
		 ADD	ECX, destY1	; destY2 = destY1 + srcY2 - srcY1
		CALC_ADDR	EDI, EBX, ECX, pitch

		MOV	EBX, srcX2
		SUB	EBX, srcX1
		INC	EBX		; width count

		MOV	ECX, srcY2
		SUB	ECX, srcY1
		INC	ECX		; height count

		MOV	EDX, pitch
		SUB	EDX, EBX
		SUB	EDX, EBX	; srclineDiff/destLineDiff

		STD			; reverse direction

		SHR	EBX, 1
		JC 	@@reverseOdd

		SUB	ESI, 2		; esi/edi point to last pixel, but dword copy 
		SUB	EDI, 2		; will copy next pixel
		ALIGN	4
@@reverseEven:
		PUSH	ECX
		MOV	ECX, EBX
		REP MOVSD
		POP	ECX
		SUB	ESI, EDX
		SUB	EDI, EDX
		DEC	ECX
		JNZ	@@reverseEven
		JMP	@@end

@@reverseOdd:
		PUSH	ECX
		MOV	ECX, EBX
		MOVSW				; do the last odd spot first
		REP MOVSD
		POP	ECX
		SUB	ESI, EDX
		SUB	EDI, EDX
		DEC	ECX
		JNZ	@@reverseOdd
		JMP	@@end

@@end:		CLD
		ENDPROC
IMGscrollW	ENDP

;----------- END OF FUNCTION IMGscrollW ----------

END
