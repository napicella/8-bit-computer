;
; File generated by cc65 v 2.19 - Git 4bde3afd8
;
	.fopt		compiler,"cc65 v 2.19 - Git 4bde3afd8"
	.setcpu		"65SC02"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	on
	.importzp	sp, sreg, regsave, regbank
	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
	.macpack	longbranch
	.dbg		file, "main_lcd.c", 806, 1712770605
	.dbg		file, "/home/ANT.AMAZON.COM/napicell/github/cc65/include/stdint.h", 6196, 1707744857
	.dbg		file, "/home/ANT.AMAZON.COM/napicell/github/cc65/include/stdio.h", 6999, 1707744857
	.dbg		file, "/home/ANT.AMAZON.COM/napicell/github/cc65/include/stdlib.h", 6948, 1707744857
	.forceimport	__STARTUP__
	.import		_itoa
	.import		_lcd_init
	.import		_lcd_print
	.import		_lcd_clear
	.import		_counter_init
	.import		_millis
	.import		_serial_write
	.export		_lastcalled
	.export		_current
	.export		_delay
	.export		_main

.segment	"DATA"

_lastcalled:
	.dword	$00000000
_current:
	.dword	$00000000

; ---------------------------------------------------------------
; unsigned char __near__ delay (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_delay: near

	.dbg	func, "delay", "00", static, "_delay"

.segment	"CODE"

	.dbg	line, "main_lcd.c", 19
	jsr     _millis
	sta     _current
	stx     _current+1
	ldy     sreg
	sty     _current+2
	ldy     sreg+1
	sty     _current+3
	.dbg	line, "main_lcd.c", 20
	lda     _current+3
	sta     sreg+1
	lda     _current+2
	sta     sreg
	ldx     _current+1
	lda     _current
	jsr     pusheax
	lda     _lastcalled+3
	sta     sreg+1
	lda     _lastcalled+2
	sta     sreg
	ldx     _lastcalled+1
	lda     _lastcalled
	jsr     tossubeax
	cmp     #$65
	txa
	sbc     #$00
	lda     sreg
	sbc     #$00
	lda     sreg+1
	sbc     #$00
	lda     #$00
	ldx     #$00
	rol     a
	jeq     L0002
	.dbg	line, "main_lcd.c", 21
	lda     _current+3
	sta     sreg+1
	lda     _current+2
	sta     sreg
	ldx     _current+1
	lda     _current
	sta     _lastcalled
	stx     _lastcalled+1
	ldy     sreg
	sty     _lastcalled+2
	ldy     sreg+1
	sty     _lastcalled+3
	.dbg	line, "main_lcd.c", 22
	ldx     #$00
	lda     #$01
	jmp     L0001
	.dbg	line, "main_lcd.c", 24
L0002:	ldx     #$00
	lda     #$00
	jmp     L0001
	.dbg	line, "main_lcd.c", 25
L0001:	rts

	.dbg	line
.endproc

; ---------------------------------------------------------------
; void __near__ main (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_main: near

	.dbg	func, "main", "00", static, "_main"
	.dbg	sym, "time", "00", auto, -4
	.dbg	sym, "buf", "00", auto, -14

.segment	"CODE"

	.dbg	line, "main_lcd.c", 28
	lda     #$00
	sta     sreg+1
	lda     #$00
	sta     sreg
	lda     #$00
	ldx     #$00
	jsr     pusheax
	.dbg	line, "main_lcd.c", 31
	ldy     #$0A
	jsr     subysp
	jsr     _counter_init
	.dbg	line, "main_lcd.c", 32
	jsr     _lcd_init
	.dbg	line, "main_lcd.c", 34
	jmp     L0005
	.dbg	line, "main_lcd.c", 35
L0002:	jsr     _delay
	tax
	jeq     L0005
	.dbg	line, "main_lcd.c", 36
	lda     #$00
	sta     sreg+1
	lda     #$00
	sta     sreg
	lda     #$01
	ldx     #$00
	ldy     #$0A
	jsr     laddeqysp
	.dbg	line, "main_lcd.c", 37
	jsr     _lcd_clear
	.dbg	line, "main_lcd.c", 38
	ldy     #$0B
	jsr     ldaxysp
	jsr     pushax
	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$0A
	jsr     _itoa
	.dbg	line, "main_lcd.c", 39
	lda     sp
	ldx     sp+1
	jsr     _lcd_print
	.dbg	line, "main_lcd.c", 40
	lda     sp
	ldx     sp+1
	jsr     _serial_write
	.dbg	line, "main_lcd.c", 34
L0005:	jmp     L0002
	.dbg	line, "main_lcd.c", 43
	ldy     #$0E
	jsr     addysp
	rts

	.dbg	line
.endproc

