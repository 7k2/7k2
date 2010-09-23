;Filename    : I_BAR.ASM
;Description : Draw a rectangle bar on vga image buffer


INCLUDE IMGFUN.INC

.CODE

;--------- BEGIN OF FUNCTION IMGbar -----------
;
; Draw a bar on the VGA screen
;
; Note : No border checking is made in this function.
;	 Placing an icon outside image buffer will cause serious BUG.
;
; char *imageBuf   - the pointer to the display surface buffer
; int  pitch       - the pitch of the display surface buffer
; int  x1,y1       - the top left vertex of the bar
; int  x2,y2       - the bottom right vertex of the bar
; int  color       - the color of the line
;
		PUBLIC IMGbar
IMGbar          PROC   imageBuf, pitch, x1, y1, x2, y2, color
		STARTPROC

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX

		;------ calc bar width and height -----;

		MOV     AX , DS
		MOV	ES , AX

		MOV     EBX, x2
		SUB	EBX, x1
		INC	EBX

		MOV	ECX, y2
		SUB	ECX, y1
		INC	ECX

		MOV	EDX, pitch		 ; EDX = lineDiff
		SUB	EDX, EBX		 ; lineDiff = image_width - 2*icon_width
		SUB	EDX, EBX

		MOV	AX, word ptr color

		CLD                              ; clear direction flag for MOVSW

		;------- pixels copying loop --------;

		CALC_ADDR EDI, x1, y1, pitch            ; Get the offset to the image buffer address

		CMP	EBX, 1				; test for drawing vertical line
		JE	@@putVLine

		; ------ duplicate AX to EAX ----------;

		PUSH	ECX
		MOV	CX, AX
		SHL	EAX, 16
		MOV	AX, CX
		POP	ECX

		; ------- test odd or even pixel per line -------;

		SHR	EBX,1
		JC	@@putLineOdd

		ALIGN	4
@@putLineEven:
		PUSH	ECX
		MOV	ECX,EBX
		REP	STOSD
		POP	ECX
		ADD	EDI,EDX
		DEC	ECX
		JNZ	@@putLineEven
		JMP	@@end

@@putVLine:	; special for drawing vertical line
		MOV	EDX, pitch
		ALIGN	4
@@putVLine1:
		MOV	[EDI], AX
		ADD	EDI, EDX
		DEC	ECX
		JNZ	@@putVLine1
		JMP	@@end

		ALIGN	4
@@putLineOdd:
		PUSH	ECX
		MOV	ECX,EBX
		REP	STOSD
		STOSW
		POP	ECX
		ADD	EDI,EDX
		DEC	ECX
		JNZ	@@putLineOdd
		;JMP	@@end
@@end:
		ENDPROC
IMGbar          ENDP


;---------- END OF FUNCTION IMGbar ------------

END
