        .text
        .arm

        .section test, "ax"
        /* Data processing */
	    and r0, r1, r2
	    eor r0, r1, r2
	    sub r0, r1, r2
	    rsb r0, r1, r2
	    add r0, r1, r2
	    adc r0, r1, r2
        sbc r0, r1, r2
	    rsc r0, r1, r2
	    tst r0, r1
        teq r0, r1
        cmp r0, r1
        cmn r0, r1
        orr r0, r1, r2
        mov r0, r1
        bic r0, r1, r2
        mvn r0, r1
        /* Branch */
        b   .
        bl  .
        blx .
        blx r0
        bx  lr
        /* Interrupt */
        bkpt    0
        swi 0
        /* Load/Store */
        /* LDM(1) */
        ldmia   r0, {r1, r2, r3}
        ldmia   sp!, {r1, r2, r3}
        pop {r1, r2, r3}
        /* LDM(2) */
        ldmia   r0, {r1, r2, r3}^
        /* LDM(3) */
        ldmia   r0, {r1, r2, pc}^
        ldr r0, [r1]
        ldrb    r0, [r1]
        ldrbt   r0, [r1]
        ldrh    r0, [r1]
        ldrsb   r0, [r1]
        ldrsh   r0, [r1]
        ldrt    r0, [r1]
        /* STM(1) */
        stmdb   r0, {r1, r2, r3}
        stmdb   sp!, {r1, r2, r3}
        push    {r1, r2, r3}
        /* STM(2) */
        stmdb   r0, {r1, r2, r3}^
        str r0, [r1]
        strb    r0, [r1]
        strbt   r0, [r1]
        strh    r0, [r1]
        strt    r0, [r1]
        /* Multiply */
        mla r0, r1, r2, r3
        mul r0, r1, r2
        smlal   r0, r1, r2, r3
        smull   r0, r1, r2, r3
        umlal   r0, r1, r2, r3
        umull   r0, r1, r2, r3
        /* Coprocessor */
        cdp     p0, 0, cr0, cr1, cr2, 0
        cdp2    p0, 0, cr0, cr1, cr2, 0
        mcr     p0, 0, r0, cr0, cr1, 1
        mcr2    p0, 0, r0, cr0, cr1, 1
        mrc     p0, 0, r0, cr0, cr1, 1
        mrc2    p0, 0, r0, cr0, cr1, 1
        ldc     p0, cr0, [r0]
        ldc2    p0, cr0, [r0]
        stc     p0, cr0, [r0]
        stc2    p0, cr0, [r0]
        /* Misc */
        swp r0, r1, [r2]
        swpb    r0, r1, [r2]
        mrs r0, cpsr
        msr cpsr_fsxc, r0
        clz r0, r1
        /* DSP extension */
        ldrd    r0, [r1]
        strd    r0, [r1]
        mcrr    p0, 0, r0, r1, cr0
        mrrc    p0, 0, r0, r1, cr0
        pld [r0]
        qadd    r0, r1, r2
        qsub    r0, r1, r2
        qdadd   r0, r1, r2
        qdsub   r0, r1, r2
        smlabb  r0, r1, r2, r3
        smlabt  r0, r1, r2, r3
        smlatb  r0, r1, r2, r3
        smlatt  r0, r1, r2, r3
        smlalbb r0, r1, r2, r3
        smlalbt r0, r1, r2, r3
        smlaltb r0, r1, r2, r3
        smlaltt r0, r1, r2, r3
        smlawb  r0, r1, r2, r3
        smlawt  r0, r1, r2, r3
        smulbb  r0, r1, r2
        smulbt  r0, r1, r2
        smultb  r0, r1, r2
        smultt  r0, r1, r2
        smulwb  r0, r1, r2
        smulwt  r0, r1, r2
        
        add r0, r1, r2
        add r0, r1, r2
        add r0, r1, r2
        add r0, r1, r2
        add r0, r1, r2
        .end
        