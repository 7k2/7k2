;Filename    : IB_TRD.ASM
;Description : Blt a bitmap to the display surface buffer
;	       with decompression, transparency handling and colormaping


INCLUDE IMGFUN.INC
INCLUDE COLCODE.INC
INCLUDE I_EFFECT.INC

.CODE


;----------- BEGIN OF FUNCTION IMGbltTransRemapDecompress ------
;
; Put a compressed bitmap on image buffer.
; It does handle color key transparency and remaping.
;
; Syntax : IMGbltTransRemapDecompress( imageBuf, pitch, x, y, bitmapBuf,
;                 colorTable)
;
; char *imageBuf - the pointer to the display surface buffer
; int  pitch     - pitch of the display surface buffer
; int  x,y       - where to put the image on the surface buffer
; char *bitmapPtr  - the pointer to the bitmap buffer
; short *colorTable - color remaping table
;
; two counters are maintained, EDX and ECX for counting no. of rows
; and no. of columns remaining to draw
; if the counter reach zero, exit the loop
;
; ESI initally points to the start of bitmap data
; EDI initally points to the top left hand cornder of the destination
;     in video memory
;
; compressed data is loaded from ESI, into AL
; If AL is non-transparent, blit the point to video memory.
; If AL is transparent, seek EDI forward. If the right side of the
; destination is passed,
;   1. seek EDI to the left side of the next line
;   2. if run-length is still very long, seek one more line
;   3. residue (of run-length) is added to EDI, ECX will count from a number
;      lower than the width of bitmap
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

		PUBLIC    IMGbltTransRemapDecompress
IMGbltTransRemapDecompress PROC imageBuf,pitch,x,y,bitmapPtr,colorTable
		LOCAL	bitmapWidth:DWORD, bitmapHeight:DWORD
		STARTPROC

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX

		;------ get the bitmap width and height -----;

		MOV     AX , DS
		MOV	ES , AX
		MOV     ESI, bitmapPtr
		CLD

		;------ calculate destination on the video memory ----;
		CALC_ADDR EDI, x, y, pitch	; Get the address to the destination buffer

		;------ calculate bitmapWidth - no. of points ploted on each row --;
		XOR	EAX, EAX
		LODSW                            ; get bitmap width
		MOV     bitmapWidth, EAX

		;----- calculate number of rows to be ploted on the screen
		LODSW                            ; get bitmap height
		MOV	bitmapHeight, EAX
		MOV     EDX, EAX		; EDX contains no. of rows remain

		MOV	EBX, colorTable
@@loopY:
		MOV	ECX, bitmapWidth
@@loopX:
		MOVZX	EAX, BYTE PTR [ESI]
		INC	ESI
		PRE_REMAP
		JUMP_IF_TRANS_OR_EFFECT al, @@transOrEffect
		; JUMP_IF_TRANS al, @@compressed1
@@nonTrans:
		; -----  00-F7, simply blit the point on video memory ----
		POST_REMAP
		MOV	[EDI], AX
@@nonTransWrite:
		ADD	EDI,2

		; ----------  test end of line ------------
		; (pass the right of clipping window)
		LOOP	@@loopX
		ADD	EDI, pitch
		SUB	EDI, bitmapWidth
		; JMP	@@endloopY	; reduce jump
		DEC	EDX
		JNE	@@loopY
		JMP	@@end

		ALIGN	4
@@effect:
		; F0-F7
		CALL	IMGeffect	; al is effect id , [EDI] is the dest pixel
		JMP     @@nonTransWrite

		ALIGN	4
@@transOrEffect:
		JUMP_ON_FEW_TRANS_OR_EFFECT al, @@compress1a, @@effect
@@compressed1:
		; JUMP_IF_NOT_MANY_TRANS al, @@compress1a
		; F8
		MOV	AL, [ESI]
		INC	ESI
		ENCODE_FEW_TRANS_CODE al
@@compress1a:
		; F7-FF
		DECODE_FEW_TRANS_CODE al
		MOVZX	EAX,AL

		CMP	ECX, EAX
		JBE	@@compress2
		; ECX > EAX
		; meaning the run-length is still within output bitmap
		LEA	EDI, [EDI + 2*EAX]	; skip the number of points
		SUB	ECX, EAX
		JMP	@@loopX

		ALIGN	4
@@compress2:
		; run-length is outside clip window
		; adjust EDI to point to left of next line
		SUB	EAX, ECX
		ADD	EDI, pitch
		LEA	EDI, [EDI + 2 * ECX]	; add twice for word size pixel
		SUB	EDI, bitmapWidth
		SUB	EDI, bitmapWidth	; sub twice for word size pixel

@@compress3a:
		; if EAX is longer than width of bitmap,
		; position to EDI one line below
		CMP	EAX, bitmapWidth
		JB	@@compress4
		ADD	EDI, pitch
		SUB	EAX, bitmapWidth
		DEC	EDX			; minus y remain by 1
		JNE	@@compress3a
		JMP	@@end

		ALIGN	4
@@compress4:
		; add remainder to EDI
		; ECX has another initial value other than bitmapWidth
		LEA	EDI, [EDI + 2*EAX]
		MOV     ECX, bitmapWidth
		SUB	ECX, EAX
		DEC	EDX
		JNE	@@loopX
		JMP	@@end

		ALIGN	4
@@endloopY:
		DEC	EDX
		JNE	@@loopY

@@end:          ENDPROC
IMGbltTransRemapDecompress   	ENDP

;----------- END OF FUNCTION IMGbltTransRemapDecompress ----------

END
