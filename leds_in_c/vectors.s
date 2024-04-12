; ---------------------------------------------------------------------------
; vectors.s
; ---------------------------------------------------------------------------
;
; Defines the interrupt vector table.

.import    _init
.import    _nmi_int,  irq ;_irq_int

.segment  "VECTORS"

.addr      _nmi_int    ; NMI vector
.addr      _init       ; Reset vector
.addr      irq    ; IRQ/BRK vector