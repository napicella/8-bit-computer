.import _serial_init
.import _serial_writeline
.import _serial_writebyte
.import _scheduler_irq
.import _scheduler_init
.import _spy

.data
thread_0_string: .asciiz "Thread 0!"
thread_1_string: .asciiz "Thread 1!"

.code
; the routine executed when the processor boots
_main:
    jsr _serial_init

    lda #$41
    jsr _serial_writebyte
    
    ; disable interrupt during init
    sei
    
    ; Init scheduler, passing the address of thread 1 to create the stack frame
    lda #((thread_1 >> 8) & 255)
	ldx #(thread_1 & 255)
    jsr _scheduler_init

    ; reenable interrupts
    cli

thread_0:
    ; lda #(<thread_0_string)
	; ldx #(>thread_0_string)
    ; TODO: looks like _serial_writeline does not work.
    ; It's weird because the same function is used in the 
    ; shell project - but there the C runtime calls it
    ;
    ; jsr _serial_writeline
    lda #$41
    jsr _serial_writebyte
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop

    ;lda #$0a
    ;jsr _spy
    jmp thread_0

thread_1:
    ; lda #(<thread_1_string)
	; ldx #(>thread_1_string)
    ; jsr _serial_writeline
    lda #$42
    jsr _serial_writebyte
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    ;lda #$0b
    ;jsr _spy
    jmp thread_1

busy_loop:
    ldy #$FF
busy_loop_internal:    
    beq exit_loop
    dey
    jmp busy_loop_internal

exit_loop:
    rts

nmi:
    rti

.segment "VECTORS"
    .addr      nmi               ; NMI vector
    .addr      _main             ; Reset vector
    .addr      _scheduler_irq    ; IRQ/BRK vector