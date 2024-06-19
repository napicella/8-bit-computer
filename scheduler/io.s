; Simple led drivers. Leds need to be connected to the VIA PORTA
.export _ledOn
.export _ledOff
.export _led

.segment    "CODE"
.proc    _ledOn: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ ledOn (void)
; ---------------------------------------------------------------    
    lda #$ff
    sta VIA_DDRA
    lda #$ff
    sta VIA_PORTA
    rts
.endproc


; ---------------------------------------------------------------
; void __near__ __fastcall__ ledOff (void)
; ---------------------------------------------------------------

.segment	"CODE"
.proc	_ledOff: near    
    lda #$ff
    sta VIA_DDRA
    lda #$00
    sta VIA_PORTA
    rts
.endproc



; ---------------------------------------------------------------
; void __near__ __fastcall__ led (uint8_t)
; ---------------------------------------------------------------

; in a fast call the right most parameter is in the accumulator
;
; since we do not change the accumulator value we do not need to
; to store it in memory 

.segment	"CODE"
.proc	_led: near    
    ldx #$ff
    stx VIA_DDRA
    sta VIA_PORTA
    rts
.endproc


