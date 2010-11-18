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

;Filename    : I_BRIGHT.ASM
;Description : Change brightness of a rectangle bar on vga image buffer


INCLUDE IMGFUN.INC
INCLUDE I_EFFECT.INC

.DATA

MAX_BRIGHTNESS_ADJUST_DEGREE = 32
MAX_BRIGHTNESS_SHIFT = 5

local_pixel_format	dd	?	; should be equal to pixel_format_flag

red_table		dw	(2*MAX_BRIGHTNESS_ADJUST_DEGREE+1)*32 dup(?)
green_table		dw	(2*MAX_BRIGHTNESS_ADJUST_DEGREE+1)*32 dup(?)
blue_table		dw	(2*MAX_BRIGHTNESS_ADJUST_DEGREE+1)*32 dup(?)

.CODE

		PUBLIC INITbright, IMGbrightBar

;--------- BEGIN OF FUNCTION INITbright -----------
;
; Initialization routine for IMGbrightBar
; call after the pixel format is determined
; Call once before any calling IMGbrightBar
;
; int pixelFormatFlag		pixel format of the primary surface
;
INITbright	PROC	pixelFormatFlag
		STARTPROC

		MOV	EAX, pixelFormatFlag
		MOV	local_pixel_format, EAX

		MOV	ECX, -MAX_BRIGHTNESS_ADJUST_DEGREE
		MOV	EDI, 0		; index on red_table, green_table and blue_table
@@initBrightLoop1:
		MOV	EAX, 0
@@initBrightLoop2:
		PUSH	EAX

		; calcuate new EAX (ECX is brightness)
		CMP	ECX, 0
		JG	@@initBrightPos
@@initBrightNeg:	; negative brightness
		; EAX = EAX * (brigthness + MAX_B) / MAX_B
		MOV	EDX, ECX
		ADD	EDX, MAX_BRIGHTNESS_ADJUST_DEGREE
		MUL	EDX
		SHR	EAX, MAX_BRIGHTNESS_SHIFT
		JMP	@@initBright0
@@initBrightPos:
		; EAX = EAX + (31 - EAX) * brightness / MAX_B
		; EAX = (31*brightness + EAX*(MAX_B - brightness)) / MAX_B
		MOV	EDX, MAX_BRIGHTNESS_ADJUST_DEGREE
		SUB	EDX, ECX
		MUL	EDX
		MOV	EDX, ECX
		SHL	EDX, 5
		SUB	EDX, ECX		; EDX = 31*brightness
		ADD	EAX, EDX
		SHR	EAX, MAX_BRIGHTNESS_SHIFT
		; JMP	@@initBright0

		; EAX now between 0 to 31
@@initBright0:
		CMP	EAX, 31
		JNG	@@initBright0a
		MOV	EAX, 31
@@initBright0a:
		CMP	EAX, 0
		JNL	@@initBright0b
		MOV	EAX, 0
@@initBright0b:
		CMP	pixelFormatFlag, PIXFORM_RGB_555
		JNE	@@initBright1

		MOV	red_table[EDI], AX
		SHL	AX, 5
		MOV	green_table[EDI], AX
		SHL	AX, 5
		MOV	blue_table[EDI], AX
		JMP	@@initBrightEnd

@@initBright1:
		CMP	pixelFormatFlag, PIXFORM_RGB_565
		JNE	@@initBright2

		MOV	red_table[EDI], AX
		SHL	AX, 6
		MOV	green_table[EDI], AX
		SHL	AX, 5
		MOV	blue_table[EDI], AX
		JMP	@@initBrightEnd

@@initBright2:
		CMP	pixelFormatFlag, PIXFORM_BGR_555
		JNE	@@initBright3

		MOV	blue_table[EDI], AX
		SHL	AX, 5
		MOV	green_table[EDI], AX
		SHL	AX, 5
		MOV	red_table[EDI], AX
		JMP	@@initBrightEnd

@@initBright3:	; assume PIXFORM_BGR_565
		MOV	blue_table[EDI], AX
		SHL	AX, 6
		MOV	green_table[EDI], AX
		SHL	AX, 5
		MOV	red_table[EDI], AX
		; JMP	@@initBrightEnd

@@initBrightEnd:
		ADD	EDI, 2
		POP	EAX
		INC	EAX
		CMP	EAX, 32
		JB	@@initBrightLoop2

		INC	ECX
		CMP	ECX, MAX_BRIGHTNESS_ADJUST_DEGREE
		JLE	@@initBrightLoop1

		ENDPROC
INITbright	ENDP

;--------- END OF FUNCTION INITbright -----------


;--------- BEGIN OF FUNCTION IMGbrightBar -----------
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
; int  brightness  - the brightness -32 to +32
;

