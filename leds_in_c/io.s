.export _ledOn
.export _ledOff
.export _led
.export _millis

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

; reserving space in the zero page
.zeropage
counter:    .res 1, $00

PORT_B=$8000
PORT_B_DIR=$8002
T1_LOWER_ORDER_COUNT=$8004
T1_HIGH_ORDER_COUNT=$8005
ACR=$800B
IER=$800E

.segment "CODE"

.proc _millis: near

.segment	"CODE"
    ; set return value (A low byte, X, high byte)
    lda counter
    ldx #$00
    rts
.endproc


; ----
.export irq
.export init_counter

irq:
  bit T1_LOWER_ORDER_COUNT  ; reset VIA interrupt (achieved by reading T1 lower)
  inc counter
  rti
; exit_irq:  
;   rti


init_counter:
  lda #$00      
  sta counter       ; initialize counter to zero

  lda #%01000000
  sta ACR       ; set VIA timer T1 in continuos interrupt

  lda #%11000000
  sta IER       ; enable interrupts on VIA
  cli           ; enable interrupts on CPU

 ; The counter decrements at each clock cycle. When in reaches zero
 ; it interrupts and then restart the counter. Note the interrupt
 ; is not sent after N clock cycle, but at N + 2.
 ;
 ; Here we initialize the counter to trigger every 10.000 clock cyles (10 ms).
 ; The $2702 comes from the fact that the counter sends the interrupt 
 ; after N + 2 cycles, which means if we want to receive an interrupt
 ; every 10.000 cycles, we need to load the counter with 9998, or 
 ; $2702 in hex.
  lda #$0E
  sta T1_LOWER_ORDER_COUNT

  lda #$27
  sta T1_HIGH_ORDER_COUNT   ; load the counter and starts it
  rts




