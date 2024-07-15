.import _counter_init
.import _spy
.import VIA_T1CL

.code
; the routine executed when the processor boots
_main:
    ; disable interrupt during init
    sei
    jsr _counter_init
    ; reenable interrupts
    cli

thread_0:
    lda #$0a
    jsr _spy
    jmp thread_0


nmi:
    rti

_handle_irq:
    nop
    lda VIA_T1CL
    rti

.segment "VECTORS"
    .addr      nmi               ; NMI vector
    .addr      _main             ; Reset vector
    .addr      _handle_irq       ; IRQ/BRK vector