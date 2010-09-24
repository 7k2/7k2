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

; Filename    : EFFECT.INC
; Description : Assembly header for IMGeffect


PIXFORM_RGB_555 = 0	; red on low bits
PIXFORM_RGB_565 = 1
PIXFORM_BGR_555 = 2	; blue on low bits
PIXFORM_BGR_565 = 3

; INITeffect	PROTO	STDCALL	pixelFormatFlag

; AL is effect id, change the word at [EDI]
IMGeffect	PROTO	STDCALL		; see I_EFFECT.ASM

		EXTERNDEF	pixel_format_flag:DWORD
		EXTERNDEF	red_mask:WORD, green_mask:WORD, blue_mask:WORD
		EXTERNDEF	red_shift:BYTE, green_shift:BYTE, blue_shift:BYTE
		EXTERNDEF	log_alpha_mask:WORD
