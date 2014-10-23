; flap --- USR routine for Flappy 101                          2014-03-24
; John Honniball

vec             equ     $E5               ; Vector in unused space

VDURAM          equ     $D00D             ; Top LH corner of VDU RAM
STRIDE          equ     64
GAP             equ     STRIDE*3

; Data block in zero-page for speed and smaller code
XCOORD          equ     $E8               ; VDU column
LC              equ     $E9               ; Left character
MC              equ     $EA               ; Middle character
RC              equ     $EB               ; Right character
CAP1            equ     $EC               ; Column cap char 1
CAP2            equ     $ED               ; Column cap char 2
CAP3            equ     $EE               ; Column cap char 3
CAP4            equ     $EF               ; Column cap char 4
ROWS            equ     $F0               ; Number of rows to fill
;
                org     $0228             ; Free RAM below BASIC
; USRDRAW
; Called from BASIC with parameter block in zero-page RAM
USRDRAW         lda     XCOORD            ; Load X coord
                lsr                       ; Divide by 2
                sta     XCOORD            ; Save halved X coord
                bcs     xodd              ; Did a '1' shift into carry?
                lda     #161              ; Whole block
                sta     LC
                sta     MC
                sta     CAP2
                lda     #157              ; RH half-block
                sta     CAP1
                lda     #156              ; LH half-block
                sta     CAP3
                lda     #32               ; Blank
                sta     RC
                sta     CAP4
                jmp     xdone
xodd            lda     #161              ; Whole block
                sta     CAP2
                sta     CAP3
                sta     MC
                lda     #157              ; RH half-block
                sta     LC
                lda     #156              ; LH half-block
                sta     RC
                lda     #32               ; Blank
                sta     CAP1
                sta     CAP4
xdone           jsr     USR2
                rts                       ; Back to BASIC
                
USRERASE        lda     XCOORD
                lsr                       ; Divide by 2
                sta     XCOORD            ; Save halved X coord
                lda     #32               ; Blank
                ldx     #6
er1             sta     LC,x              ; Fill entire parameter block
                dex
                bpl     er1
                jsr     USR2
                rts
;
USR2            lda     #<VDURAM          ; vec -> VDU RAM
                sta     vec
                lda     #>VDURAM
                sta     vec+1
                ldx     ROWS              ; Load row counter for top
                jsr     wrcol             ; Write upper column
                jsr     wrcap
                lda     vec               ; Add GAP to 'vec'
                clc
                adc     #<GAP
                sta     vec
                lda     vec+1
                adc     #>GAP
                sta     vec+1
                jsr     wrcap
                lda     #9
                sec
                sbc     ROWS
                tax                       ; X = 9 - ROWS
                jsr     wrcol             ; Write lower column
                rts

; addstride
; Add the VDU stride (64) to the contents of 'vec'
; Entry:
; Exit: A changed
addstride       lda     vec
                clc
                adc     #STRIDE
                sta     vec
                bcc     usr2
                inc     vec+1
usr2            rts

; wrcol
; Write a column of bytes to video memory
; Entry: X=number of rows to write
; Exit:
wrcol           ldy     XCOORD
usr1            lda     LC
                sta     (vec),y
                iny
                lda     MC
                sta     (vec),y
                iny
                lda     RC
                sta     (vec),y
                dey
                dey
                jsr     addstride
                dex
                bpl     usr1
                rts
;
; wrcap
; Write a column cap to video memory
; Entry:
; Exit:
wrcap           ldy     XCOORD
                dey
                bmi     nocap
                lda     CAP1
                sta     (vec),y
nocap           iny
                lda     CAP2
                sta     (vec),y
                iny
                lda     MC
                sta     (vec),y
                iny
                lda     CAP3
                sta     (vec),y
                iny
                lda     CAP4
                sta     (vec),y
                jsr     addstride         ; Add STRIDE to 'vec'
                rts
;
                end     USR
