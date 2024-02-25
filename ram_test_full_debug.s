  .setcpu "65sc02"
  .segment "VECTORS"

  .word   $eaea
  .word   INIT
  .word   $eaea

  .segment "CODE"

INIT:
; write FF in memory locations $0 to $FF
    LDX #$00
    LDA #$00
INIT_LOOP:
    ; Your code inside the loop goes here
    ; For example, you can use the X register as a counter or perform some operation
    LDA #$FF
    STA $0000,X
    ; NOP
    ; NOP
    ; NOP
    ; NOP
    ; NOP

    INX 
    LDA #$FF
    STA $0000,X
    ; NOP
    ; NOP
    ; NOP
    ; NOP
    ; NOP

    INX 
    LDA #$FF
    STA $0000,X
    ; NOP
    ; NOP
    ; NOP
    ; NOP
    ; NOP

    INX 
    LDA #$FF
    STA $0000,X
    ; NOP
    ; NOP
    ; NOP
    ; NOP
    ; NOP

    INX 
    LDA #$FF
    ; If I enable the following instruction the verification fails
    STA $0000,X
    ; NOP
    ; NOP
    ; NOP
    ; NOP
    ; NOP
; verification
VER:
; direct addressing
    LDX $0000
    CPX #$FF                ; Compare with 255
    BNE VERIFICATION_FAILED ; Branch if verification fails
    JMP VERIFICATION_SUCCESS


; store $FF in memory locations $0100 to $7FFF.
; ? means the value of the memory location referenced.
; $10 holds low byte of address.
; $11 hold high byte of address.

; START:
;     LDY #$00             ; Y = $00
;     TYA                  ; A = $00
;     STA $10              ; Memory location $10 = $00
;     LDA #$01             ; A = $01
;     STA $11              ; Memory location $11 = $01
; loop1:
;     LDA #$FF             ; A = $FF
; loop2:
;     STA ($10),Y          ; ?$11 * 256 + ?$10 + Y = A
;     INY                  ; Y = Y + 1
;     BNE loop2            ; Y <> 0 goto loop2
;     INC $11              ; $11 = $11 + 1
;     LDA $11              ; A = ?$11
;     CMP #$80             ; A = $80
;     BNE loop1            ; If yes goto loop1

; ; verification
;     LDY #$00             ; Y = $00
;     TYA                  ; A = $20
;     STA $10              ; Memory location $10 = $20
;     LDA #$01             ; A = $01
;     STA $11              ; Memory location $11 = $00
; loop1_v:
;     LDA #$00             ; A = $00
; loop2_v:    
;     LDA #$00 
;     LDA ($10),Y             ; A = ?$11 * 256 + ?$10 + Y
;     CMP #$FF                ; Compare with 255
;     BNE VERIFICATION_FAILED ; Branch if verification fails
;     INY                     ; Y = Y + 1
;     BNE loop2_v               ; Y <> 0 goto loop2
;     INC $11                 ; $11 = $11 + 1
;     LDA $11                 ; A = ?$11
;     CMP #$80                ; A = $80
;     BNE loop1_v             ; If yes goto loop1
;     JMP VERIFICATION_SUCCESS
    

VERIFICATION_SUCCESS:
    LDA #$FF   
    STA $8002
    LDA #%11000011    

    STA $8000
    JMP *

VERIFICATION_FAILED:
    LDX #$FF
    STX $8002

    LDX #%00111100    
    STX $8000
    JMP *

VERIFICATION_FAILED_IN:
    LDX #$FF
    STX $8002

    LDX #%10101010    
    STX $8000
    JMP *    