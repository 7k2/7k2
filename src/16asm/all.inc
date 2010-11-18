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

;Filename    : all.inc
;Description : General purpose include file

.386
.MODEL FLAT, STDCALL


;--------- BEGIN OF MARCO STARTPROC ----------
;
; Codes placed at the beginning of a function
;
; Syntax : STARTPROC
;
STARTPROC       MACRO
		PUSH    EBX
		PUSH    ESI
		PUSH    EDI
		ENDM

;----------- END OF MARCO STARTPROC ---------


;-------- BEGIN OF MARCO ENDPROC -----------
;
; Codes placed at the ending of a function
;
ENDPROC         MACRO
		POP  EDI
		POP  ESI
		POP  EBX
		RET
		ENDM

;----------- END OF MARCO ENDPROC ------------

