.export _serial_write
.export _serial_read
.export _serial_read_byte

UART_WRITE  = %1001000000000001
UART_READ   = %1001000000000010
UART_STATUS = %1001000000000100

UART_WRITE_BUSY_MASK   = %00000001
UART_READ_NO_DATA_MASK = %00000010

.zeropage
_data:    .res 2, $00 ;  Reserve a local zero page pointer

.segment    "CODE"
.proc    _serial_write: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ serial_write (char)
; ---------------------------------------------------------------
  ;sta _data       ;  Set zero page pointer to string address
  ;stx _data+1     ;    (pointer passed in via the A/X registers)
  ;ldy #00
  jsr print_uart
  rts
.endproc

print_uart:
print_uart_loop:
  ;lda #UART_WRITE_BUSY_MASK
  ;and UART_STATUS
  ;bne print_uart_loop        ; can't transfer yet cause TXE is high
  ;ldy #00
  ;lda (_data),y              ;zero page address, see Post-Indexed Indirect, "(Zero-Page),Y"
                             ; https://www.masswerk.at/6502/6502_instruction_set.html
  ;beq print_uart_loop_exit
  sta UART_WRITE
  ;iny
  
  ;jmp print_uart_loop
  rts

print_uart_loop_exit:
  rts


.zeropage
_value:    .res 2, $00 ;  Reserve a local zero page pointer

.segment    "CODE"
.proc    _serial_read: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ serial_read (char*)
; ---------------------------------------------------------------
  sta _data       ;  Set zero page pointer to string address
  stx _data+1     ;    (pointer passed in via the A/X registers)
  ldy #00
  jsr read_uart
  rts
.endproc

read_uart:
read_uart_loop:
  lda #UART_READ_NO_DATA_MASK
  and UART_STATUS
  bne read_uart_loop        ; can't read, no data

  lda UART_READ
  beq read_uart_loop_exit   ; exit if byte received is the zero value

  sta (_value),y            ;zero page address, see Post-Indexed Indirect, "(Zero-Page),Y"
                             ; https://www.masswerk.at/6502/6502_instruction_set.html
  iny
  jmp read_uart_loop

read_uart_loop_exit:
  rts


.segment    "CODE"
.proc    _serial_read_byte: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ char serial_read_byte ()
; ---------------------------------------------------------------
  jsr read_uart_byte
  rts
.endproc

read_uart_byte:
read_uart_byte_loop:
  lda #UART_READ_NO_DATA_MASK
  and UART_STATUS
  bne read_uart_byte_loop        ; can't read, no data

  ; set return value 
  ; low word  : (A low byte, X high byte)
  lda UART_READ
  ldx #$00
  rts