.segment "CODE"

.export _scheduler_init
.export _scheduler_irq
.import _counter_init
.import _counter_reset
.import _spy

; reserving space for counter in the zero page
.zeropage
thread_running:     .res 1, $00
ctx_switch_mem_t0:  .res 4, $00
ctx_switch_mem_t1:  .res 4, $00
temp:               .res 2, $00

sp_scheduler_init:  .res 1, $00

.code

; defining the symbols makes it easier to debug
ctx_switch_mem_t0_0: .addr ctx_switch_mem_t0
ctx_switch_mem_t0_1: .addr ctx_switch_mem_t0 + 1
ctx_switch_mem_t0_2: .addr ctx_switch_mem_t0 + 2
ctx_switch_mem_t0_3: .addr ctx_switch_mem_t0 + 3

ctx_switch_mem_t1_0: .addr ctx_switch_mem_t1
ctx_switch_mem_t1_1: .addr ctx_switch_mem_t1 + 1
ctx_switch_mem_t1_2: .addr ctx_switch_mem_t1 + 2
ctx_switch_mem_t1_3: .addr ctx_switch_mem_t1 + 3

; routine assumes that the address of thread 1 is in register A (high byte) and X (low byte)
_scheduler_init:
    ; save the input to memory since we need the registers
    sta temp
    stx temp + 1

    tsx
    stx sp_scheduler_init

    ; initialize stack pointers variables
    ;   for thread 0
    ldx #$ff 
    stx ctx_switch_mem_t0 + 3
    ;   for thread 1
    ldx #$80
    stx ctx_switch_mem_t1 + 3
    
    ; We assume thread 0 is the first one to run.
    ; When receiving the first interrupt the scheduler assumes the stack for thread 1 is populated.
    ; That it is not true the firt time, so we set it up manually.

    ; move stack pointer to thread one
    ldx ctx_switch_mem_t1 + 3
    txs

    ; store the address and the cpu status flags
    ;
    ; address
    lda temp
    pha 
    lda temp + 1
    pha 
    ;
    ; cpu status flags
    ; Note that we set it to all zero. It's particularly
    ; important that the Interrupt inhibit (third bit, LS) is
    ; zero, otherwise theread 1 would start with interrupts
    ; disabled
    lda #$0
    pha

    ; store the update stack pointer in memory
    tsx
    stx ctx_switch_mem_t1 + 3

    ; reset stack pointer
    ldx sp_scheduler_init
    txs

    ; init counter
    jsr _counter_init
    ; set thread 0 as running
    lda #$0
    sta thread_running

    rts



_scheduler_irq:
    ; disable interrupts
    sei                                             
    
    ; reset counter
    jsr _counter_reset

    ; check which thread is running
    lda thread_running
    cmp #$0
    ; thread 0 is running, swith to thread 1
    beq switch_thread_1

    ; thread 1 was running, falling through switching
    ; to thread 0
    
switch_thread_0:
    ; save registers in memory for thread 1 (a, x, y, stack_pointer)
    sta ctx_switch_mem_t1
    stx ctx_switch_mem_t1 + 1
    sty ctx_switch_mem_t1 + 2
    tsx
    stx ctx_switch_mem_t1 + 3

    ; updating the stack pointer to point to thread 0
    ldx ctx_switch_mem_t0 + 3
    txs

    ; update the thread running
    lda #$0
    sta thread_running

    ; hydrate the registers
    lda ctx_switch_mem_t0
    ldx ctx_switch_mem_t0 + 1
    ldy ctx_switch_mem_t0 + 2

    ; rti also re-enables interrupts
    rti


switch_thread_1:
    ; save registers in memory for thread 0 (a, x, y, stack_pointer)
    sta ctx_switch_mem_t0
    stx ctx_switch_mem_t0 + 1
    sty ctx_switch_mem_t0 + 2
    tsx
    stx ctx_switch_mem_t0 + 3

    ; updating the stack pointer to point to thread 1
    ldx ctx_switch_mem_t1 + 3
    txs

    ; update the thread running
    lda #$1
    sta thread_running

    ; hydrate the registers
    lda ctx_switch_mem_t1
    ldx ctx_switch_mem_t1 + 1
    ldy ctx_switch_mem_t1 + 2
    
    ; rti also re-enables interrupts
    rti

exit_irq:
    ; rti also re-enables interrupts
    rti