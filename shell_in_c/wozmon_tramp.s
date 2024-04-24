.import __WOZMON_LOAD__
.export _wozmon_address
.export _wozmon_run

.segment    "CODE"
.proc    _wozmon_address: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ uint16 _wozmon_address ()
; ---------------------------------------------------------------
  jsr wozmon
  rts
.endproc

wozmon:
  lda #<(__WOZMON_LOAD__)
  ldx #>(__WOZMON_LOAD__)
  rts



.segment    "CODE"
.proc    _wozmon_run: near
; ---------------------------------------------------------------
; void __near__ __fastcall__ uint16 _wozmon_run ()
; ---------------------------------------------------------------
  jmp __WOZMON_LOAD__
  rts
.endproc