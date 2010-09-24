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

;Filename    : IB_T.ASM
;Description : Blt a bitmap to the display surface buffer with color key transparency handling


INCLUDE IMGFUN.INC
INCLUDE COLCODE.INC

.CODE


;-------- BEGIN OF FUNCTION IMGbltWTrans ----------
;
; Put an non-compressed bitmap on image buffer.
; It handles color key transparency. The color key code is 255.
;
; Syntax : IMGbltWTrans( imageBuf, pitch, x, y, bitmapBuf )
;
; char *imageBuf - the pointer to the display surface buffer
; int  pitch        - pitch of the display surface buffer
; int  x,y       - where to put the image on the surface buffer
; char *bitmapPtr  - the pointer to the bitmap buffer
;
;-------------------------------------------------
;
; Format of the bitmap data :
;
; <short>  width
; <short>  height
; <char..> bitmap image
;
;-------------------------------------------------

		PUBLIC	IMGbltWTrans
IMGbltWTrans   	PROC	imageBuf, pitch, x, y, bitmapPtr
		LOCAL	bitmapWidth
		STARTPROC

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX

		;------ get the bitmap width and height -----;

		MOV	AX , DS
		MOV	ES , AX
		MOV	ESI, bitmapPtr

		XOR	EAX, EAX
		LODSW                           ; get bitmap width
		MOV	EBX, EAX

		LODSW                           ; get bitmap height
		MOV	ECX, EAX

		MOV	EDX, pitch              ; EDX = lineDiff
		SUB	EDX, EBX		; lineDiff = pitch - 2*bitmap_width
		SUB	EDX, EBX
		MOV	bitmapWidth, EBX

		CLD                             ; clear direction flag for MOVSB

		;------- pixels copying loop --------;

		CALC_ADDR EDI, x, y, pitch              ; Get the offset to the image buffer address
		MOV	BX, transparent_code_w
		; SHL	EBX, 16
		; MOV	BX, transparent_code_w		; duplicate EBX to high 16-bit

		SHR	bitmapWidth, 1
		JC	@@moreLinesOdd

		ALIGN	4
@@moreLinesEven:
		PUSH	ECX
		MOV	ECX, bitmapWidth

		ALIGN	4
@@morePointsEven:
		MOV	EAX, [ESI]
		ADD	ESI, 4
		CMP	AX, BX
		JE	@@skipPointsEven1
		MOV	[EDI], AX
@@skipPointsEven1:
		ROR	EAX, 16
		CMP	AX, BX
		JE	@@skipPointsEven2
		MOV	[EDI+2], AX
@@skipPointsEven2:
		ADD	EDI, 4
		DEC	ECX
		JNZ	@@morePointsEven

		POP	ECX
		ADD	EDI, EDX
		DEC	ECX
		JNE	@@moreLinesEven
		JMP	@@end

		ALIGN	4
@@moreLinesOdd:
		PUSH	ECX
		MOV	ECX, bitmapWidth
		JECXZ	@@skipPointsOdd3

		ALIGN	4
@@morePointsOdd:
		MOV	EAX, [ESI]
		ADD	ESI, 4
		CMP	AX, BX
		JE	@@skipPointsOdd1
		MOV	[EDI], AX
@@skipPointsOdd1:
		ROR	EAX, 16
		CMP	AX, BX
		JE	@@skipPointsOdd2
		MOV	[EDI+2], AX
@@skipPointsOdd2:
		ADD	EDI, 4
		DEC	ECX
		JNZ	@@morePointsOdd

		; odd points at the end of line
@@skipPointsOdd3:
		MOV	AX, [ESI]
		ADD	ESI, 2
		CMP	AX, BX
		JE	@@skipPointsOdd4
		MOV	[EDI], AX
@@skipPointsOdd4:
		ADD	EDI, 2

		POP	ECX
		ADD	EDI, EDX
		DEC	ECX
		JNE	@@moreLinesOdd
		JMP	@@end

;		ALIGN	4
;@@moreLines:
;		PUSH	ECX
;		MOV	ECX, bitmapWidth	 ; ECX is the line pixel counter
;		ALIGN	4
;@@morePoints:
;		MOV	AX, [ESI]
;		ADD	ESI,2
;		CMP	AX, BX
;		JE	@@skipPoint
;		MOV	[EDI], AX
;@@skipPoint:
;		ADD	EDI,2
;		DEC	ECX
;		JNE	@@morePoints
;		POP	ECX
;		ADD	EDI, EDX
;		DEC	ECX
;		JNE	@@moreLines

@@end:		ENDPROC
IMGbltWTrans	ENDP

;----------- END OF FUNCTION IMGbltWTrans ----------


END
