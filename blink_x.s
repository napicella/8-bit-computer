  .setcpu "65C02"
  .segment "VECTORS"

  .word   $eaea
  .word   init
  .word   $eaea

  .segment "CODE"

init:
  lda #$01
  sta %1110000000000000

loop: 
  lda #$55
  sta %0000000000000111

  jmp loop


  
