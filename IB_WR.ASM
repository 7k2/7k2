;Filename    : IB_WR.ASM
;Description : Blt a bitmap to the display surface buffer with colour remapping
;              but without color key transparency handling


INCLUDE IMGFUN.INC
INCLUDE COLCODE.INC
INCLUDE I_EFFECT.INC

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
