; Seven Kingdoms 2: The Fryhtan War
;
; Copyright 1999 Enlight Software Ltd.
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

;Filename    : I_ALPHA.ASM
;Description : Draw a rectangle bar on vga image buffer alpha blending


INCLUDE imgfun.inc
INCLUDE i_effect.inc

.CODE

;--------- BEGIN OF FUNCTION IMGbarAlpha -----------
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
; int  logAlpha    - right shift bit count
; int  color       - the color of the line
;
		PUBLIC IMGbarAlpha
IMGbarAlpha          PROC   imageBuf, pitch, x1, y1, x2, y2, logAlpha, color
		LOCAL	lineDiff, bitmapWidth
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
		MOV	bitmapWidth, EBX
		MOV	lineDiff, EDX
		MOV	ESI, ECX		; yCount

		CLD                              ; clear direction flag for MOVSW

		; ------ calculate bit mask and add -----

		MOV	ECX, logAlpha
		MOV	BX, log_alpha_mask[ECX*2]

		; ----- value added is color - alpha * color ------- ;

		MOV	AX, word ptr color
		MOV	DX, AX
		SHR	AX, CL
		AND	AX, BX
		SUB	DX, AX

		; ------- duplicate BX, DX to high 16 bits --------- ;

		MOV	AX, BX
		SHL	EBX, 16
		MOV	BX, AX

		MOV	AX, DX
		SHL	EDX, 16
		MOV	DX, AX

		;------- pixels copying loop --------;

		CALC_ADDR EDI, x1, y1, pitch            ; Get the offset to the image buffer address

		SHR	bitmapWidth, 1
		JZ	@@putZeroWidth
		JC	@@putLineOdd

		ALIGN	4
@@putLineEven:
		PUSH	ESI
		MOV	ESI, bitmapWidth
		ALIGN	4
@@putPixelEven:
		MOV	EAX, [EDI]
		SHR	EAX, CL
		AND	EAX, EBX		; mask
		ADD	EAX, EDX		; add
		MOV	[EDI], EAX
		ADD	EDI, 4
		DEC	ESI
		JNZ	@@putPixelEven
		POP	ESI
		ADD	EDI, lineDiff
		DEC	ESI
		JNZ	@@putLineEven
		JMP	@@end

		ALIGN	4
@@putLineOdd:
		PUSH	ESI
		MOV	ESI, bitmapWidth
		ALIGN	4
@@putPixelOdd:
		MOV	EAX, [EDI]
		SHR	EAX, CL
		AND	EAX, EBX		; mask
		ADD	EAX, EDX		; add
		MOV	[EDI], EAX
		ADD	EDI, 4
		DEC	ESI
		JNZ	@@putPixelOdd
		; final pixel
		MOV	AX, [EDI]
		SHR	AX, CL
		AND	AX, BX
		ADD	AX, DX
		MOV	[EDI], AX
		ADD	EDI, 2

		POP	ESI
		ADD	EDI, lineDiff
		DEC	ESI
		JNZ	@@putLineOdd
		JMP	@@end

@@putZeroWidth:
		JNC	@@end
		ALIGN	4
@@putVline:
		MOV	AX, [EDI]
		SHR	AX, CL
		AND	AX, BX
		ADD	AX, DX
		MOV	[EDI], AX
		ADD	EDI, pitch
		DEC	ESI
		JNZ	@@putVLine
		;JMP	@@end

@@end:
		ENDPROC
IMGbarAlpha	ENDP


;---------- END OF FUNCTION IMGbarAlpha ------------

END
