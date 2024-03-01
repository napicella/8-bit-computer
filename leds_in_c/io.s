.export _ledOn
.export _ledOff
.export _led

.segment    "CODE"

.proc    _ledOn: near


; ---------------------------------------------------------------
; void __near__ __fastcall__ ledOn (void)
; ---------------------------------------------------------------
.segment    "CODE"

    ;jsr     pusha
    
    lda #$ff
    sta $8002
    lda #$ff
    sta $8000

    ;jsr     incsp1
    rts

.endproc


; ---------------------------------------------------------------
; void __near__ __fastcall__ ledOff (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_ledOff: near

.segment	"CODE"

    ;jsr     pusha
    
    lda #$ff
    sta $8002
    
    lda #$00
    sta $8000

    ;jsr     incsp1
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

.segment	"CODE"

    ; sta _led_data
    
    ldx #$ff
    stx $8002
    
    ; lda _led_data
    sta $8000

    rts

.endproc





