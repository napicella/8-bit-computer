; ---------------------------------------------------------------
; void __near__ __fastcall__ led ()
; ---------------------------------------------------------------

.export _ledOn
.export _ledOff

.segment    "CODE"

.proc    _ledOn: near

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
; void __near__ foo (void)
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



