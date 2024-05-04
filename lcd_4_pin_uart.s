PORTB = $8000
DDRB = $8002

UART_WRITE  = %1000010000000001
UART_READ   = %1000010000000010
UART_STATUS = %1000010000000100
UART_WRITE_BUSY_MASK   = %00000001
UART_READ_NO_DATA_MASK = %00000010

E  = %01000000
RW = %00100000
RS = %00010000

  .setcpu "65C02"
  .segment "VECTORS"

  .word   $eaea
  .word   init
  .word   $eaea


  .segment "CODE"

init:
  ldx #$ff
  txs

  lda #%11111111 ; Set all pins on port B to output
  sta DDRB

  jsr lcd_init
  lda #%00101000 ; Set 4-bit mode; 2-line display; 5x8 font
  jsr lcd_instruction
  lda #%00001110 ; Display on; cursor on; blink off
  jsr lcd_instruction
  lda #%00000110 ; Increment and shift cursor; don't shift display
  jsr lcd_instruction
  lda #%00000001 ; Clear display
  jsr lcd_instruction

  ldx #$00


wait_input:
  lda #UART_READ_NO_DATA_MASK
  and UART_STATUS
  bne wait_input   ; no data available

  jmp read_uart
  ldx #$00
  jmp print_uart_loop
  jmp loop

ack:
  jmp print_uart
  jmp wait_input

read_uart:
  lda UART_READ
  jsr print_char

  lda #UART_READ_NO_DATA_MASK
  and UART_STATUS
  bne ack   ; no data available
  jmp read_uart


loop:
  jmp loop


print_uart:
  ldx #$00
print_uart_loop:
  lda #UART_WRITE_BUSY_MASK
  and UART_STATUS
  bne print_uart_loop ; can't transfer yet cause TXE is high

  lda message,x
  beq wait_input
  sta UART_WRITE
  inx
  
  jmp print_uart_loop


message: .asciiz "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ"
;message: .byte "my string", 0
;  message: .byte %00000001
; .byte %00000010
; .byte %00000100
; .byte %00001000
; .byte %00010000
; .byte %00100000
; .byte %01000000
; .byte %10000000
; .byte %01101000  ; this is the lower case h
; .byte %01101101  ; this is the lower case m
; .byte %00000000

lcd_wait:
  pha
  lda #%11110000  ; LCD data is input
  sta DDRB
lcdbusy:
  lda #RW
  sta PORTB
  lda #(RW | E)
  sta PORTB
  lda PORTB       ; Read high nibble
  pha             ; and put on stack since it has the busy flag
  lda #RW
  sta PORTB
  lda #(RW | E)
  sta PORTB
  lda PORTB       ; Read low nibble
  pla             ; Get high nibble off stack
  and #%00001000
  bne lcdbusy

  lda #RW
  sta PORTB
  lda #%11111111  ; LCD data is output
  sta DDRB
  pla
  rts

lcd_init:
  lda #%00000010 ; Set 4-bit mode
  sta PORTB
  ora #E
  sta PORTB
  and #%00001111
  sta PORTB
  rts

lcd_instruction:
  jsr lcd_wait
  pha
  lsr
  lsr
  lsr
  lsr            ; Send high 4 bits
  sta PORTB
  ora #E         ; Set E bit to send instruction
  sta PORTB
  eor #E         ; Clear E bit
  sta PORTB
  pla
  and #%00001111 ; Send low 4 bits
  sta PORTB
  ora #E         ; Set E bit to send instruction
  sta PORTB
  eor #E         ; Clear E bit
  sta PORTB
  rts

print_char:
  jsr lcd_wait
  pha
  lsr
  lsr
  lsr
  lsr             ; Send high 4 bits
  ora #RS         ; Set RS
  sta PORTB
  ora #E          ; Set E bit to send instruction
  sta PORTB
  eor #E          ; Clear E bit
  sta PORTB
  pla
  and #%00001111  ; Send low 4 bits
  ora #RS         ; Set RS
  sta PORTB
  ora #E          ; Set E bit to send instruction
  sta PORTB
  eor #E          ; Clear E bit
  sta PORTB
  rts