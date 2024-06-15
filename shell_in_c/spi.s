; SPI mode 1 - MOSI, MISO, CLK, CS connected to PORTA DA3, DA2, DA1, DA0
;
;
; Example With Rasberry Pico
; 
; The PICO has two SPI ports, each one can be assigned to different pins according to the datasheet.
; ```
; #define PIN_SCK 10
; #define PIN_MISO 11  // TX
; #define PIN_MOSI 12  // RX
; #define PIN_CS 13
; ```
; In slave mode, TX is MISO. In master it would be MOSI. 

					.setcpu		"6502"

					.include "hardware.inc"
					.export _spiRead
					.export _spiWrite
					.export _spiBegin
					.export _spiEnd
					.export _spiInit

					MOSI = %00001000
					MISO = %00000100
					CLK  = %00000010
					CS   = %00000001
		
					.data
temp:				.byte 00	; used for shift in/out bits via carry, put into zeropage for minor speedup

.segment    "CODE"
.proc _spiInit: near
					pha
					; set clock low, CS unchanged
					lda #CLK
					eor #$FF
					and VIA_PORTA
					ora #CS
					sta VIA_PORTA

					; set data direction (default is input, so we only set the ones that needs to be output to 1)
					lda VIA_DDRA
					ora #MOSI
					ora #CS
					ora #CLK
					sta VIA_DDRA
					
					pla
					rts
.endproc

.segment    "CODE"
.proc _spiEnd: near
					pha
					lda VIA_PORTA
					ora #CS	
					sta VIA_PORTA
					pla
					rts
.endproc					
	

.segment    "CODE"
.proc _spiBegin: near
		 			pha
					; bring CS low
					lda #CS
					eor #$FF
					and VIA_PORTA
					sta VIA_PORTA
					pla
					rts	
.endproc


.segment    "CODE"
.proc _spiRead: near
					tya
					pha
					txa
					pha
					lda #$00
					sta temp
					lda #MOSI
					ora VIA_PORTA
					sta VIA_PORTA
					ldy #8
@loop:				lda #CLK
					ora VIA_PORTA
					sta VIA_PORTA
					ldx VIA_PORTA
					lda #CLK
					eor VIA_PORTA
					sta VIA_PORTA
					clc
					txa
					and #MISO
					beq @skip
					sec
@skip:				rol temp
					dey
					bne @loop
					pla
					tax
					pla
					tay
					lda temp
					rts
.endproc


.segment    "CODE"
.proc _spiWrite: near
					sta temp
					tya
					pha
					lda temp
					ldy #8
@loop:				lda #MOSI
					asl temp  			; shift left one bit from temp
					bcc @is_low
					ora VIA_PORTA             ; send one
					sta VIA_PORTA
					jmp @is_high
@is_low:			lda	#MOSI           ; send zero
					eor #$FF
					and VIA_PORTA
					sta VIA_PORTA
@is_high:			lda #CLK            ; strobe CLK
					ora VIA_PORTA
					sta VIA_PORTA
					lda #CLK
					eor VIA_PORTA
					sta VIA_PORTA
					dey
					bne @loop
					pla
					tay
					rts		
.endproc