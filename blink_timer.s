; A version of the blink program that uses the VIA timer to delay the led on and off.
; 
; blink a single led on PORT_B. It uses the T1 VIA timer in continuos interrupt (VIA sends an interrput every time the
; the timer reaches zero) to delay  led on/led off.
; https://youtu.be/g_koa00MBLg?si=M8WQuERmXIHCiqtn&t=1500

  .setcpu "65C02"
  .segment "VECTORS"

  .word   $eaea
  .word   init
  .word   irq

  .segment "CODE"

PORT_B=$8000
PORT_B_DIR=$8002
T1_LOWER_ORDER_COUNT=$8004
T1_HIGH_ORDER_COUNT=$8005
ACR=$800B
IER=$800E

counter=$0100
toggle_time=$0101


irq:
  bit T1_LOWER_ORDER_COUNT  ; reset VIA interrupt (achieved by reading T1 lower)
  inc counter
  bne exit_irq
  inc counter + 1
exit_irq:  
  rti


init:
  lda #$00      
  sta counter       ; initialize counter to zero
  sta toggle_time   ; initialize toggle_time to zero

  lda #$ff
  sta PORT_B_DIR     ; set VIA port B as output

  lda #%01000000
  sta ACR       ; set VIA timer T1 in continuos interrupt

  lda #%11000000
  sta IER       ; enable interrupts on VIA
  cli           ; enable interrupts on CPU

  lda #$FF
  sta T1_LOWER_ORDER_COUNT

  lda #$FF
  sta T1_HIGH_ORDER_COUNT   ; load the counter and starts it


loop:
  sec 
  lda counter
  sbc toggle_time
  cmp #25
  bcc loop
  
  lda #$01
  eor PORT_B
  sta PORT_B  ; toggle LED
  lda counter
  sta toggle_time
  jmp loop
