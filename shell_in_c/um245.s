.export _serial_writebyte
.export _serial_writeline
.export _serial_readline
.export _serial_readbyte

UART_WRITE  = %1001000000000001
UART_READ   = %1001000000000010
UART_STATUS = %1001000000000100

UART_WRITE_BUSY_MASK   = %00000001
UART_READ_NO_DATA_MASK = %00000010


; ---------------------------------------------------------------
; void __near__ __fastcall__ serial_writeline (char*)
; ---------------------------------------------------------------
.zeropage
; Reserve a local zero page pointer to store the char*
; which points to the data to write to the serial
_data:    .res 2, $00

.segment    "CODE"
.proc _serial_writeline: near
  sta _data       ;  Set zero page pointer to string address
  stx _data+1     ;    (pointer passed in via the A/X registers)
  ldy #00
  jsr uart_write_line
  rts
.endproc

uart_write_line:
uart_write_line_loop:
  lda #UART_WRITE_BUSY_MASK
  and UART_STATUS
  bne uart_write_line_loop   ; can't transfer yet cause TXE is high
  lda (_data),y              ;zero page address, see Post-Indexed Indirect, "(Zero-Page),Y"
                             ; https://www.masswerk.at/6502/6502_instruction_set.html
  beq uart_write_line_loop_exit
  sta UART_WRITE
  iny
  jmp uart_write_line_loop
  
  rts

uart_write_line_loop_exit:
  rts


; ---------------------------------------------------------------
; void __near__ __fastcall__ serial_writebyte (char)
; ---------------------------------------------------------------
.segment    "CODE"
.proc _serial_writebyte: near
  jsr uart_write_byte
  rts
.endproc

uart_write_byte:
uart_write_byte_loop:
  pha
  lda #UART_WRITE_BUSY_MASK
  and UART_STATUS
  bne uart_write_byte_loop        ; can't transfer yet cause TXE is high
  pla
  sta UART_WRITE
  rts


; ---------------------------------------------------------------
; void __near__ __fastcall__ serial_readline (char*)
; ---------------------------------------------------------------
.zeropage
; Reserve a local zero page pointer to store the char*
; where the data read should be stored in memory
_data_read:  .res 2, $00 ;  

.segment    "CODE"
.proc    _serial_readline: near
  sta _data       ;  Set zero page pointer to string address
  stx _data+1     ;    (pointer passed in via the A/X registers)
  ldy #00
  jsr uart_read
  rts
.endproc

uart_read:
uart_read_loop:
  lda #UART_READ_NO_DATA_MASK
  and UART_STATUS
  bne uart_read_loop        ; can't read, no data

  lda UART_READ
  beq read_uart_loop_exit   ; exit if byte received is the zero value

  sta (_data_read),y        ;zero page address, see Post-Indexed Indirect, "(Zero-Page),Y"
                            ; https://www.masswerk.at/6502/6502_instruction_set.html
  iny
  jmp uart_read_loop

read_uart_loop_exit:
  rts


; ---------------------------------------------------------------
; void __near__ __fastcall__ char serial_readbyte ()
; ---------------------------------------------------------------
.segment    "CODE"
.proc _serial_readbyte: near
  jsr uart_read_byte
  rts
.endproc

uart_read_byte:
uart_read_byte_loop:
  lda #UART_READ_NO_DATA_MASK
  and UART_STATUS
  bne uart_read_byte_loop        ; can't read, no data

  ; set return value 
  ; low word  : (A low byte, X high byte)
  lda UART_READ
  ldx #$00
  rts