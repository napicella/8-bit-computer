  .setcpu "65C02"

  .export __STARTUP__ : absolute = 1

  .segment "VECTORS"

  .word   $eaea
  .word   init
  .word   $eaea

  .import _main

  .segment "STARTUP"

init:
      ;cld
      ldx #$FF
      TXS

run:
      jsr _main
      jsr run