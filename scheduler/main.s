.import _serial_init
.import _serial_writeline
.import _serial_writebyte
.import _scheduler_irq
.import _scheduler_init
.import _spy

.code
 thread_0_string: .byte "Thread 0!", $0D, $0A, $00
 thread_1_string: .byte "Thread 1!", $0D, $0A, $00
 

.code
; the routine executed when the processor boots
_main:
    jsr _serial_init
    
    ; disable interrupt during init
    sei
    
    ; Init scheduler, passing the address of thread 1 to create the stack frame
    lda #((thread_1 >> 8) & 255)
	ldx #(thread_1 & 255)
    jsr _scheduler_init

    ; reenable interrupts
    cli

thread_0:
    ; NOTE: _serial_writeline works correctly if the
    ; messages to print are in the code section. The 
    ; data segment needs to be understood better in 
    ; the load.cfg - I have never got how load from 
    ; ROM and run from RAM actually works

    lda #(<thread_0_string)
	ldx #(>thread_0_string)

    ; create a critical section (ie non-interruptable)
    ; around the _serial_writeline. This is achieved by
    ; disabling interrupts and enabling it after.
    sei
    jsr _serial_writeline
    cli

    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jmp thread_0

thread_1:
    lda #(<thread_1_string)
	ldx #(>thread_1_string)

    sei
    jsr _serial_writeline
    cli

    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
    jsr busy_loop
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