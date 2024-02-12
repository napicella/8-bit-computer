  .setcpu "65C02"
  .segment "VECTORS"

  .word   $eaea
  .word   init
  .word   $eaea

  .segment "CODE"

init:
  lda #$ff
  sta $8002

 loop: 
  jsr leds

  jmp loop


leds:
  pha
  lda #$55
  sta $8000

  lda #$aa
  sta $8000

  pla
  rts



  
