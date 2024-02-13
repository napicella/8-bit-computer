; ---------------------------------------------------------------
; void __near__ __fastcall__ led ()
; ---------------------------------------------------------------

.export _led

.segment    "CODE"

.proc    _led: near

.segment    "CODE"

    ;jsr     pusha
    
    lda #$ff
    sta $8002
    lda #$ff
    sta $8000

    ;jsr     incsp1
    rts

.endproc