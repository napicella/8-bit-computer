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
  lda #$55
  sta $8001

  lda #$aa
  sta $8001

  jmp loop


  
