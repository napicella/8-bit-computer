.code

_main:
    lda #$FF
    jmp hello

hello:
    lda #$BB

.segment "VECTORS"
    .addr      $00               ; NMI vector
    .addr      _main             ; Reset vector
    .addr      $00             ; IRQ/BRK vector