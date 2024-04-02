.export _millis
.export _counter_init
.export _counter_irq

PORT_B=$8000
PORT_B_DIR=$8002
T1_LOWER_ORDER_COUNT=$8004
T1_HIGH_ORDER_COUNT=$8005
ACR=$800B
IER=$800E


; reserving space for counter in the zero page
.zeropage
counter:    .res 1, $00

.segment "CODE"
.proc _millis: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ millis () uint8_t
; ---------------------------------------------------------------

    ; set return value (A low byte, X, high byte)
    lda counter
    ldx #$00
    rts
.endproc


.segment "CODE"
.proc _counter_init: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ counter_init ()
; ---------------------------------------------------------------
    jsr counter_init
    rts
.endproc


counter_init:
  lda #$00      
  sta counter       ; initialize counter to zero

  lda #%01000000
  sta ACR           ; set VIA timer T1 in continuos interrupt

  lda #%11000000
  sta IER           ; enable interrupts on VIA
  cli               ; enable interrupts on CPU

 ; The counter decrements at each clock cycle. When in reaches zero
 ; it interrupts and then restart the counter. Note the interrupt
 ; is not sent after N clock cycle, but at N + 2.
 ;
 ; Here we initialize the counter to trigger every 10.000 clock cyles (10 ms).
 ; The $270E comes from the fact that the counter sends the interrupt 
 ; after N + 2 cycles, which means if we want to receive an interrupt
 ; every 10.000 cycles, we need to load the counter with 9998, or 
 ; $270E in hex.
  lda #$0E
  sta T1_LOWER_ORDER_COUNT

  lda #$27
  sta T1_HIGH_ORDER_COUNT   ; load the counter and starts it
  rts

_counter_irq:
  bit T1_LOWER_ORDER_COUNT  ; reset VIA interrupt (achieved by reading T1 lower).
                            ; This makes the counter restart
  inc counter
  rts