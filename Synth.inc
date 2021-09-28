.area CSEG    (CODE)
_SynthAsm:
	pSynth = SynthAbsAddr
	mov (pSynth+pMixOut_l),#0
	mov (pSynth+pMixOut_h),#0	

	; r4,r5,r6,r7 : temporary registers

.irp  Idx,0,1,2,3,4,5

	pSndUnit = pSynth+Idx*unitSz
	mov a, (pSndUnit+pEnvelopeLevel)
	mov b,a
	jz soundUnitProcessEnd'Idx'$

	mov a,(pSndUnit+pWaveGenAccumulator_int)
	mov (pSndUnit+pSampleVal),a

	jb a.7,signedMulNegBranch'Idx'$	; Do signed mutiple with unsigned MUL
	mul ab
	mov (pSndUnit+pVal_l),a
	mov (pSndUnit+pVal_h),b
	mov a,b			; Div with 0xFF
	mov b,#0
	sjmp signedMulEnd'Idx'$
	signedMulNegBranch'Idx'$:
	cpl a				;Do signed mutiple with unsigned MUL
	inc a
	mul ab				; Mutiple envelopeLevel with sample

	cpl  a
	add  a,#1
	mov (pSndUnit+pVal_l),a
	mov a,b
	cpl a
	addc a,#0
	mov (pSndUnit+pVal_h),a	
	mov b,#0xFF ; Div with 0xFF	
	signedMulEnd'Idx'$:

	add a, (pSynth+pMixOut_l)
	mov (pSynth+pMixOut_l),a
	mov a,b
	addc a,(pSynth+pMixOut_h)
	mov (pSynth+pMixOut_h),a


	mov a,(pSndUnit+pIncrement_frac)
	add a,(pSndUnit+pWaveGenAccumulator_frac)
	mov (pSndUnit+pWaveGenAccumulator_frac),a

	mov a,(pSndUnit+pIncrement_int)
	addc a,(pSndUnit+pWaveGenAccumulator_int)
	mov (pSndUnit+pWaveGenAccumulator_int),a
soundUnitProcessEnd'Idx'$:
.endm

	mov r4,(pSynth+pMixOut_l)
	mov r5,(pSynth+pMixOut_h)

	;if (synthForAsm.mixOut > 255)
	clr	c
	mov	a,#0xff
	subb	a,r4
	mov	a,#(0x00 ^ 0x80)
	mov	b,r5
	xrl	b,#0x80
	subb	a,b
	jnc	clipBranchLowerBound$
	;synthForAsm.mixOut = 255;
	mov	r4,#0xff
	mov	r5,#0x00
	sjmp	clipBranchEnd$
	clipBranchLowerBound$:
	;else if (synthForAsm.mixOut < -255)
	clr	c
	mov	a,r4
	subb	a,#0x01
	mov	a,r5
	xrl	a,#0x80
	subb	a,#0x7f
	jnc	clipBranchEnd$
	;synthForAsm.mixOut = -255;
	mov	r4,#0x01
	mov	r5,#0xff
	clipBranchEnd$:
	
	; mixOut = mixOut>>1
	mov a,r5
	mov c,a.0
	mov a,r4
	rrc a
	mov r4,a

	; a = a - (-128)
	clr c
	subb a,#0x80

	mov REG_CCAP0H,a
	cpl a
	mov REG_CCAP1H,a