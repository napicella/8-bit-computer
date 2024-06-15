; Connect the VIA port B (PB0 to PB7) to the LCD.
; The PB7 (most significant bit) is unused, though the code sets it as output for convenience
; PB0 -> D4
; PB1 -> D5
; PB2 -> D6
; PB3 -> D7
; PB4 -> RS
; PB5 -> RW
; PB6 -> E

.include "hardware.inc"
.export _lcd_init
.export _lcd_print
.export _lcd_clear

E  = %01000000
RW = %00100000
RS = %00010000

.zeropage

_message:    .res 2, $00      ;  Reserve a local zero page pointer


.segment    "CODE"
.proc    _lcd_init: near

; ---------------------------------------------------------------
; void __near__ __fastcall__ lcd_init (void)
; ---------------------------------------------------------------
    jsr init
    rts
.endproc

init:
  lda #%11111111 ; Set all pins on port B to output
  sta VIA_DDRB

  jsr lcd_init
  lda #%00101000 ; Set 4-bit mode; 2-line display; 5x8 font
  jsr lcd_instruction
  lda #%00001110 ; Display on; cursor on; blink off
  jsr lcd_instruction
  lda #%00000110 ; Increment and shift cursor; don't shift display
  jsr lcd_instruction
  lda #%00000001 ; Clear display
  jsr lcd_instruction

  rts


.segment    "CODE"
.proc    _lcd_print: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ lcd_print (char*)
; ---------------------------------------------------------------
    sta     _message      ;  Set zero page pointer to string address
    stx     _message+1    ;    (pointer passed in via the A/X registers)
    ldy     #00
    jsr print
    rts
.endproc


print:
  lda (_message),y        ;zero page address, see Post-Indexed Indirect, "(Zero-Page),Y"
                          ; https://www.masswerk.at/6502/6502_instruction_set.html
  beq print_exit
  jsr print_char
  iny
  jmp print

print_exit:
  rts


lcd_wait:
  pha
  lda #%11110000  ; LCD data is input
  sta VIA_DDRB
lcdbusy:
  lda #RW
  sta VIA_PORTB
  lda #(RW | E)
  sta VIA_PORTB
  lda VIA_PORTB       ; Read high nibble
  pha             ; and put on stack since it has the busy flag
  lda #RW
  sta VIA_PORTB
  lda #(RW | E)
  sta VIA_PORTB
  lda VIA_PORTB       ; Read low nibble
  pla             ; Get high nibble off stack
  and #%00001000
  bne lcdbusy

  lda #RW
  sta VIA_PORTB
  lda #%11111111  ; LCD data is output
  sta VIA_DDRB
  pla
  rts

lcd_init:
  lda #%00000010 ; Set 4-bit mode
  sta VIA_PORTB
  ora #E
  sta VIA_PORTB
  and #%00001111
  sta VIA_PORTB
  rts

lcd_instruction:
  jsr lcd_wait
  pha
  lsr
  lsr
  lsr
  lsr            ; Send high 4 bits
  sta VIA_PORTB
  ora #E         ; Set E bit to send instruction
  sta VIA_PORTB
  eor #E         ; Clear E bit
  sta VIA_PORTB
  pla
  and #%00001111 ; Send low 4 bits
  sta VIA_PORTB
  ora #E         ; Set E bit to send instruction
  sta VIA_PORTB
  eor #E         ; Clear E bit
  sta VIA_PORTB
  rts

print_char:
  jsr lcd_wait
  pha
  lsr
  lsr
  lsr
  lsr             ; Send high 4 bits
  ora #RS         ; Set RS
  sta VIA_PORTB
  ora #E          ; Set E bit to send instruction
  sta VIA_PORTB
  eor #E          ; Clear E bit
  sta VIA_PORTB
  pla
  and #%00001111  ; Send low 4 bits
  ora #RS         ; Set RS
  sta VIA_PORTB
  ora #E          ; Set E bit to send instruction
  sta VIA_PORTB
  eor #E          ; Clear E bit
  sta VIA_PORTB
  rts


.segment    "CODE"
.proc    _lcd_clear: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ lcd_clear (void)
; ---------------------------------------------------------------
    jsr lcd_clear
    rts
.endproc

lcd_clear:
    lda #%00000001 ; Clear display
    jsr lcd_instruction
    lda #%00000010 ; Return home
    jsr lcd_instruction
    rts