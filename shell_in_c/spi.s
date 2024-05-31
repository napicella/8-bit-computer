; SPI mode 1 - MOSI, MISO, CLK, CS connected to PORTA DA8, DA7, DA6, DA5

					.setcpu		"6502"
		
					.export _spiRead
					.export _spiWrite
					.export _spiBegin
					.export _spiEnd
					.export _spiInit
		
					VIA1_BASE   = $8000
					PRA  = VIA1_BASE+1
					DDRA = VIA1_BASE+3

					MOSI = %10000000
					MISO = %01000000
					CLK  = %00100000
					CS   = %00010000
		
					.data
temp:				.byte 00	; used for shift in/out bits via carry, put into zeropage for minor speedup

.segment    "CODE"
.proc _spiInit: near
					pha
					; set clock low, CS unchanged
					lda #CLK
					eor #$FF
					and PRA
					ora #CS
					sta PRA

					; set data direction (default is input, so we only set the ones that needs to be output to 1)
					lda DDRA
					ora #MOSI
					ora #CS
					ora #CLK
					sta DDRA
					
					pla
					rts
.endproc

.segment    "CODE"
.proc _spiEnd: near
					pha
					lda PRA
					ora #CS	
					sta PRA
					pla
					rts
.endproc					
	

.segment    "CODE"
.proc _spiBegin: near
		 			pha
					; bring CS low
					lda #CS
					eor #$FF
					and PRA
					sta PRA
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
					ora PRA
					sta PRA
					ldy #8
@loop:				lda #CLK
					ora PRA
					sta PRA
					ldx PRA
					lda #CLK
					eor PRA
					sta PRA
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
					ora PRA             ; send one
					sta PRA
					jmp @is_high
@is_low:			lda	#MOSI           ; send zero
					eor #$FF
					and PRA
					sta PRA
@is_high:			lda #CLK            ; strobe CLK
					ora PRA
					sta PRA
					lda #CLK
					eor PRA
					sta PRA
					dey
					bne @loop
					pla
					tay
					rts		
.endproc