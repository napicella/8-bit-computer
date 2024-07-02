.import _counter_init
.import _counter_reset
.import _spy

.code

_main:
    cli ; enable interrupts
    jsr _counter_init
loop:
    jmp loop

nmi:
    rti

irq_handler:
    jsr _spy
    jsr _counter_reset
    rti

.segment "VECTORS"
    .addr      nmi               ; NMI vector
    .addr      _main             ; Reset vector
    .addr      irq_handler       ; IRQ/BRK vector