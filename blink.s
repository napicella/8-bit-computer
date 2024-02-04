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
  lda #$55
  sta $8000

  lda #$aa
  sta $8000

  jmp loop


  