IMGbrightBar          PROC   imageBuf, pitch, x1, y1, x2, y2, brightness
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
		MOV	bitmapWidth, EBX

		MOV	ECX, y2
		SUB	ECX, y1
		INC	ECX

		MOV	EDX, pitch		 ; EDX = lineDiff
		SUB	EDX, EBX		 ; lineDiff = image_width - 2*icon_width
		SUB	EDX, EBX
		MOV	lineDiff, EDX

		CLD                              ; clear direction flag for MOVSW

		;------- pixels copying loop --------;

		CALC_ADDR EDI, x1, y1, pitch            ; Get the offset to the image buffer address
		MOV	ESI, brightness
		ADD	ESI, MAX_BRIGHTNESS_ADJUST_DEGREE
		SAL	ESI, 6		; * 32 * 2

@@brightRGB555:
		CMP	local_pixel_format, PIXFORM_RGB_555
		JNE	@@brightRGB565
@@brightRGB555line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@brightRGB555pixel:
		MOVZX	EAX, word ptr [EDI]	; EAX=red component
		 MOV	EBX, EAX		; EBX=green component
		  MOV	EDX, EAX		; EDX=blue component
		AND	EAX, 1fh
		 SHR	EBX, 5
		  SHR	EDX, 10
		MOV	AX, red_table[ESI+2*EAX]
		 AND	EBX, 1fh
		  AND	EDX, 1fh
		 OR	AX, green_table[ESI+2*EBX]
		  OR	AX, blue_table[ESI+2*EDX]
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@brightRGB555pixel
		POP	ECX
		ADD	EDI, lineDiff
		DEC	ECX
		JNE	@@brightRGB555line
		JMP	@@end

@@brightRGB565:
		CMP	local_pixel_format, PIXFORM_RGB_565
		JNE	@@brightBGR555
@@brightRGB565line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@brightRGB565pixel:
		MOVZX	EAX, word ptr [EDI]	; EAX=red component
		 MOV	EBX, EAX		; EBX=green component
		  MOV	EDX, EAX		; EDX=blue component
		AND	EAX, 1fh
		 SHR	EBX, 6
		  SHR	EDX, 11
		MOV	AX, red_table[ESI+2*EAX]
		 AND	EBX, 1fh
		  AND	EDX, 1fh
		 OR	AX, green_table[ESI+2*EBX]
		  OR	AX, blue_table[ESI+2*EDX]
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@brightRGB565pixel
		POP	ECX
		ADD	EDI, lineDiff
		DEC	ECX
		JNE	@@brightRGB565line
		JMP	@@end

@@brightBGR555:
		CMP	local_pixel_format, PIXFORM_BGR_555
		JNE	@@brightBGR565
@@brightBGR555line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@brightBGR555pixel:
		MOVZX	EAX, word ptr [EDI]	; EAX=blue component
		 MOV	EBX, EAX		; EBX=green component
		  MOV	EDX, EAX		; EDX=red component
		AND	EAX, 1fh
		 SHR	EBX, 5
		  SHR	EDX, 10
		MOV	AX, blue_table[ESI+2*EAX]
		 AND	EBX, 1fh
		  AND	EDX, 1fh
		 OR	AX, green_table[ESI+2*EBX]
		  OR	AX, red_table[ESI+2*EDX]
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@brightBGR555pixel
		POP	ECX
		ADD	EDI, lineDiff
		DEC	ECX
		JNE	@@brightBGR555line
		JMP	@@end

@@brightBGR565:		; assume PIXFORM_BGR565
@@brightBGR565line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@brightBGR565pixel:
		MOVZX	EAX, word ptr [EDI]	; EAX=blue component
		 MOV	EBX, EAX		; EBX=green component
		  MOV	EDX, EAX		; EDX=red component
		AND	EAX, 1fh
		 SHR	EBX, 6
		  SHR	EDX, 11
		MOV	AX, blue_table[ESI+2*EAX]
		 AND	EBX, 1fh
		  AND	EDX, 1fh
		 OR	AX, green_table[ESI+2*EBX]
		  OR	AX, red_table[ESI+2*EDX]
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@brightBGR565pixel
		POP	ECX
		ADD	EDI, lineDiff
		DEC	ECX
		JNE	@@brightBGR565line
		; JMP	@@end

@@end:
		ENDPROC
IMGbrightBar          ENDP


;---------- END OF FUNCTION IMGbrightBar ------------


;--------- BEGIN OF FUNCTION IMGcopyWbright -----------
;
; Draw a bar on the VGA screen
;
; Note : No border checking is made in this function.
;	 Placing an icon outside image buffer will cause serious BUG.
;
; char *imageBuf   - the pointer to the display surface buffer
; int  imgPitch    - pitch of the display surface buffer
; char *backBuf    - the pointer to the source surface buffer
; int  backPitch   - pitch of the source display surface buffer
; int  x1,y1       - the top left vertex of the bar ( note : width < 1024)
; int  x2,y2       - the bottom right vertex of the bar
; int  brightness  - the brightness -32 to +32
;

