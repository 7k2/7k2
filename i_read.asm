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

;Filename    : I_READ.ASM
;Description : Read a bitmap from the display surface buffer


INCLUDE IMGFUN.INC

.CODE


;----------- BEGIN OF FUNCTION IMGreadW ------------
;
; Put an non-compressed bitmap on image buffer.
; It does not handle color key transparency.
;
; Syntax : IMGreadW( imageBuf, pitch, x1, y1, x2, y2, bitmapBuf )
;
; char *imageBuf      - the pointer to the display surface buffer
; int  pitch          - pitch of the surface buffer
; int  x1, y1, x2, y2 - the read of the surface buffer to read
; short *bitmapPtr    - the pointer to the bitmap buffer
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

		PUBLIC    IMGreadW
IMGreadW   	PROC   	  imageBuf, pitch, x1, y1, x2, y2, bitmapPtr
		STARTPROC

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX

		;------ get the bitmap width and height -----;

		MOV     AX , DS
		MOV	ES , AX
		MOV     EDI, bitmapPtr

		MOV	EAX, x2
		SUB	EAX, x1
		INC 	EAX             	 ; calculate the bitmap width
		STOSW      		         ; store it into the bitmap buffer
		MOV	EBX, EAX		 ; store it to EBX for later internal processing

		MOV	EAX, y2
		SUB	EAX, y1
		INC	EAX			 ; calculate the bitmap height
		STOSW				 ; store it into the bitmap buffer
		MOV     ECX, EAX		 ; store it to ECX for later internal processing

		MOV	EDX, pitch		 ; EDX = lineDiff
		SUB	EDX, EBX		 ; lineDiff = image_width - 2*bitmap_width
		SUB	EDX, EBX

		CLD                              ; clear direction flag for MOVSB

		;------- pixels copying loop --------;

		CALC_ADDR ESI, x1, y1, pitch            ; Get the offset to the image buffer address
		MOV	EAX, ECX
		SHR	EBX,1
		JC	@@putLineOdd

		ALIGN	4
@@putLineEven:
		MOV     ECX, EBX
		REP MOVSD

		ADD	ESI, EDX		 ; EDX = lineDiff
		DEC	EAX
		JNZ	@@putLineEven            ; decrease the remain height and loop
		JMP	@@end

		ALIGN	4
@@putLineOdd:
		MOV     ECX, EBX
		REP MOVSD
		MOVSW

		ADD	ESI, EDX		 ; EDX = lineDiff
		DEC	EAX
		JNZ	@@putLineOdd             ; decrease the remain height and loop
		;JMP	@@end

@@end:          ENDPROC
IMGreadW   	ENDP

;----------- END OF FUNCTION IMGreadW ----------


END
