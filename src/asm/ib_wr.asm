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

;Filename    : IB_WR.ASM
;Description : Blt a bitmap to the display surface buffer with colour remapping
;              but without color key transparency handling


INCLUDE imgfun.inc
INCLUDE colcode.inc
INCLUDE i_effect.inc

.CODE


;----------- BEGIN OF FUNCTION IMGbltWeakblendRemap ------------
;
; Put an non-compressed bitmap on image buffer.
; It does not handle color key transparency but colorRemapping
;
; Syntax : IMGbltWeakblendRemap( imageBuf, pitch, x, y, bitmapBuf, colorTable )
;
; char *imageBuf - the pointer to the display surface buffer
; int  pitch        - pitch of the display surface buffer
; int  x,y       - where to put the image on the surface buffer
; char *bitmapPtr  - the pointer to the bitmap buffer
; short *colorTable - a 256-entry color remapping table
;
;-------------------------------------------------
;
; Format of the bitmap data :
;
; <short>  width
; <short>  height
; <char+char..> bitmap+logAlpha image
;
;-------------------------------------------------

		PUBLIC	IMGbltWeakblendRemap
IMGbltWeakblendRemap   	PROC	imageBuf, pitch, x, y, bitmapPtr, colorTable
		LOCAL	drawWidth, lineDiff
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
		MOV	drawWidth, EAX

		LODSW                            ; get bitmap height
		MOV     ECX, EAX

		ADD	EBX, EBX
		MOV	EDX, pitch	 ; EDX = lineDiff
		SUB	EDX, EBX		 ; lineDiff = pitch - 2*bitmap_width
		MOV	lineDiff, EDX

		CLD                              ; clear direction flag for MOVSB

		;------- pixels copying loop --------;

		CALC_ADDR EDI, x, y, pitch              ; Get the offset to the image buffer address
		MOV	EBX, colorTable

		ALIGN	4
@@putLine:
		PUSH	ECX
		MOV     ECX, drawWidth

		ALIGN	4
@@putPoint:
		MOVSX	EAX, WORD PTR [ESI]
		ADD	ESI,2
		SAR	AH, 1			; less opaque

		PUSH	ECX
		MOV	DX, WORD PTR [EDI]
		MOVSX	ECX, AH
		PRE_REMAP
		POST_REMAP
		SHR	DX, CL
		AND	DX, log_alpha_mask[ECX*2]
		ADD	AX, DX

		MOV	[EDI], AX
		POP	ECX
		ADD	EDI,2
		LOOP	@@putPoint

		ADD	EDI, lineDiff

		POP	ECX
		LOOP    @@putLine                ; decrease the remain height and loop

@@end:          ENDPROC
IMGbltWeakblendRemap   	ENDP

;----------- END OF FUNCTION IMGbltWeakblendRemap ----------


END
