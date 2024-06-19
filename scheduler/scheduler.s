.segment "CODE"

.export _scheduler_init
.export _scheduler_irq
.import _counter_init
.import _counter_reset

; reserving space for counter in the zero page
.zeropage
thread_running:     .res 1, $00
ctx_switch_mem_t0:  .res 4, $00
ctx_switch_mem_t1:  .res 4, $00
temp:               .res 2, $00

.code

; routine assumes that the address of thread 1 is in register A (high byte) and X (low byte)
_scheduler_init:
    jsr _counter_init
    ; save the input to memory since we need the registers
    sta temp
    stx temp + 1
    ; set thread 0 as running
    lda #$0
    sta thread_running

    ; initialize stack pointers variables
    ;   for thread 0
    lda #$ff
    sta ctx_switch_mem_t0 + 3
    ;   for thread 1
    lda #$80
    sta ctx_switch_mem_t1 + 3
    
    ; We assume thread 0 is the first one to run.
    ; When receiving the first interrupt the scheduler assumes the stack for thread 1 is populated.
    ; That it is not true the firt time, so we set it up manually.

    ; move stack pointer to thread one
    ldx ctx_switch_mem_t1 + 3
    txs

    ; store the address and the cpu status
    lda temp
    pha 
    lda temp + 1
    pha 
    php

    ; store the update stack pointer in memory
    tsx
    stx ctx_switch_mem_t1 + 3

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
    lda ctx_switch_mem_t0 + 3
    txs

    ; hydrate the registers
    lda ctx_switch_mem_t0
    ldx ctx_switch_mem_t0 + 1
    ldy ctx_switch_mem_t0 + 2

    ; update the thread running
    lda #$0
    sta thread_running

    jmp exit_irq


switch_thread_1:
    ; save registers in memory for thread 0 (a, x, y, stack_pointer)
    sta ctx_switch_mem_t0
    stx ctx_switch_mem_t0 + 1
    sty ctx_switch_mem_t0 + 2
    tsx
    stx ctx_switch_mem_t0 + 3

    ; updating the stack pointer to point to thread 1
    lda ctx_switch_mem_t1 + 3
    txs

    ; hydrate the registers
    lda ctx_switch_mem_t1
    ldx ctx_switch_mem_t1 + 1
    ldy ctx_switch_mem_t1 + 2

    lda #$1
    sta thread_running

    jmp exit_irq

exit_irq:
    ; re-enable interrupts
    cli
    rti