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
