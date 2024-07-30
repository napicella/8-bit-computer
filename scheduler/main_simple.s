.import _serial_init
.import _serial_writeline

.code
message: .asciiz "Hello mom!"
; the routine executed when the processor boots
_main:
    jsr _serial_init

thread_0:
    lda #(<message)
	ldx #(>message)
    jsr _serial_writeline
    
loop: 
    jmp loop


nmi:
    rti

_handle_irq:
    rti

.segment "VECTORS"
    .addr      nmi               ; NMI vector
    .addr      _main             ; Reset vector
    .addr      _handle_irq       ; IRQ/BRK vector