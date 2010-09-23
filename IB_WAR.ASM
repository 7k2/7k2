;Filename    : IB_WAR.ASM
;Description : Blt a bitmap to the display surface buffer without color key transparency handling
;		but with colour Remapping


INCLUDE IMGFUN.INC
INCLUDE COLCODE.INC
INCLUDE I_EFFECT.INC

.CODE


;----------- BEGIN OF FUNCTION IMGbltWeakblendAreaRemap ------------
;
; Put an non-compressed bitmap on image buffer.
; It does not handle color key transparency.
; It can blt a specific area of the source image to the
; destination buffer instead of the whole source image.
; It handles colour remapping
;
; Syntax : IMGbltWeakblendAreaRemap( imageBuf, pitch, desX, desY, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorTable )
;
; char *imageBuf    - the pointer to the display surface buffer
; int  pitch     - pitch of the display surface buffer
; int  desX, desY   - where to put the area on the surface buffer
; char *bitmapPtr   - the pointer to the bitmap buffer
; int  srcX1, srcY1 - where to get the area on the source buffer
;      srcX2, srcY2
; short *colorTable  - a 256-entry color remapping table
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

		PUBLIC    IMGbltWeakblendAreaRemap
IMGbltWeakblendAreaRemap	PROC   	  imageBuf, pitch, desX, desY, bitmapPtr, srcX1, srcY1, srcX2, srcY2, colorTable
		LOCAL	  srcLineDiff, drawWidth, lineDiff
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
		LEA	ESI, [ESI+2*EAX]	 ; two bytes per pixel

		MOV	EAX, srcX2		 ; srcLineDiff = bitmap width - (srcX2-srcX1+1)
		SUB	EAX, srcX1
		INC	EAX

		MOV	srcLineDiff, EBX
		SUB	srcLineDiff, EAX
		SHL	srcLineDiff, 1		; two bytes per pixel

		MOV	EDX, pitch	 ; EDX = lineDiff
		SUB	EDX, EAX		 ; lineDiff = pitch - 2*(srcX2-srcX1+1)
		SUB	EDX, EAX
		MOV	lineDiff, EDX

		MOV	drawWidth, EAX		 ;  line pixel copy count

		MOV	ECX, srcY2		 ; blt lines = srcY2-srcY1+1
		SUB	ECX, srcY1
		INC	ECX

		CLD                              ; clear direction flag for MOVSB

		;---------- pixels copying loop ----------;

		CALC_ADDR_2 EDI, desX, desY, srcX1, srcY1, pitch     ; Get the address to the destination buffer
		MOV	EBX, colorTable

		ALIGN	4
@@putLine:
		PUSH	ECX
		MOV     ECX, drawWidth

		ALIGN	4
@@putPoint:
		MOVSX	EAX, WORD PTR [ESI]
		ADD	ESI,2
		SAR	AH,1		; opaque one less

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

		ADD	ESI, srcLineDiff
		ADD	EDI, lineDiff

		POP	ECX
		LOOP    @@putLine                ; decrease the remain height and loop
@@end:          ENDPROC
IMGbltWeakblendAreaRemap ENDP

;----------- END OF FUNCTION IMGbltWeakblendAreaRemap ----------


END
