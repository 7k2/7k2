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

;Filename    : IMGFUN.INC
;Description : Include file for image buffer processing functions


INCLUDE ALL.INC

;---------------------------------------------------

.DATA

; extrn image_buf:DWORD, image_width:DWORD, image_height:DWORD
extrn image_buf:DWORD

;---------------------------------------------------


;------- BEGIN OF MARCO CALC_ADDR -----------
;
; Calculate address of the image buffer of the specified line
;
; <destReg>    the destination register for restoring result offset
; <posX>       the x position
; <posY>       the y position
; <pitch>      pitch of the display surface buffer
;
CALC_ADDR       MACRO   destReg, posX, posY, pitch
		PUSH    EAX
		PUSH    EDX

		MOV	EAX, pitch
		MUL     posY
		ADD	EAX, posX
		ADD	EAX, posX
		ADD	EAX, image_buf

		MOV	destReg, EAX

		POP     EDX
		POP	EAX
		ENDM

;--------- END OF MARCO CALC_ADDR -------------


;------- BEGIN OF MARCO CALC_ADDR_2 -----------
;
; Calculate address of the image buffer of the specified line.
;
; For IMGbltArea() and IMGbltAreaTrans() only.
;
; <destReg>    the destination register for restoring result offset
; <posX>       the x position
; <posY>       the y position
;
CALC_ADDR_2     MACRO   destReg, posX, posY, offsetX, offsetY, pitch
		PUSH    EAX
		PUSH    EDX

		MOV	EAX, posY
		ADD	EAX, offsetY
		MUL pitch

		MOV	EDX, posX
		ADD	EDX, offsetX
		LEA	EAX, [EAX + 2*EDX]
		ADD	EAX, image_buf

		MOV	destReg, EAX

		POP     EDX
		POP	EAX
		ENDM

;--------- END OF MARCO CALC_ADDR_2 -------------

