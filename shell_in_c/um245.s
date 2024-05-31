; General note on UM245R. Despite what the Datasheet says, both WR# and RD# need to be held normally.
; high and strobed low/high to write and read.
; For example for a write, stobe WE# low, then high. On the rising edge, the data is written to the UM245R.
; Time ->
;     __|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
; WE#  _________               ________
;               |             |
;               |             |
;               |_____________|
;
;
; ----------------------------------------------------------------------------------------------------
; Driver for UM245R USB Parallel FIFO that is connected to the 65C22 VIA interface.
;
;
; Driver expects the UM245R USB to be connected to the VIA as follows:
; VIA_PORT_B -> UM245R_DATA
; VIA_PORT_A [A0] -> UM245R_WE#
; VIA_PORT_A [A1] -> UM245R_RD#
; VIA_PORT_A [A2] -> UM245R_TXE
; VIA_PORT_A [A3] -> UM245R_RXE

.export _serial_init
.export _serial_writebyte
.export _serial_writeline
.export _serial_readline
.export _serial_readbyte


PORTB = $8000
PORTA = $8001
DDRB  = $8002
DDRA  = $8003

UART_WRITE_BUSY_MASK   = %00000100
UART_READ_NO_DATA_MASK = %00001000


; ---------------------------------------------------------------
; void __near__ __fastcall__ serial_init (void)
; ---------------------------------------------------------------
.segment    "CODE"
.proc _serial_init: near
  lda #$FF                        ; set PORTB as output
  sta DDRB
  lda #$00
  sta PORTB

  lda #%00000011   ; set PORTA first and second bit to output (connected W# and R# on UM245R)
  ora DDRA
  sta DDRA
  lda #%00000011   ; W# is stable high, R# is stable high
  ora PORTA
  sta PORTA
  rts
.endproc


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
  ldy #$00
  jsr uart_write_line
  rts
.endproc

uart_write_line:
  lda #$FF                        ; set PORTB as output
  sta DDRB
uart_write_line_loop:
  lda #UART_WRITE_BUSY_MASK
  and PORTA
  bne uart_write_line_loop        ; can't write, um245 busy

  lda (_data),y                   ; zero page address, see Post-Indexed Indirect, "(Zero-Page),Y"
                                  ; https://www.masswerk.at/6502/6502_instruction_set.html
  beq uart_write_line_loop_exit   ; if NULL byte exit
                                  ; otherwise
  sta PORTB                       ; store the byte to PORTB 
  

  lda #%11111110                   ; strobe W# (low)
  and PORTA
  sta PORTA

  lda #%00000001                   ; (high)
  ora PORTA
  sta PORTA

  iny
  bne uart_write_line_loop   ; If Y hasn't wrapped around, continue loop
  inc _data+1                ; If Y wrapped around, increment the high byte of the address
  jmp uart_write_line_loop

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
  pha
  lda #$FF                        ; set PORTB as output
  sta DDRB
uart_write_byte_loop:
  lda #UART_WRITE_BUSY_MASK
  and PORTA
  bne uart_write_byte_loop        ; can't write, um245 busy

  pla
  sta PORTB                       ; store the byte to PORTB 

  lda #%11111110                   ; strobe W#
  and PORTA
  sta PORTA

  lda #%00000001
  ora PORTA
  sta PORTA

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
  sta _data_read       ;  Set zero page pointer to string address
  stx _data_read+1     ;    (pointer passed in via the A/X registers)
  ldy #$00
  jsr uart_read
  rts
.endproc

uart_read:
uart_read_loop:
  lda #UART_READ_NO_DATA_MASK
  and PORTA
  bne uart_read_loop        ; can't read, no data

  lda #%11111101            ; strobe R#
  and PORTA
  sta PORTA

  lda PORTB
  pha

  lda #%00000010
  ora PORTA
  sta PORTA

  pla
  beq read_uart_loop_exit   ; exit if byte received is the zero value

  sta (_data_read),y        ;zero page address, see Post-Indexed Indirect, "(Zero-Page),Y"
                            ; https://www.masswerk.at/6502/6502_instruction_set.html

  iny
  bne uart_read_loop         ; If Y hasn't wrapped around, continue loop
  inc _data_read+1           ; If Y wrapped around, increment the high byte of the address
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
  lda #%00000000  ; set PORTB as input
  sta DDRB
uart_read_byte_loop:
  lda #UART_READ_NO_DATA_MASK
  and PORTA
  bne uart_read_byte_loop        ; can't read, no data

  lda #%11111101                 ; strobe R#
  and PORTA
  sta PORTA

  lda PORTB
  pha

  lda #%00000010
  ora PORTA
  sta PORTA

  pla

  ; set return value 
  ; low word  : (A low byte, X high byte)
  ldx #$00
  rts