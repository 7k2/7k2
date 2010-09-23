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

		PUBLIC    IMGbltWTrans
IMGbltWTrans   	PROC   	  imageBuf, pitch, x, y, bitmapPtr
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

		LODSW                            ; get bitmap height
		MOV     ECX, EAX

		MOV	EDX, pitch	 ; EDX = lineDiff
		SUB	EDX, EBX		 ; lineDiff = pitch - 2*bitmap_width
		SUB	EDX, EBX

		CLD                              ; clear direction flag for MOVSB

		;------- pixels copying loop --------;

		CALC_ADDR EDI, x, y, pitch              ; Get the offset to the image buffer address

		ALIGN	4
@@moreLines:
		PUSH	ECX
		MOV     ECX, EBX		 ; ECX is the line pixel counter

		ALIGN	4
@@morePoints:
		MOV	AX, [ESI]
		ADD	ESI,2
		CMP	AX, transparent_code_w
		JE	@@skipPoint
		MOV	[EDI], AX
@@skipPoint:
		ADD	EDI,2
		LOOP	@@morePoints

		ADD	EDI, EDX
		POP	ECX
		LOOP	@@moreLines

		ENDPROC
IMGbltWTrans   	ENDP

;----------- END OF FUNCTION IMGbltWTrans ----------


END
