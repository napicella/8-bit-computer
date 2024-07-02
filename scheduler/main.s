.import _counter_init
.import _spy

.code

_main:
   jsr _counter_init
loop:
    jmp loop

nmi:
    rti

irq_handler:
    jsr _spy
    rti

.segment "VECTORS"
    .addr      nmi               ; NMI vector
    .addr      _main             ; Reset vector
    .addr      irq_handler       ; IRQ/BRK vector