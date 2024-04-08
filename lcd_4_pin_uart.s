; issue when sending lower case characters from MCU to serial/
; Upper case characters are sent correctly but lower case are not.
;
; 01101000 h becomes
; 01111000 x


; 01101101 m becomes
; 01111101 }

; expected string
; abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ
; becomes
; abcdefgxyz{|}~pqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ


PORTB = $8000
DDRB = $8002

UART_READ = %1001000000000010
UART_WRITE = %1001000000000001
UART_DATA_AVAILABLE = %1001000000000100;$9003

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


; main:
;   lda #%00000010
;   and UART_DATA_AVAILABLE
;   bne main ; can't transfer yet cause TXE is low

;   lda message,x
;   sta UART_WRITE

;   jmp loop


wait_input:
  lda #%00000001
  and UART_DATA_AVAILABLE
  bne wait_input   ; no data available

  jmp read_uart

  ; ldx #$00 
  ; lda message,x
  ; jsr print_char

  ; ldx #$00
  ; lda message,x
  ; sta UART_WRITE

  ldx #$00
  jmp print_uart_loop
  jmp loop

ack:
  jmp print_uart
  jmp wait_input

read_uart:
  lda UART_READ
  jsr print_char

  lda #%00000001
  and UART_DATA_AVAILABLE
  bne ack   ; no data available
  jmp read_uart


loop:
  jmp loop

; print_uart_exit:
;   rts


print_uart:
  ldx #$00
print_uart_loop:
  ; lda #%00000010
  ; and UART_DATA_AVAILABLE
  ; bne print_uart_loop ; can't transfer yet cause TXE is low

  ;jsr busy_wait

  lda message,x
  beq wait_input
  sta UART_WRITE
  inx
  
  jmp print_uart_loop

;message: .byte "my string", 0
message: .asciiz "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ"
;message: .asciiz "hijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ"
;message: .asciiz "abdce"
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

busy_wait:
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop

  rts