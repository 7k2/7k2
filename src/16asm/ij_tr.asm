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

;Filename    : IJ_TR.ASM
;Description : Add a bitmap and another display surface to the display
;              surface on the same place with transparency handling


INCLUDE imgfun.inc
INCLUDE colcode.inc

.CODE


;-------- BEGIN OF FUNCTION IMGjoinTransRemap ----------
;
; Put an non-compressed bitmap on image buffer.
; It handles color key transparency. The color key code is 255.
;
; Syntax : IMGjoinTransRemap( imageBuf, imgPitch, backBuf, backPitch, x, y, bitmapBuf, colorRemapTable )
;
; char *imageBuf - the pointer to the display surface buffer
; int imgPitch   - the pitch of the display surface buffer
; char *backBuf  - the pointer to the back buffer
; int backPitch  - the pitch of the back buffer
; int  x,y       - where to put the image on the surface buffer
; char *bitmapPtr  - the pointer to the bitmap buffer
; short *colorRemapTable - color remap table
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

		PUBLIC	IMGjoinTransRemap
IMGjoinTransRemap	PROC	imageBuf, imgPitch, backBuf, backPitch, x, y, bitmapPtr, colorRemapTable
		LOCAL	bitmapWidth:DWORD
		STARTPROC

		MOV	EAX, backBuf
		MOV     image_buf, EAX
		CALC_ADDR EBX, x, y, backPitch

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX
		CALC_ADDR EDI, x, y, imgPitch   ; Get the offset to the image buffer address

		;------ get the bitmap width and height -----;

		MOV     AX , DS
		MOV	ES , AX
		MOV     ESI, bitmapPtr

		XOR	EAX, EAX
		LODSW                            ; get bitmap width
		MOV     bitmapWidth, EAX
		SHL	EAX, 1
		SUB	imgPitch, EAX		; lineDiff = image_width - 2 * bitmap_width
		SUB	backPitch, EAX		; lineDiff = image_width - 2 * bitmap_width

		LODSW                           ; get bitmap height
		MOV     ECX, EAX

		MOV	EDX, ESI		; EDX points to bitmapPtr+4, start of bitmap data
		MOV	ESI, EBX		; ESI points to start of (x1,y1) of backBuf
		MOV	EBX, colorRemapTable

		CLD                              ; clear direction flag for MOVSB

		;------- pixels copying loop --------;

		ALIGN	4
@@moreLines:
		PUSH	ECX
		MOV     ECX, bitmapWidth	 ; ECX is the line pixel counter

		ALIGN	4
@@moreColumns:
		MOVZX	EAX, BYTE PTR [EDX]
		INC	EDX
		PRE_REMAP
		CMP	AL, TRANSPARENT_CODE
		JNE	@@notrans

@@trans:
		MOVSW				; transparent pixel, copy from backBuf
		LOOP	@@moreColumns
		ADD	EDI, imgPitch
		POP	ECX
		ADD	ESI, backPitch
		DEC	ECX
		JNZ	@@moreLines
		JMP	@@end

		ALIGN	4
@@notrans:
		POST_REMAP
		ADD	ESI, 2
		MOV	[EDI],AX
		ADD	EDI,2
		LOOP	@@moreColumns
		ADD	EDI, imgPitch
		POP	ECX
		ADD	ESI, backPitch
		DEC	ECX
		JNZ	@@moreLines

@@end:          ENDPROC
IMGjoinTransRemap	ENDP

;----------- END OF FUNCTION IMGjoinTransRemap ----------

		END