IMGcopyWbright          PROC   imageBuf, imgPitch, backBuf, backPitch, x1, y1, x2, y2, brightness
		LOCAL	bitmapWidth
		STARTPROC

		; ----- calc start of source and destination address ----;
		MOV	EAX, backBuf
		MOV	image_buf, EAX
		CALC_ADDR ESI, x1, y1, backPitch

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX
		CALC_ADDR EDI, x1, y1, imgPitch	; Get the offset to the image buffer address

		;------ calc bar width and height -----;

		MOV	AX , DS
		MOV	ES , AX

		MOV	EBX, x2
		SUB	EBX, x1
		INC	EBX
		MOV	bitmapWidth, EBX

		MOV	ECX, y2
		SUB	ECX, y1
		INC	ECX

		SUB	imgPitch,EBX		; lineDiff = image_width - 2 * icon_width
		SUB	backPitch,EBX		; lineDiff = image_width - 2 * icon_width
		SUB	imgPitch,EBX
		SUB	backPitch,EBX

		CLD                              ; clear direction flag for MOVSW

		;------- pixels copying loop --------;

		MOV	EDX, brightness
		ADD	EDX, MAX_BRIGHTNESS_ADJUST_DEGREE
		SAL	EDX, 6		; * 32 * 2

@@copyBWRGB555:
		CMP	local_pixel_format, PIXFORM_RGB_555
		JNE	@@copyBWRGB565
@@copyBWRGB555line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@copyBWRGB555pixel:
		; red component
		MOVZX	EBX, word ptr [ESI]
		AND	EBX, 1fh
		MOV AX, red_table[EDX + 2*EBX]

		; green component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 5
		AND	EBX, 1fh
		OR	AX, green_table[EDX + 2*EBX]

		; blue component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 10
		AND	EBX, 1fh
		OR	AX, blue_table[EDX + 2*EBX]

		ADD	ESI, 2
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@copyBWRGB555pixel
		POP	ECX
		ADD	ESI, backPitch
		ADD	EDI, imgPitch
		DEC	ECX
		JNE	@@copyBWRGB555line
		JMP	@@copyEnd

@@copyBWRGB565:
		CMP	local_pixel_format, PIXFORM_RGB_565
		JNE	@@copyBWBGR555
@@copyBWRGB565line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@copyBWRGB565pixel:
		; red component
		MOVZX	EBX, word ptr [ESI]
		AND	EBX, 1fh
		MOV AX, red_table[EDX + 2*EBX]

		; green component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 6
		AND	EBX, 1fh
		OR	AX, green_table[EDX + 2*EBX]

		; blue component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 11
		AND	EBX, 1fh
		OR	AX, blue_table[EDX + 2*EBX]

		ADD	ESI, 2
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@copyBWRGB565pixel
		POP	ECX
		ADD	ESI, backPitch
		ADD	EDI, imgPitch
		DEC	ECX
		JNE	@@copyBWRGB565line
		JMP	@@copyEnd


@@copyBWBGR555:
		CMP	local_pixel_format, PIXFORM_BGR_555
		JNE	@@copyBWBGR565
@@copyBWBGR555line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@copyBWBGR555pixel:
		; blue component
		MOVZX	EBX, word ptr [ESI]
		AND	EBX, 1fh
		MOV AX, blue_table[EDX + 2*EBX]

		; green component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 5
		AND	EBX, 1fh
		OR	AX, green_table[EDX + 2*EBX]

		; red component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 10
		AND	EBX, 1fh
		OR	AX, red_table[EDX + 2*EBX]

		ADD	ESI, 2
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@copyBWBGR555pixel
		POP	ECX
		ADD	ESI, backPitch
		ADD	EDI, imgPitch
		DEC	ECX
		JNE	@@copyBWBGR555line
		JMP	@@copyEnd

@@copyBWBGR565:	; assume PIXFORM_BGR_565
@@copyBWBGR565line:
		PUSH	ECX
		MOV	ECX, bitmapWidth
@@copyBWBGR565pixel:
		; blue component
		MOVZX	EBX, word ptr [ESI]
		AND	EBX, 1fh
		MOV AX, blue_table[EDX + 2*EBX]

		; green component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 6
		AND	EBX, 1fh
		OR	AX, green_table[EDX + 2*EBX]

		; red component
		MOVZX	EBX, word ptr [ESI]
		SHR	EBX, 11
		AND	EBX, 1fh
		OR	AX, red_table[EDX + 2*EBX]

		ADD	ESI, 2
		MOV	[EDI], AX
		ADD	EDI, 2
		DEC	ECX
		JNE	@@copyBWBGR565pixel
		POP	ECX
		ADD	ESI, backPitch
		ADD	EDI, imgPitch
		DEC	ECX
		JNE	@@copyBWBGR565line
		JMP	@@copyEnd

@@copyEnd:
		ENDPROC
IMGcopyWbright          ENDP


;---------- END OF FUNCTION IMGcopyWbright ------------

END
