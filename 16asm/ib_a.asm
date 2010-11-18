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

;Filename    : IB_A.ASM
;Description : Blt a bitmap to the display surface buffer without color key transparency handling


INCLUDE IMGFUN.INC

.CODE


;----------- BEGIN OF FUNCTION IMGbltWArea ------------
;
; Put an non-compressed bitmap on image buffer.
; It does not handle color key transparency.
; It can blt a specific area of the source image to the
; destination buffer instead of the whole source image.
;
; Syntax : IMGbltWArea( imageBuf, pitch, desX, desY, bitmapBuf, srcX1, srcY1, srcX2, srcY2 )
;
; char *imageBuf    - the pointer to the display surface buffer
; int  pitch        - pitch of the display surface buffer
; int  desX, desY   - where to put the area on the surface buffer
; short *bitmapPtr   - the pointer to the bitmap buffer
; int  srcX1, srcY1 - where to get the area on the source buffer
;      srcX2, srcY2
;
;-------------------------------------------------
;
; Format of the bitmap data :
;
; <short>  width
; <short>  height
; <short..> bitmap image
;
;-------------------------------------------------

		PUBLIC    IMGbltWArea
IMGbltWArea   	PROC   	  imageBuf, pitch, desX, desY, bitmapPtr, srcX1, srcY1, srcX2, srcY2
		LOCAL	  srcLineDiff
		STARTPROC

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX

		;------ get the bitmap width and height -----;

		MOV     AX , DS
		MOV	ES , AX
		MOV     ESI, bitmapPtr

		XOR	EAX, EAX
		LODSW                            ; get bitmap width
		MOV     EBX, EAX
		ADD	ESI, 2			 ; by pass the bitmap height, we don't need it. srcY2 and srcY1 will give us the data we need

		MUL     srcY1			 ; calculate the source starting address
		ADD	EAX, srcX1               ; bitmap width * srcY1 + srcX1
		LEA	ESI, [ESI + 2*EAX]       ; add twice for word size pixel

		MOV	EAX, srcX2		 ; srcLineDiff = 2*( bitmap width - (srcX2-srcX1+1))
		SUB	EAX, srcX1
		INC	EAX

		MOV	srcLineDiff, EBX
		SUB	srcLineDiff, EAX
		SHL	srcLineDiff, 1

		MOV	EDX, pitch	 ; EDX = lineDiff
		SUB	EDX, EAX		 ; lineDiff = image_width - 2*(srcX2-srcX1+1)
		SUB	EDX, EAX

		MOV	EBX, EAX		 ; EBX - line pixel copy count

		MOV	ECX, srcY2		 ; blt lines = srcY2-srcY1+1
		SUB	ECX, srcY1
		INC	ECX

		CLD                              ; clear direction flag for MOVSB

		;---------- pixels copying loop ----------;

		CALC_ADDR_2 EDI, desX, desY, srcX1, srcY1, pitch     ; Get the address to the destination buffer

		ALIGN	4
@@putLine:
		PUSH	ECX
		MOV     ECX, EBX
		REP MOVSW

		ADD	EDI, EDX		 ; EDX = lineDiff
		POP	ECX
		ADD	ESI, srcLineDiff

		DEC	ECX
		JNZ	@@putLine                ; decrease the remain height and loop

@@end:          ENDPROC
IMGbltWArea   	ENDP

;----------- END OF FUNCTION IMGbltWArea ----------


END
