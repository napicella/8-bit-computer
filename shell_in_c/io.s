.export _ledOn
.export _ledOff
.export _led

PORT_A = $8001
PORT_A_DIR = $8003

.segment    "CODE"
.proc    _ledOn: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ ledOn (void)
; ---------------------------------------------------------------    
    lda #$ff
    sta PORT_A_DIR
    lda #$ff
    sta PORT_A
    rts
.endproc


; ---------------------------------------------------------------
; void __near__ __fastcall__ ledOff (void)
; ---------------------------------------------------------------

.segment	"CODE"
.proc	_ledOff: near    
    lda #$ff
    sta PORT_A_DIR
    lda #$00
    sta PORT_A
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
    stx PORT_A_DIR
    sta PORT_A
    rts
.endproc


