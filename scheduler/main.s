.import _serial_writeline
.import _scheduler_irq
.import _scheduler_init
.import _spy

.data
thread_0_string: .asciiz "Thread 0!"
thread_1_string: .asciiz "Thread 1!"

.code
; the routine executed when the processor boots
_main:
    ; disable interrupt during init
    sei
    
    ; Init scheduler, passing the address of thread 1 to create the stack frame
    lda #((thread_1 >> 8) & 255)
	ldx #(thread_1 & 255)
    jsr _scheduler_init
    
    ; reenable interrupts
    cli

thread_0:
    lda #(<thread_0_string)
	ldx #(>thread_0_string)
    ; jsr _serial_writeline
    lda #$0a
    jsr _spy
    jmp thread_0

thread_1:
    lda #(<thread_1_string)
	ldx #(>thread_1_string)
    ; jsr _serial_writeline
    lda #$0b
    jsr _spy
    jmp thread_1

nmi:
    rti

.segment "VECTORS"
    .addr      nmi               ; NMI vector
    .addr      _main             ; Reset vector
    .addr      _scheduler_irq    ; IRQ/BRK vector