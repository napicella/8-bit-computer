  ; Like the blink.s program, but it uses subroutines 
  
  .setcpu "65C02"
  .segment "VECTORS"

  .word   $eaea
  .word   init
  .word   $eaea

  .segment "CODE"

init:
  lda #$ff
  sta $8003

loop:
  jsr leds

  jmp loop


leds:
  pha
  lda #$FF
  sta $8001

  jsr WAIT

  lda #$00
  sta $8001

  jsr WAIT

  pla
  rts


WAIT:
  PHX
  LDX #255        ; Set loop counter to 100
WAIT_LOOP: 
  JSR NOP_LOOP
  DEX             ; Decrement X register
  BNE WAIT_LOOP  ; Branch to LOOP if X is not zero
  PLX
  RTS             ; Return from subroutine

; Subroutine to run a loop with NOP operation
NOP_LOOP:
  PHX
  LDX #255        ; Set loop counter to 100
COUNT_LOOP: 
  DEX             ; Decrement X register
  BNE COUNT_LOOP  ; Branch to LOOP if X is not zero
  PLX
  RTS             ; Return from subroutine