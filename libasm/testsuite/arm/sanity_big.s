        .text
        .arm

        .section test, "ax"
        /* Data processing */
        /* AND */
        /* .. condition codes */
        andeq r0, r1, r2
        andne r0, r1, r2
        andcs r0, r1, r2
        andcc r0, r1, r2
        andmi r0, r1, r2
        andpl r0, r1, r2
        andvs r0, r1, r2
        andvc r0, r1, r2
        andhi r0, r1, r2
        andls r0, r1, r2
        andge r0, r1, r2
        andlt r0, r1, r2
        andgt r0, r1, r2
        andle r0, r1, r2
	    and r0, r1, r2
        nop
        /* .. write flags */
        andeqs r0, r1, r2
        andnes r0, r1, r2
        andcss r0, r1, r2
        andccs r0, r1, r2
        andmis r0, r1, r2
        andpls r0, r1, r2
        andvss r0, r1, r2
        andvcs r0, r1, r2
        andhis r0, r1, r2
        andlss r0, r1, r2
        andges r0, r1, r2
        andlts r0, r1, r2
        andgts r0, r1, r2
        andles r0, r1, r2
	    ands r0, r1, r2
        nop
        /* .. addressing modes */
        and r0, r1, #1
        and r0, r1, r2
        and r0, r1, r2, lsl #1
        and r0, r1, r2, lsl r3
        and r0, r1, r2, lsr #1
        and r0, r1, r2, lsr r3
        and r0, r1, r2, asr #1
        and r0, r1, r2, asr r3
        and r0, r1, r2, ror #1
        and r0, r1, r2, ror r3
        and r0, r1, r2, rrx
        nop
        /* .. write pc */
        and pc, r1, #1
        and pc, r1, r2
        and pc, r1, r2, lsl #1
        and pc, r1, r2, lsl r3
        and pc, r1, r2, lsr #1
        and pc, r1, r2, lsr r3
        and pc, r1, r2, asr #1
        and pc, r1, r2, asr r3
        and pc, r1, r2, ror #1
        and pc, r1, r2, ror r3
        and pc, r1, r2, rrx

        /* EOR */
        /* .. condition codes */
        eoreq r0, r1, r2
        eorne r0, r1, r2
        eorcs r0, r1, r2
        eorcc r0, r1, r2
        eormi r0, r1, r2
        eorpl r0, r1, r2
        eorvs r0, r1, r2
        eorvc r0, r1, r2
        eorhi r0, r1, r2
        eorls r0, r1, r2
        eorge r0, r1, r2
        eorlt r0, r1, r2
        eorgt r0, r1, r2
        eorle r0, r1, r2
	    eor r0, r1, r2
        nop
        /* .. write flags */
        eoreqs r0, r1, r2
        eornes r0, r1, r2
        eorcss r0, r1, r2
        eorccs r0, r1, r2
        eormis r0, r1, r2
        eorpls r0, r1, r2
        eorvss r0, r1, r2
        eorvcs r0, r1, r2
        eorhis r0, r1, r2
        eorlss r0, r1, r2
        eorges r0, r1, r2
        eorlts r0, r1, r2
        eorgts r0, r1, r2
        eorles r0, r1, r2
	    eors r0, r1, r2
        nop
        /* .. addressing modes */
        eor r0, r1, #1
        eor r0, r1, r2
        eor r0, r1, r2, lsl #1
        eor r0, r1, r2, lsl r3
        eor r0, r1, r2, lsr #1
        eor r0, r1, r2, lsr r3
        eor r0, r1, r2, asr #1
        eor r0, r1, r2, asr r3
        eor r0, r1, r2, ror #1
        eor r0, r1, r2, ror r3
        eor r0, r1, r2, rrx
        nop
        /* .. write pc */
        eor pc, r1, #1
        eor pc, r1, r2
        eor pc, r1, r2, lsl #1
        eor pc, r1, r2, lsl r3
        eor pc, r1, r2, lsr #1
        eor pc, r1, r2, lsr r3
        eor pc, r1, r2, asr #1
        eor pc, r1, r2, asr r3
        eor pc, r1, r2, ror #1
        eor pc, r1, r2, ror r3
        eor pc, r1, r2, rrx

        /* SUB */
        /* .. condition codes */
        subeq r0, r1, r2
        subne r0, r1, r2
        subcs r0, r1, r2
        subcc r0, r1, r2
        submi r0, r1, r2
        subpl r0, r1, r2
        subvs r0, r1, r2
        subvc r0, r1, r2
        subhi r0, r1, r2
        subls r0, r1, r2
        subge r0, r1, r2
        sublt r0, r1, r2
        subgt r0, r1, r2
        suble r0, r1, r2
	    sub r0, r1, r2
        nop
        /* .. write flags */
        subeqs r0, r1, r2
        subnes r0, r1, r2
        subcss r0, r1, r2
        subccs r0, r1, r2
        submis r0, r1, r2
        subpls r0, r1, r2
        subvss r0, r1, r2
        subvcs r0, r1, r2
        subhis r0, r1, r2
        sublss r0, r1, r2
        subges r0, r1, r2
        sublts r0, r1, r2
        subgts r0, r1, r2
        subles r0, r1, r2
	    subs r0, r1, r2
        nop
        /* .. addressing modes */
        sub r0, r1, #1
        sub r0, r1, r2
        sub r0, r1, r2, lsl #1
        sub r0, r1, r2, lsl r3
        sub r0, r1, r2, lsr #1
        sub r0, r1, r2, lsr r3
        sub r0, r1, r2, asr #1
        sub r0, r1, r2, asr r3
        sub r0, r1, r2, ror #1
        sub r0, r1, r2, ror r3
        sub r0, r1, r2, rrx
        nop
        /* .. write pc */
        sub pc, r1, #1
        sub pc, r1, r2
        sub pc, r1, r2, lsl #1
        sub pc, r1, r2, lsl r3
        sub pc, r1, r2, lsr #1
        sub pc, r1, r2, lsr r3
        sub pc, r1, r2, asr #1
        sub pc, r1, r2, asr r3
        sub pc, r1, r2, ror #1
        sub pc, r1, r2, ror r3
        sub pc, r1, r2, rrx

        /* RSB */
        /* .. condition codes */
        rsbeq r0, r1, r2
        rsbne r0, r1, r2
        rsbcs r0, r1, r2
        rsbcc r0, r1, r2
        rsbmi r0, r1, r2
        rsbpl r0, r1, r2
        rsbvs r0, r1, r2
        rsbvc r0, r1, r2
        rsbhi r0, r1, r2
        rsbls r0, r1, r2
        rsbge r0, r1, r2
        rsblt r0, r1, r2
        rsbgt r0, r1, r2
        rsble r0, r1, r2
	    rsb r0, r1, r2
        nop
        /* .. write flags */
        rsbeqs r0, r1, r2
        rsbnes r0, r1, r2
        rsbcss r0, r1, r2
        rsbccs r0, r1, r2
        rsbmis r0, r1, r2
        rsbpls r0, r1, r2
        rsbvss r0, r1, r2
        rsbvcs r0, r1, r2
        rsbhis r0, r1, r2
        rsblss r0, r1, r2
        rsbges r0, r1, r2
        rsblts r0, r1, r2
        rsbgts r0, r1, r2
        rsbles r0, r1, r2
	    rsbs r0, r1, r2
        nop
        /* .. addressing modes */
        rsb r0, r1, #1
        rsb r0, r1, r2
        rsb r0, r1, r2, lsl #1
        rsb r0, r1, r2, lsl r3
        rsb r0, r1, r2, lsr #1
        rsb r0, r1, r2, lsr r3
        rsb r0, r1, r2, asr #1
        rsb r0, r1, r2, asr r3
        rsb r0, r1, r2, ror #1
        rsb r0, r1, r2, ror r3
        rsb r0, r1, r2, rrx
        nop
        /* .. write pc */
        rsb pc, r1, #1
        rsb pc, r1, r2
        rsb pc, r1, r2, lsl #1
        rsb pc, r1, r2, lsl r3
        rsb pc, r1, r2, lsr #1
        rsb pc, r1, r2, lsr r3
        rsb pc, r1, r2, asr #1
        rsb pc, r1, r2, asr r3
        rsb pc, r1, r2, ror #1
        rsb pc, r1, r2, ror r3
        rsb pc, r1, r2, rrx

        /* ADD */
        /* .. condition codes */
        addeq r0, r1, r2
        addne r0, r1, r2
        addcs r0, r1, r2
        addcc r0, r1, r2
        addmi r0, r1, r2
        addpl r0, r1, r2
        addvs r0, r1, r2
        addvc r0, r1, r2
        addhi r0, r1, r2
        addls r0, r1, r2
        addge r0, r1, r2
        addlt r0, r1, r2
        addgt r0, r1, r2
        addle r0, r1, r2
	    add r0, r1, r2
        nop
        /* .. write flags */
        addeqs r0, r1, r2
        addnes r0, r1, r2
        addcss r0, r1, r2
        addccs r0, r1, r2
        addmis r0, r1, r2
        addpls r0, r1, r2
        addvss r0, r1, r2
        addvcs r0, r1, r2
        addhis r0, r1, r2
        addlss r0, r1, r2
        addges r0, r1, r2
        addlts r0, r1, r2
        addgts r0, r1, r2
        addles r0, r1, r2
	    adds r0, r1, r2
        nop
        /* .. addressing modes */
        add r0, r1, #1
        add r0, r1, r2
        add r0, r1, r2, lsl #1
        add r0, r1, r2, lsl r3
        add r0, r1, r2, lsr #1
        add r0, r1, r2, lsr r3
        add r0, r1, r2, asr #1
        add r0, r1, r2, asr r3
        add r0, r1, r2, ror #1
        add r0, r1, r2, ror r3
        add r0, r1, r2, rrx
        nop
        /* .. write pc */
        add pc, r1, #1
        add pc, r1, r2
        add pc, r1, r2, lsl #1
        add pc, r1, r2, lsl r3
        add pc, r1, r2, lsr #1
        add pc, r1, r2, lsr r3
        add pc, r1, r2, asr #1
        add pc, r1, r2, asr r3
        add pc, r1, r2, ror #1
        add pc, r1, r2, ror r3
        add pc, r1, r2, rrx

        /* ADC */
        /* .. condition codes */
        adceq r0, r1, r2
        adcne r0, r1, r2
        adccs r0, r1, r2
        adccc r0, r1, r2
        adcmi r0, r1, r2
        adcpl r0, r1, r2
        adcvs r0, r1, r2
        adcvc r0, r1, r2
        adchi r0, r1, r2
        adcls r0, r1, r2
        adcge r0, r1, r2
        adclt r0, r1, r2
        adcgt r0, r1, r2
        adcle r0, r1, r2
	    adc r0, r1, r2
        nop
        /* .. write flags */
        adceqs r0, r1, r2
        adcnes r0, r1, r2
        adccss r0, r1, r2
        adcccs r0, r1, r2
        adcmis r0, r1, r2
        adcpls r0, r1, r2
        adcvss r0, r1, r2
        adcvcs r0, r1, r2
        adchis r0, r1, r2
        adclss r0, r1, r2
        adcges r0, r1, r2
        adclts r0, r1, r2
        adcgts r0, r1, r2
        adcles r0, r1, r2
	    adcs r0, r1, r2
        nop
        /* .. addressing modes */
        adc r0, r1, #1
        adc r0, r1, r2
        adc r0, r1, r2, lsl #1
        adc r0, r1, r2, lsl r3
        adc r0, r1, r2, lsr #1
        adc r0, r1, r2, lsr r3
        adc r0, r1, r2, asr #1
        adc r0, r1, r2, asr r3
        adc r0, r1, r2, ror #1
        adc r0, r1, r2, ror r3
        adc r0, r1, r2, rrx
        nop
        /* .. write pc */
        adc pc, r1, #1
        adc pc, r1, r2
        adc pc, r1, r2, lsl #1
        adc pc, r1, r2, lsl r3
        adc pc, r1, r2, lsr #1
        adc pc, r1, r2, lsr r3
        adc pc, r1, r2, asr #1
        adc pc, r1, r2, asr r3
        adc pc, r1, r2, ror #1
        adc pc, r1, r2, ror r3
        adc pc, r1, r2, rrx

        /* SBC */
        /* .. condition codes */
        sbceq r0, r1, r2
        sbcne r0, r1, r2
        sbccs r0, r1, r2
        sbccc r0, r1, r2
        sbcmi r0, r1, r2
        sbcpl r0, r1, r2
        sbcvs r0, r1, r2
        sbcvc r0, r1, r2
        sbchi r0, r1, r2
        sbcls r0, r1, r2
        sbcge r0, r1, r2
        sbclt r0, r1, r2
        sbcgt r0, r1, r2
        sbcle r0, r1, r2
	    sbc r0, r1, r2
        nop
        /* .. write flags */
        sbceqs r0, r1, r2
        sbcnes r0, r1, r2
        sbccss r0, r1, r2
        sbcccs r0, r1, r2
        sbcmis r0, r1, r2
        sbcpls r0, r1, r2
        sbcvss r0, r1, r2
        sbcvcs r0, r1, r2
        sbchis r0, r1, r2
        sbclss r0, r1, r2
        sbcges r0, r1, r2
        sbclts r0, r1, r2
        sbcgts r0, r1, r2
        sbcles r0, r1, r2
	    sbcs r0, r1, r2
        nop
        /* .. addressing modes */
        sbc r0, r1, #1
        sbc r0, r1, r2
        sbc r0, r1, r2, lsl #1
        sbc r0, r1, r2, lsl r3
        sbc r0, r1, r2, lsr #1
        sbc r0, r1, r2, lsr r3
        sbc r0, r1, r2, asr #1
        sbc r0, r1, r2, asr r3
        sbc r0, r1, r2, ror #1
        sbc r0, r1, r2, ror r3
        sbc r0, r1, r2, rrx
        nop
        /* .. write pc */
        sbc pc, r1, #1
        sbc pc, r1, r2
        sbc pc, r1, r2, lsl #1
        sbc pc, r1, r2, lsl r3
        sbc pc, r1, r2, lsr #1
        sbc pc, r1, r2, lsr r3
        sbc pc, r1, r2, asr #1
        sbc pc, r1, r2, asr r3
        sbc pc, r1, r2, ror #1
        sbc pc, r1, r2, ror r3
        sbc pc, r1, r2, rrx

        /* RSC */
        /* .. condition codes */
        rsceq r0, r1, r2
        rscne r0, r1, r2
        rsccs r0, r1, r2
        rsccc r0, r1, r2
        rscmi r0, r1, r2
        rscpl r0, r1, r2
        rscvs r0, r1, r2
        rscvc r0, r1, r2
        rschi r0, r1, r2
        rscls r0, r1, r2
        rscge r0, r1, r2
        rsclt r0, r1, r2
        rscgt r0, r1, r2
        rscle r0, r1, r2
	    rsc r0, r1, r2
        nop
        /* .. write flags */
        rsceqs r0, r1, r2
        rscnes r0, r1, r2
        rsccss r0, r1, r2
        rscccs r0, r1, r2
        rscmis r0, r1, r2
        rscpls r0, r1, r2
        rscvss r0, r1, r2
        rscvcs r0, r1, r2
        rschis r0, r1, r2
        rsclss r0, r1, r2
        rscges r0, r1, r2
        rsclts r0, r1, r2
        rscgts r0, r1, r2
        rscles r0, r1, r2
	    rscs r0, r1, r2
        nop
        /* .. addressing modes */
        rsc r0, r1, #1
        rsc r0, r1, r2
        rsc r0, r1, r2, lsl #1
        rsc r0, r1, r2, lsl r3
        rsc r0, r1, r2, lsr #1
        rsc r0, r1, r2, lsr r3
        rsc r0, r1, r2, asr #1
        rsc r0, r1, r2, asr r3
        rsc r0, r1, r2, ror #1
        rsc r0, r1, r2, ror r3
        rsc r0, r1, r2, rrx
        nop
        /* .. write pc */
        rsc pc, r1, #1
        rsc pc, r1, r2
        rsc pc, r1, r2, lsl #1
        rsc pc, r1, r2, lsl r3
        rsc pc, r1, r2, lsr #1
        rsc pc, r1, r2, lsr r3
        rsc pc, r1, r2, asr #1
        rsc pc, r1, r2, asr r3
        rsc pc, r1, r2, ror #1
        rsc pc, r1, r2, ror r3
        rsc pc, r1, r2, rrx

        /* TST */
        /* .. condition codes */
        tsteq r0, r1
        tstne r0, r1
        tstcs r0, r1
        tstcc r0, r1
        tstmi r0, r1
        tstpl r0, r1
        tstvs r0, r1
        tstvc r0, r1
        tsthi r0, r1
        tstls r0, r1
        tstge r0, r1
        tstlt r0, r1
        tstgt r0, r1
        tstle r0, r1
	    tst r0, r1
        nop
        /* .. addressing modes */
        tst r0, #1
        tst r0, r1
        tst r0, r1, lsl #1
        tst r0, r1, lsl r2
        tst r0, r1, lsr #1
        tst r0, r1, lsr r2
        tst r0, r1, asr #1
        tst r0, r1, asr r2
        tst r0, r1, ror #1
        tst r0, r1, ror r2
        tst r0, r1, rrx

        /* TEQ */
        /* .. condition codes */
        teqeq r0, r1
        teqne r0, r1
        teqcs r0, r1
        teqcc r0, r1
        teqmi r0, r1
        teqpl r0, r1
        teqvs r0, r1
        teqvc r0, r1
        teqhi r0, r1
        teqls r0, r1
        teqge r0, r1
        teqlt r0, r1
        teqgt r0, r1
        teqle r0, r1
	    teq r0, r1
        nop
        /* .. addressing modes */
        teq r0, #1
        teq r0, r1
        teq r0, r1, lsl #1
        teq r0, r1, lsl r2
        teq r0, r1, lsr #1
        teq r0, r1, lsr r2
        teq r0, r1, asr #1
        teq r0, r1, asr r2
        teq r0, r1, ror #1
        teq r0, r1, ror r2
        teq r0, r1, rrx

        /* CMP */
        /* .. condition codes */
        cmpeq r0, r1
        cmpne r0, r1
        cmpcs r0, r1
        cmpcc r0, r1
        cmpmi r0, r1
        cmppl r0, r1
        cmpvs r0, r1
        cmpvc r0, r1
        cmphi r0, r1
        cmpls r0, r1
        cmpge r0, r1
        cmplt r0, r1
        cmpgt r0, r1
        cmple r0, r1
	    cmp r0, r1
        nop
        /* .. addressing modes */
        cmp r0, #1
        cmp r0, r1
        cmp r0, r1, lsl #1
        cmp r0, r1, lsl r2
        cmp r0, r1, lsr #1
        cmp r0, r1, lsr r2
        cmp r0, r1, asr #1
        cmp r0, r1, asr r2
        cmp r0, r1, ror #1
        cmp r0, r1, ror r2
        cmp r0, r1, rrx

        /* CMN */
        /* .. condition codes */
        cmneq r0, r1
        cmnne r0, r1
        cmncs r0, r1
        cmncc r0, r1
        cmnmi r0, r1
        cmnpl r0, r1
        cmnvs r0, r1
        cmnvc r0, r1
        cmnhi r0, r1
        cmnls r0, r1
        cmnge r0, r1
        cmnlt r0, r1
        cmngt r0, r1
        cmnle r0, r1
	    cmn r0, r1
        nop
        /* .. addressing modes */
        cmn r0, #1
        cmn r0, r1
        cmn r0, r1, lsl #1
        cmn r0, r1, lsl r2
        cmn r0, r1, lsr #1
        cmn r0, r1, lsr r2
        cmn r0, r1, asr #1
        cmn r0, r1, asr r2
        cmn r0, r1, ror #1
        cmn r0, r1, ror r2
        cmn r0, r1, rrx

        /* ORR */
        /* .. condition codes */
        orreq r0, r1, r2
        orrne r0, r1, r2
        orrcs r0, r1, r2
        orrcc r0, r1, r2
        orrmi r0, r1, r2
        orrpl r0, r1, r2
        orrvs r0, r1, r2
        orrvc r0, r1, r2
        orrhi r0, r1, r2
        orrls r0, r1, r2
        orrge r0, r1, r2
        orrlt r0, r1, r2
        orrgt r0, r1, r2
        orrle r0, r1, r2
	    orr r0, r1, r2
        nop
        /* .. write flags */
        orreqs r0, r1, r2
        orrnes r0, r1, r2
        orrcss r0, r1, r2
        orrccs r0, r1, r2
        orrmis r0, r1, r2
        orrpls r0, r1, r2
        orrvss r0, r1, r2
        orrvcs r0, r1, r2
        orrhis r0, r1, r2
        orrlss r0, r1, r2
        orrges r0, r1, r2
        orrlts r0, r1, r2
        orrgts r0, r1, r2
        orrles r0, r1, r2
	    orrs r0, r1, r2
        nop
        /* .. addressing modes */
        orr r0, r1, #1
        orr r0, r1, r2
        orr r0, r1, r2, lsl #1
        orr r0, r1, r2, lsl r3
        orr r0, r1, r2, lsr #1
        orr r0, r1, r2, lsr r3
        orr r0, r1, r2, asr #1
        orr r0, r1, r2, asr r3
        orr r0, r1, r2, ror #1
        orr r0, r1, r2, ror r3
        orr r0, r1, r2, rrx
        nop
        /* .. write pc */
        orr pc, r1, #1
        orr pc, r1, r2
        orr pc, r1, r2, lsl #1
        orr pc, r1, r2, lsl r3
        orr pc, r1, r2, lsr #1
        orr pc, r1, r2, lsr r3
        orr pc, r1, r2, asr #1
        orr pc, r1, r2, asr r3
        orr pc, r1, r2, ror #1
        orr pc, r1, r2, ror r3
        orr pc, r1, r2, rrx

        /* MOV */
        /* .. condition codes */
        moveq r0, r1
        movne r0, r1
        movcs r0, r1
        movcc r0, r1
        movmi r0, r1
        movpl r0, r1
        movvs r0, r1
        movvc r0, r1
        movhi r0, r1
        movls r0, r1
        movge r0, r1
        movlt r0, r1
        movgt r0, r1
        movle r0, r1
	    mov r0, r1
        nop
        /* .. write flags */
        moveqs r0, r1
        movnes r0, r1
        movcss r0, r1
        movccs r0, r1
        movmis r0, r1
        movpls r0, r1
        movvss r0, r1
        movvcs r0, r1
        movhis r0, r1
        movlss r0, r1
        movges r0, r1
        movlts r0, r1
        movgts r0, r1
        movles r0, r1
	    movs r0, r1
        nop
        /* .. addressing modes */
        mov r0, #1
        mov r0, r1
        mov r0, r1, lsl #1
        mov r0, r1, lsl r2
        mov r0, r1, lsr #1
        mov r0, r1, lsr r2
        mov r0, r1, asr #1
        mov r0, r1, asr r2
        mov r0, r1, ror #1
        mov r0, r1, ror r2
        mov r0, r1, rrx
        nop
        /* .. write pc */
        mov pc, #1
        mov pc, r1
        mov pc, r1, lsl #1
        mov pc, r1, lsl r2
        mov pc, r1, lsr #1
        mov pc, r1, lsr r2
        mov pc, r1, asr #1
        mov pc, r1, asr r2
        mov pc, r1, ror #1
        mov pc, r1, ror r2
        mov pc, r1, rrx

        /* BIC */
        /* .. condition codes */
        biceq r0, r1, r2
        bicne r0, r1, r2
        biccs r0, r1, r2
        biccc r0, r1, r2
        bicmi r0, r1, r2
        bicpl r0, r1, r2
        bicvs r0, r1, r2
        bicvc r0, r1, r2
        bichi r0, r1, r2
        bicls r0, r1, r2
        bicge r0, r1, r2
        biclt r0, r1, r2
        bicgt r0, r1, r2
        bicle r0, r1, r2
	    bic r0, r1, r2
        nop
        /* .. write flags */
        biceqs r0, r1, r2
        bicnes r0, r1, r2
        biccss r0, r1, r2
        bicccs r0, r1, r2
        bicmis r0, r1, r2
        bicpls r0, r1, r2
        bicvss r0, r1, r2
        bicvcs r0, r1, r2
        bichis r0, r1, r2
        biclss r0, r1, r2
        bicges r0, r1, r2
        biclts r0, r1, r2
        bicgts r0, r1, r2
        bicles r0, r1, r2
	    bics r0, r1, r2
        nop
        /* .. addressing modes */
        bic r0, r1, #1
        bic r0, r1, r2
        bic r0, r1, r2, lsl #1
        bic r0, r1, r2, lsl r3
        bic r0, r1, r2, lsr #1
        bic r0, r1, r2, lsr r3
        bic r0, r1, r2, asr #1
        bic r0, r1, r2, asr r3
        bic r0, r1, r2, ror #1
        bic r0, r1, r2, ror r3
        bic r0, r1, r2, rrx
        nop
        /* .. write pc */
        bic pc, r1, #1
        bic pc, r1, r2
        bic pc, r1, r2, lsl #1
        bic pc, r1, r2, lsl r3
        bic pc, r1, r2, lsr #1
        bic pc, r1, r2, lsr r3
        bic pc, r1, r2, asr #1
        bic pc, r1, r2, asr r3
        bic pc, r1, r2, ror #1
        bic pc, r1, r2, ror r3
        bic pc, r1, r2, rrx

        /* MVN */
        /* .. condition codes */
        mvneq r0, r1
        mvnne r0, r1
        mvncs r0, r1
        mvncc r0, r1
        mvnmi r0, r1
        mvnpl r0, r1
        mvnvs r0, r1
        mvnvc r0, r1
        mvnhi r0, r1
        mvnls r0, r1
        mvnge r0, r1
        mvnlt r0, r1
        mvngt r0, r1
        mvnle r0, r1
	    mvn r0, r1
        nop
        /* .. write flags */
        mvneqs r0, r1
        mvnnes r0, r1
        mvncss r0, r1
        mvnccs r0, r1
        mvnmis r0, r1
        mvnpls r0, r1
        mvnvss r0, r1
        mvnvcs r0, r1
        mvnhis r0, r1
        mvnlss r0, r1
        mvnges r0, r1
        mvnlts r0, r1
        mvngts r0, r1
        mvnles r0, r1
	    mvns r0, r1
        nop
        /* .. addressing modes */
        mvn r0, #1
        mvn r0, r1
        mvn r0, r1, lsl #1
        mvn r0, r1, lsl r2
        mvn r0, r1, lsr #1
        mvn r0, r1, lsr r2
        mvn r0, r1, asr #1
        mvn r0, r1, asr r2
        mvn r0, r1, ror #1
        mvn r0, r1, ror r2
        mvn r0, r1, rrx
        nop
        /* .. write pc */
        mvn pc, #1
        mvn pc, r1
        mvn pc, r1, lsl #1
        mvn pc, r1, lsl r2
        mvn pc, r1, lsr #1
        mvn pc, r1, lsr r2
        mvn pc, r1, asr #1
        mvn pc, r1, asr r2
        mvn pc, r1, ror #1
        mvn pc, r1, ror r2
        mvn pc, r1, rrx

        /* Branch */
        /* B */
        /* .. condition codes */
        beq .
        bne .
        bcs .
        bcc .
        bmi .
        bpl .
        bvs .
        bvc .
        bhi .
        bls .
        bge .
        blt .
        bgt .
        ble .
	    b .

        /* BL */
        /* .. condition codes */
        bleq .
        blne .
        blcs .
        blcc .
        blmi .
        blpl .
        blvs .
        blvc .
        blhi .
        blls .
        blge .
        bllt .
        blgt .
        blle .
	    bl .

        /* BLX(1) */
	    blx .

        /* BLX(2) */
        /* .. condition codes */
        blxeq r0
        blxne r0
        blxcs r0
        blxcc r0
        blxmi r0
        blxpl r0
        blxvs r0
        blxvc r0
        blxhi r0
        blxls r0
        blxge r0
        blxlt r0
        blxgt r0
        blxle r0
	    blx r0

        /* BX */
        /* .. condition codes */
        bxeq r0
        bxne r0
        bxcs r0
        bxcc r0
        bxmi r0
        bxpl r0
        bxvs r0
        bxvc r0
        bxhi r0
        bxls r0
        bxge r0
        bxlt r0
        bxgt r0
        bxle r0
	    bx r0
        nop
        /* .. use LR */
        bxeq lr
        bxne lr
        bxcs lr
        bxcc lr
        bxmi lr
        bxpl lr
        bxvs lr
        bxvc lr
        bxhi lr
        bxls lr
        bxge lr
        bxlt lr
        bxgt lr
        bxle lr
	    bx lr

        /* Interrupt */
        /* BKPT */
        bkpt    0
        
        /* SWI */
        /* .. condition codes */
        swieq 0
        swine 0
        swics 0
        swicc 0
        swimi 0
        swipl 0
        swivs 0
        swivc 0
        swihi 0
        swils 0
        swige 0
        swilt 0
        swigt 0
        swile 0
        swi 0
        
        /* Load/Store */
        /* LDM(1) */
        /* .. condition codes */
        ldmeqia r0, {r1, r2, r3}
        ldmneia r0, {r1, r2, r3}
        ldmcsia r0, {r1, r2, r3}
        ldmccia r0, {r1, r2, r3}
        ldmmiia r0, {r1, r2, r3}
        ldmplia r0, {r1, r2, r3}
        ldmvsia r0, {r1, r2, r3}
        ldmvcia r0, {r1, r2, r3}
        ldmhiia r0, {r1, r2, r3}
        ldmlsia r0, {r1, r2, r3}
        ldmgeia r0, {r1, r2, r3}
        ldmltia r0, {r1, r2, r3}
        ldmgtia r0, {r1, r2, r3}
        ldmleia r0, {r1, r2, r3}
        ldmia r0, {r1, r2, r3}
        nop
        /* .. addressing modes */
        ldmeqib r0, {r1, r2, r3}
        ldmneib r0, {r1, r2, r3}
        ldmcsib r0, {r1, r2, r3}
        ldmccib r0, {r1, r2, r3}
        ldmmiib r0, {r1, r2, r3}
        ldmplib r0, {r1, r2, r3}
        ldmvsib r0, {r1, r2, r3}
        ldmvcib r0, {r1, r2, r3}
        ldmhiib r0, {r1, r2, r3}
        ldmlsib r0, {r1, r2, r3}
        ldmgeib r0, {r1, r2, r3}
        ldmltib r0, {r1, r2, r3}
        ldmgtib r0, {r1, r2, r3}
        ldmleib r0, {r1, r2, r3}
        ldmib r0, {r1, r2, r3}
        ldmeqda r0, {r1, r2, r3}
        ldmneda r0, {r1, r2, r3}
        ldmcsda r0, {r1, r2, r3}
        ldmccda r0, {r1, r2, r3}
        ldmmida r0, {r1, r2, r3}
        ldmplda r0, {r1, r2, r3}
        ldmvsda r0, {r1, r2, r3}
        ldmvcda r0, {r1, r2, r3}
        ldmhida r0, {r1, r2, r3}
        ldmlsda r0, {r1, r2, r3}
        ldmgeda r0, {r1, r2, r3}
        ldmltda r0, {r1, r2, r3}
        ldmgtda r0, {r1, r2, r3}
        ldmleda r0, {r1, r2, r3}
        ldmda r0, {r1, r2, r3}
        ldmeqdb r0, {r1, r2, r3}
        ldmnedb r0, {r1, r2, r3}
        ldmcsdb r0, {r1, r2, r3}
        ldmccdb r0, {r1, r2, r3}
        ldmmidb r0, {r1, r2, r3}
        ldmpldb r0, {r1, r2, r3}
        ldmvsdb r0, {r1, r2, r3}
        ldmvcdb r0, {r1, r2, r3}
        ldmhidb r0, {r1, r2, r3}
        ldmlsdb r0, {r1, r2, r3}
        ldmgedb r0, {r1, r2, r3}
        ldmltdb r0, {r1, r2, r3}
        ldmgtdb r0, {r1, r2, r3}
        ldmledb r0, {r1, r2, r3}
        ldmdb r0, {r1, r2, r3}
        nop
        /* .. writeback */
        ldmia r0!, {r1, r2, r3}
        nop
        /* .. touch sp */
        ldmia sp!, {r1, r2, r3}
        pop {r1, r2, r3}
        nop
        /* .. touch pc */
        ldmia sp!, {r1, r2, pc}
        nop
        /* LDM(2) */
        ldmia   r0, {r1, r2, r3}^
        nop
        /* LDM(3) */
        ldmia   r0, {r1, r2, pc}^

        /* LDR */
        /* .. condition codes */
        ldreq r0, [r1]
        ldrne r0, [r1]
        ldrcs r0, [r1]
        ldrcc r0, [r1]
        ldrmi r0, [r1]
        ldrpl r0, [r1]
        ldrvs r0, [r1]
        ldrvc r0, [r1]
        ldrhi r0, [r1]
        ldrls r0, [r1]
        ldrge r0, [r1]
        ldrlt r0, [r1]
        ldrgt r0, [r1]
        ldrle r0, [r1]
        ldr r0, [r1]
        nop
        /* .. addressing modes */
        ldr r0, [r1]
        ldr r0, [r1, #4]
        ldr r0, [r1, #-4]
        ldr r0, [r1, r2]
        ldr r0, [r1, -r2]
        ldr r0, [r1, r2, lsl #2]
        ldr r0, [r1, -r2, lsl #2]
        ldr r0, [r1, r2, lsr #2]
        ldr r0, [r1, -r2, lsr #2]
        ldr r0, [r1, r2, asr #2]
        ldr r0, [r1, -r2, asr #2]
        ldr r0, [r1, r2, ror #2]
        ldr r0, [r1, -r2, ror #2]
        ldr r0, [r1, r2, rrx]
        ldr r0, [r1, -r2, rrx]
        ldr r0, [r1]!
        ldr r0, [r1, #4]!
        ldr r0, [r1, #-4]!
        ldr r0, [r1, r2]!
        ldr r0, [r1, -r2]!
        ldr r0, [r1, r2, lsl #2]!
        ldr r0, [r1, -r2, lsl #2]!
        ldr r0, [r1, r2, lsr #2]!
        ldr r0, [r1, -r2, lsr #2]!
        ldr r0, [r1, r2, asr #2]!
        ldr r0, [r1, -r2, asr #2]!
        ldr r0, [r1, r2, ror #2]!
        ldr r0, [r1, -r2, ror #2]!
        ldr r0, [r1, r2, rrx]!
        ldr r0, [r1, -r2, rrx]!
        ldr r0, [r1], #4
        ldr r0, [r1], #-4
        ldr r0, [r1], r2
        ldr r0, [r1], -r2
        ldr r0, [r1], r2, lsl #2
        ldr r0, [r1], -r2, lsl #2
        ldr r0, [r1], r2, lsr #2
        ldr r0, [r1], -r2, lsr #2
        ldr r0, [r1], r2, asr #2
        ldr r0, [r1], -r2, asr #2
        ldr r0, [r1], r2, ror #2
        ldr r0, [r1], -r2, ror #2
        ldr r0, [r1], r2, rrx
        ldr r0, [r1], -r2, rrx
        nop
        /* .. write PC */
        ldr pc, [r1]
        ldr pc, [r1, #4]
        ldr pc, [r1, #-4]
        ldr pc, [r1, r2]
        ldr pc, [r1, -r2]
        ldr pc, [r1, r2, lsl #2]
        ldr pc, [r1, -r2, lsl #2]
        ldr pc, [r1, r2, lsr #2]
        ldr pc, [r1, -r2, lsr #2]
        ldr pc, [r1, r2, asr #2]
        ldr pc, [r1, -r2, asr #2]
        ldr pc, [r1, r2, ror #2]
        ldr pc, [r1, -r2, ror #2]
        ldr pc, [r1, r2, rrx]
        ldr pc, [r1, -r2, rrx]
        ldr pc, [r1]!
        ldr pc, [r1, #4]!
        ldr pc, [r1, #-4]!
        ldr pc, [r1, r2]!
        ldr pc, [r1, -r2]!
        ldr pc, [r1, r2, lsl #2]!
        ldr pc, [r1, -r2, lsl #2]!
        ldr pc, [r1, r2, lsr #2]!
        ldr pc, [r1, -r2, lsr #2]!
        ldr pc, [r1, r2, asr #2]!
        ldr pc, [r1, -r2, asr #2]!
        ldr pc, [r1, r2, ror #2]!
        ldr pc, [r1, -r2, ror #2]!
        ldr pc, [r1, r2, rrx]!
        ldr pc, [r1, -r2, rrx]!
        ldr pc, [r1], #4
        ldr pc, [r1], #-4
        ldr pc, [r1], r2
        ldr pc, [r1], -r2
        ldr pc, [r1], r2, lsl #2
        ldr pc, [r1], -r2, lsl #2
        ldr pc, [r1], r2, lsr #2
        ldr pc, [r1], -r2, lsr #2
        ldr pc, [r1], r2, asr #2
        ldr pc, [r1], -r2, asr #2
        ldr pc, [r1], r2, ror #2
        ldr pc, [r1], -r2, ror #2
        ldr pc, [r1], r2, rrx
        ldr pc, [r1], -r2, rrx

        /* LDRB */
        /* .. condition codes */
        ldreqb r0, [r1]
        ldrneb r0, [r1]
        ldrcsb r0, [r1]
        ldrccb r0, [r1]
        ldrmib r0, [r1]
        ldrplb r0, [r1]
        ldrvsb r0, [r1]
        ldrvcb r0, [r1]
        ldrhib r0, [r1]
        ldrlsb r0, [r1]
        ldrgeb r0, [r1]
        ldrltb r0, [r1]
        ldrgtb r0, [r1]
        ldrleb r0, [r1]
        ldrb r0, [r1]
        nop
        /* .. addressing modes */
        ldrb r0, [r1]
        ldrb r0, [r1, #4]
        ldrb r0, [r1, #-4]
        ldrb r0, [r1, r2]
        ldrb r0, [r1, -r2]
        ldrb r0, [r1, r2, lsl #2]
        ldrb r0, [r1, -r2, lsl #2]
        ldrb r0, [r1, r2, lsr #2]
        ldrb r0, [r1, -r2, lsr #2]
        ldrb r0, [r1, r2, asr #2]
        ldrb r0, [r1, -r2, asr #2]
        ldrb r0, [r1, r2, ror #2]
        ldrb r0, [r1, -r2, ror #2]
        ldrb r0, [r1, r2, rrx]
        ldrb r0, [r1, -r2, rrx]
        ldrb r0, [r1]!
        ldrb r0, [r1, #4]!
        ldrb r0, [r1, #-4]!
        ldrb r0, [r1, r2]!
        ldrb r0, [r1, -r2]!
        ldrb r0, [r1, r2, lsl #2]!
        ldrb r0, [r1, -r2, lsl #2]!
        ldrb r0, [r1, r2, lsr #2]!
        ldrb r0, [r1, -r2, lsr #2]!
        ldrb r0, [r1, r2, asr #2]!
        ldrb r0, [r1, -r2, asr #2]!
        ldrb r0, [r1, r2, ror #2]!
        ldrb r0, [r1, -r2, ror #2]!
        ldrb r0, [r1, r2, rrx]!
        ldrb r0, [r1, -r2, rrx]!
        ldrb r0, [r1], #4
        ldrb r0, [r1], #-4
        ldrb r0, [r1], r2
        ldrb r0, [r1], -r2
        ldrb r0, [r1], r2, lsl #2
        ldrb r0, [r1], -r2, lsl #2
        ldrb r0, [r1], r2, lsr #2
        ldrb r0, [r1], -r2, lsr #2
        ldrb r0, [r1], r2, asr #2
        ldrb r0, [r1], -r2, asr #2
        ldrb r0, [r1], r2, ror #2
        ldrb r0, [r1], -r2, ror #2
        ldrb r0, [r1], r2, rrx
        ldrb r0, [r1], -r2, rrx

        /* LDRBT */
        /* .. condition codes */
        ldreqbt r0, [r1]
        ldrnebt r0, [r1]
        ldrcsbt r0, [r1]
        ldrccbt r0, [r1]
        ldrmibt r0, [r1]
        ldrplbt r0, [r1]
        ldrvsbt r0, [r1]
        ldrvcbt r0, [r1]
        ldrhibt r0, [r1]
        ldrlsbt r0, [r1]
        ldrgebt r0, [r1]
        ldrltbt r0, [r1]
        ldrgtbt r0, [r1]
        ldrlebt r0, [r1]
        ldrbt r0, [r1]
        nop
        /* .. addressing modes */
        ldrbt r0, [r1]
        ldrbt r0, [r1], #4
        ldrbt r0, [r1], #-4
        ldrbt r0, [r1], r2
        ldrbt r0, [r1], -r2
        ldrbt r0, [r1], r2, lsl #2
        ldrbt r0, [r1], -r2, lsl #2
        ldrbt r0, [r1], r2, lsr #2
        ldrbt r0, [r1], -r2, lsr #2
        ldrbt r0, [r1], r2, asr #2
        ldrbt r0, [r1], -r2, asr #2
        ldrbt r0, [r1], r2, ror #2
        ldrbt r0, [r1], -r2, ror #2
        ldrbt r0, [r1], r2, rrx
        ldrbt r0, [r1], -r2, rrx

        /* LDRH */
        /* .. condition codes */
        ldreqh r0, [r1]
        ldrneh r0, [r1]
        ldrcsh r0, [r1]
        ldrcch r0, [r1]
        ldrmih r0, [r1]
        ldrplh r0, [r1]
        ldrvsh r0, [r1]
        ldrvch r0, [r1]
        ldrhih r0, [r1]
        ldrlsh r0, [r1]
        ldrgeh r0, [r1]
        ldrlth r0, [r1]
        ldrgth r0, [r1]
        ldrleh r0, [r1]
        ldrh r0, [r1]
        nop
        /* .. addressing modes */
        ldrh r0, [r1]
        ldrh r0, [r1, #4]
        ldrh r0, [r1, #-4]
        ldrh r0, [r1, r2]
        ldrh r0, [r1, -r2]
        ldrh r0, [r1]!
        ldrh r0, [r1, #4]!
        ldrh r0, [r1, #-4]!
        ldrh r0, [r1, r2]!
        ldrh r0, [r1, -r2]!
        ldrh r0, [r1], #4
        ldrh r0, [r1], #-4
        ldrh r0, [r1], r2
        ldrh r0, [r1], -r2

        /* LDRSB */
        /* .. condition codes */
        ldreqsb r0, [r1]
        ldrnesb r0, [r1]
        ldrcssb r0, [r1]
        ldrccsb r0, [r1]
        ldrmisb r0, [r1]
        ldrplsb r0, [r1]
        ldrvssb r0, [r1]
        ldrvcsb r0, [r1]
        ldrhisb r0, [r1]
        ldrlssb r0, [r1]
        ldrgesb r0, [r1]
        ldrltsb r0, [r1]
        ldrgtsb r0, [r1]
        ldrlesb r0, [r1]
        ldrsb r0, [r1]
        nop
        /* .. addressing modes */
        ldrsb r0, [r1]
        ldrsb r0, [r1, #4]
        ldrsb r0, [r1, #-4]
        ldrsb r0, [r1, r2]
        ldrsb r0, [r1, -r2]
        ldrsb r0, [r1]!
        ldrsb r0, [r1, #4]!
        ldrsb r0, [r1, #-4]!
        ldrsb r0, [r1, r2]!
        ldrsb r0, [r1, -r2]!
        ldrsb r0, [r1], #4
        ldrsb r0, [r1], #-4
        ldrsb r0, [r1], r2
        ldrsb r0, [r1], -r2

        /* LDRSH */
        /* .. condition codes */
        ldreqsh r0, [r1]
        ldrnesh r0, [r1]
        ldrcssh r0, [r1]
        ldrccsh r0, [r1]
        ldrmish r0, [r1]
        ldrplsh r0, [r1]
        ldrvssh r0, [r1]
        ldrvcsh r0, [r1]
        ldrhish r0, [r1]
        ldrlssh r0, [r1]
        ldrgesh r0, [r1]
        ldrltsh r0, [r1]
        ldrgtsh r0, [r1]
        ldrlesh r0, [r1]
        ldrsh r0, [r1]
        nop
        /* .. addressing modes */
        ldrsh r0, [r1]
        ldrsh r0, [r1, #4]
        ldrsh r0, [r1, #-4]
        ldrsh r0, [r1, r2]
        ldrsh r0, [r1, -r2]
        ldrsh r0, [r1]!
        ldrsh r0, [r1, #4]!
        ldrsh r0, [r1, #-4]!
        ldrsh r0, [r1, r2]!
        ldrsh r0, [r1, -r2]!
        ldrsh r0, [r1], #4
        ldrsh r0, [r1], #-4
        ldrsh r0, [r1], r2
        ldrsh r0, [r1], -r2

        /* LDRT */
        /* .. condition codes */
        ldreqt r0, [r1]
        ldrnet r0, [r1]
        ldrcst r0, [r1]
        ldrcct r0, [r1]
        ldrmit r0, [r1]
        ldrplt r0, [r1]
        ldrvst r0, [r1]
        ldrvct r0, [r1]
        ldrhit r0, [r1]
        ldrlst r0, [r1]
        ldrget r0, [r1]
        ldrltt r0, [r1]
        ldrgtt r0, [r1]
        ldrlet r0, [r1]
        ldrt r0, [r1]
        nop
        /* .. addressing modes */
        ldrt r0, [r1]
        ldrt r0, [r1], #4
        ldrt r0, [r1], #-4
        ldrt r0, [r1], r2
        ldrt r0, [r1], -r2
        ldrt r0, [r1], r2, lsl #2
        ldrt r0, [r1], -r2, lsl #2
        ldrt r0, [r1], r2, lsr #2
        ldrt r0, [r1], -r2, lsr #2
        ldrt r0, [r1], r2, asr #2
        ldrt r0, [r1], -r2, asr #2
        ldrt r0, [r1], r2, ror #2
        ldrt r0, [r1], -r2, ror #2
        ldrt r0, [r1], r2, rrx
        ldrt r0, [r1], -r2, rrx

        /* STM(1) */
        /* .. condition codes */
        stmeqia r0, {r1, r2, r3}
        stmneia r0, {r1, r2, r3}
        stmcsia r0, {r1, r2, r3}
        stmccia r0, {r1, r2, r3}
        stmmiia r0, {r1, r2, r3}
        stmplia r0, {r1, r2, r3}
        stmvsia r0, {r1, r2, r3}
        stmvcia r0, {r1, r2, r3}
        stmhiia r0, {r1, r2, r3}
        stmlsia r0, {r1, r2, r3}
        stmgeia r0, {r1, r2, r3}
        stmltia r0, {r1, r2, r3}
        stmgtia r0, {r1, r2, r3}
        stmleia r0, {r1, r2, r3}
        stmia r0, {r1, r2, r3}
        nop
        /* .. addressing modes */
        stmeqib r0, {r1, r2, r3}
        stmneib r0, {r1, r2, r3}
        stmcsib r0, {r1, r2, r3}
        stmccib r0, {r1, r2, r3}
        stmmiib r0, {r1, r2, r3}
        stmplib r0, {r1, r2, r3}
        stmvsib r0, {r1, r2, r3}
        stmvcib r0, {r1, r2, r3}
        stmhiib r0, {r1, r2, r3}
        stmlsib r0, {r1, r2, r3}
        stmgeib r0, {r1, r2, r3}
        stmltib r0, {r1, r2, r3}
        stmgtib r0, {r1, r2, r3}
        stmleib r0, {r1, r2, r3}
        stmib r0, {r1, r2, r3}
        stmeqda r0, {r1, r2, r3}
        stmneda r0, {r1, r2, r3}
        stmcsda r0, {r1, r2, r3}
        stmccda r0, {r1, r2, r3}
        stmmida r0, {r1, r2, r3}
        stmplda r0, {r1, r2, r3}
        stmvsda r0, {r1, r2, r3}
        stmvcda r0, {r1, r2, r3}
        stmhida r0, {r1, r2, r3}
        stmlsda r0, {r1, r2, r3}
        stmgeda r0, {r1, r2, r3}
        stmltda r0, {r1, r2, r3}
        stmgtda r0, {r1, r2, r3}
        stmleda r0, {r1, r2, r3}
        stmda r0, {r1, r2, r3}
        stmeqdb r0, {r1, r2, r3}
        stmnedb r0, {r1, r2, r3}
        stmcsdb r0, {r1, r2, r3}
        stmccdb r0, {r1, r2, r3}
        stmmidb r0, {r1, r2, r3}
        stmpldb r0, {r1, r2, r3}
        stmvsdb r0, {r1, r2, r3}
        stmvcdb r0, {r1, r2, r3}
        stmhidb r0, {r1, r2, r3}
        stmlsdb r0, {r1, r2, r3}
        stmgedb r0, {r1, r2, r3}
        stmltdb r0, {r1, r2, r3}
        stmgtdb r0, {r1, r2, r3}
        stmledb r0, {r1, r2, r3}
        stmdb r0, {r1, r2, r3}
        nop
        /* .. writeback */
        stmia r0!, {r1, r2, r3}
        nop
        /* .. touch sp */
        stmdb   sp!, {r1, r2, r3}
        push    {r1, r2, r3}
        nop
        /* STM(2) */
        stmdb   r0, {r1, r2, r3}^

        /* STR */
        /* .. condition codes */
        streq r0, [r1]
        strne r0, [r1]
        strcs r0, [r1]
        strcc r0, [r1]
        strmi r0, [r1]
        strpl r0, [r1]
        strvs r0, [r1]
        strvc r0, [r1]
        strhi r0, [r1]
        strls r0, [r1]
        strge r0, [r1]
        strlt r0, [r1]
        strgt r0, [r1]
        strle r0, [r1]
        str r0, [r1]
        nop
        /* .. addressing modes */
        str r0, [r1]
        str r0, [r1, #4]
        str r0, [r1, #-4]
        str r0, [r1, r2]
        str r0, [r1, -r2]
        str r0, [r1, r2, lsl #2]
        str r0, [r1, -r2, lsl #2]
        str r0, [r1, r2, lsr #2]
        str r0, [r1, -r2, lsr #2]
        str r0, [r1, r2, asr #2]
        str r0, [r1, -r2, asr #2]
        str r0, [r1, r2, ror #2]
        str r0, [r1, -r2, ror #2]
        str r0, [r1, r2, rrx]
        str r0, [r1, -r2, rrx]
        str r0, [r1]!
        str r0, [r1, #4]!
        str r0, [r1, #-4]!
        str r0, [r1, r2]!
        str r0, [r1, -r2]!
        str r0, [r1, r2, lsl #2]!
        str r0, [r1, -r2, lsl #2]!
        str r0, [r1, r2, lsr #2]!
        str r0, [r1, -r2, lsr #2]!
        str r0, [r1, r2, asr #2]!
        str r0, [r1, -r2, asr #2]!
        str r0, [r1, r2, ror #2]!
        str r0, [r1, -r2, ror #2]!
        str r0, [r1, r2, rrx]!
        str r0, [r1, -r2, rrx]!
        str r0, [r1], #4
        str r0, [r1], #-4
        str r0, [r1], r2
        str r0, [r1], -r2
        str r0, [r1], r2, lsl #2
        str r0, [r1], -r2, lsl #2
        str r0, [r1], r2, lsr #2
        str r0, [r1], -r2, lsr #2
        str r0, [r1], r2, asr #2
        str r0, [r1], -r2, asr #2
        str r0, [r1], r2, ror #2
        str r0, [r1], -r2, ror #2
        str r0, [r1], r2, rrx
        str r0, [r1], -r2, rrx

        /* STRB */
        /* .. condition codes */
        streqb r0, [r1]
        strneb r0, [r1]
        strcsb r0, [r1]
        strccb r0, [r1]
        strmib r0, [r1]
        strplb r0, [r1]
        strvsb r0, [r1]
        strvcb r0, [r1]
        strhib r0, [r1]
        strlsb r0, [r1]
        strgeb r0, [r1]
        strltb r0, [r1]
        strgtb r0, [r1]
        strleb r0, [r1]
        strb r0, [r1]
        nop
        /* .. addressing modes */
        strb r0, [r1]
        strb r0, [r1, #4]
        strb r0, [r1, #-4]
        strb r0, [r1, r2]
        strb r0, [r1, -r2]
        strb r0, [r1, r2, lsl #2]
        strb r0, [r1, -r2, lsl #2]
        strb r0, [r1, r2, lsr #2]
        strb r0, [r1, -r2, lsr #2]
        strb r0, [r1, r2, asr #2]
        strb r0, [r1, -r2, asr #2]
        strb r0, [r1, r2, ror #2]
        strb r0, [r1, -r2, ror #2]
        strb r0, [r1, r2, rrx]
        strb r0, [r1, -r2, rrx]
        strb r0, [r1]!
        strb r0, [r1, #4]!
        strb r0, [r1, #-4]!
        strb r0, [r1, r2]!
        strb r0, [r1, -r2]!
        strb r0, [r1, r2, lsl #2]!
        strb r0, [r1, -r2, lsl #2]!
        strb r0, [r1, r2, lsr #2]!
        strb r0, [r1, -r2, lsr #2]!
        strb r0, [r1, r2, asr #2]!
        strb r0, [r1, -r2, asr #2]!
        strb r0, [r1, r2, ror #2]!
        strb r0, [r1, -r2, ror #2]!
        strb r0, [r1, r2, rrx]!
        strb r0, [r1, -r2, rrx]!
        strb r0, [r1], #4
        strb r0, [r1], #-4
        strb r0, [r1], r2
        strb r0, [r1], -r2
        strb r0, [r1], r2, lsl #2
        strb r0, [r1], -r2, lsl #2
        strb r0, [r1], r2, lsr #2
        strb r0, [r1], -r2, lsr #2
        strb r0, [r1], r2, asr #2
        strb r0, [r1], -r2, asr #2
        strb r0, [r1], r2, ror #2
        strb r0, [r1], -r2, ror #2
        strb r0, [r1], r2, rrx
        strb r0, [r1], -r2, rrx

        /* STRBT */
        /* .. condition codes */
        streqbt r0, [r1]
        strnebt r0, [r1]
        strcsbt r0, [r1]
        strccbt r0, [r1]
        strmibt r0, [r1]
        strplbt r0, [r1]
        strvsbt r0, [r1]
        strvcbt r0, [r1]
        strhibt r0, [r1]
        strlsbt r0, [r1]
        strgebt r0, [r1]
        strltbt r0, [r1]
        strgtbt r0, [r1]
        strlebt r0, [r1]
        strbt r0, [r1]
        nop
        /* .. addressing modes */
        strbt r0, [r1]
        strbt r0, [r1], #4
        strbt r0, [r1], #-4
        strbt r0, [r1], r2
        strbt r0, [r1], -r2
        strbt r0, [r1], r2, lsl #2
        strbt r0, [r1], -r2, lsl #2
        strbt r0, [r1], r2, lsr #2
        strbt r0, [r1], -r2, lsr #2
        strbt r0, [r1], r2, asr #2
        strbt r0, [r1], -r2, asr #2
        strbt r0, [r1], r2, ror #2
        strbt r0, [r1], -r2, ror #2
        strbt r0, [r1], r2, rrx
        strbt r0, [r1], -r2, rrx

        /* STRH */
        /* .. condition codes */
        streqh r0, [r1]
        strneh r0, [r1]
        strcsh r0, [r1]
        strcch r0, [r1]
        strmih r0, [r1]
        strplh r0, [r1]
        strvsh r0, [r1]
        strvch r0, [r1]
        strhih r0, [r1]
        strlsh r0, [r1]
        strgeh r0, [r1]
        strlth r0, [r1]
        strgth r0, [r1]
        strleh r0, [r1]
        strh r0, [r1]
        nop
        /* .. addressing modes */
        strh r0, [r1]
        strh r0, [r1, #4]
        strh r0, [r1, #-4]
        strh r0, [r1, r2]
        strh r0, [r1, -r2]
        strh r0, [r1]!
        strh r0, [r1, #4]!
        strh r0, [r1, #-4]!
        strh r0, [r1, r2]!
        strh r0, [r1, -r2]!
        strh r0, [r1], #4
        strh r0, [r1], #-4
        strh r0, [r1], r2
        strh r0, [r1], -r2

        /* STRT */
        /* .. condition codes */
        streqt r0, [r1]
        strnet r0, [r1]
        strcst r0, [r1]
        strcct r0, [r1]
        strmit r0, [r1]
        strplt r0, [r1]
        strvst r0, [r1]
        strvct r0, [r1]
        strhit r0, [r1]
        strlst r0, [r1]
        strget r0, [r1]
        strltt r0, [r1]
        strgtt r0, [r1]
        strlet r0, [r1]
        strt r0, [r1]
        nop
        /* .. addressing modes */
        strt r0, [r1]
        strt r0, [r1], #4
        strt r0, [r1], #-4
        strt r0, [r1], r2
        strt r0, [r1], -r2
        strt r0, [r1], r2, lsl #2
        strt r0, [r1], -r2, lsl #2
        strt r0, [r1], r2, lsr #2
        strt r0, [r1], -r2, lsr #2
        strt r0, [r1], r2, asr #2
        strt r0, [r1], -r2, asr #2
        strt r0, [r1], r2, ror #2
        strt r0, [r1], -r2, ror #2
        strt r0, [r1], r2, rrx
        strt r0, [r1], -r2, rrx
        
        /* Multiply */
        /* MLA */
        /* .. condition codes */
        mlaeq r0, r1, r2, r3
        mlane r0, r1, r2, r3
        mlacs r0, r1, r2, r3
        mlacc r0, r1, r2, r3
        mlami r0, r1, r2, r3
        mlapl r0, r1, r2, r3
        mlavs r0, r1, r2, r3
        mlavc r0, r1, r2, r3
        mlahi r0, r1, r2, r3
        mlals r0, r1, r2, r3
        mlage r0, r1, r2, r3
        mlalt r0, r1, r2, r3
        mlagt r0, r1, r2, r3
        mlale r0, r1, r2, r3
	    mla r0, r1, r2, r3
        nop
        /* .. write flags */
        mlaeqs r0, r1, r2, r3
        mlanes r0, r1, r2, r3
        mlacss r0, r1, r2, r3
        mlaccs r0, r1, r2, r3
        mlamis r0, r1, r2, r3
        mlapls r0, r1, r2, r3
        mlavss r0, r1, r2, r3
        mlavcs r0, r1, r2, r3
        mlahis r0, r1, r2, r3
        mlalss r0, r1, r2, r3
        mlages r0, r1, r2, r3
        mlalts r0, r1, r2, r3
        mlagts r0, r1, r2, r3
        mlales r0, r1, r2, r3
	    mlas r0, r1, r2, r3

        /* MUL */
        /* .. condition codes */
        muleq r0, r1, r2
        mulne r0, r1, r2
        mulcs r0, r1, r2
        mulcc r0, r1, r2
        mulmi r0, r1, r2
        mulpl r0, r1, r2
        mulvs r0, r1, r2
        mulvc r0, r1, r2
        mulhi r0, r1, r2
        mulls r0, r1, r2
        mulge r0, r1, r2
        mullt r0, r1, r2
        mulgt r0, r1, r2
        mulle r0, r1, r2
	    mul r0, r1, r2
        nop
        /* .. write flags */
        muleqs r0, r1, r2
        mulnes r0, r1, r2
        mulcss r0, r1, r2
        mulccs r0, r1, r2
        mulmis r0, r1, r2
        mulpls r0, r1, r2
        mulvss r0, r1, r2
        mulvcs r0, r1, r2
        mulhis r0, r1, r2
        mullss r0, r1, r2
        mulges r0, r1, r2
        mullts r0, r1, r2
        mulgts r0, r1, r2
        mulles r0, r1, r2
	    muls r0, r1, r2

        /* SMLAL */
        /* .. condition codes */
        smlaleq r0, r1, r2, r3
        smlalne r0, r1, r2, r3
        smlalcs r0, r1, r2, r3
        smlalcc r0, r1, r2, r3
        smlalmi r0, r1, r2, r3
        smlalpl r0, r1, r2, r3
        smlalvs r0, r1, r2, r3
        smlalvc r0, r1, r2, r3
        smlalhi r0, r1, r2, r3
        smlalls r0, r1, r2, r3
        smlalge r0, r1, r2, r3
        smlallt r0, r1, r2, r3
        smlalgt r0, r1, r2, r3
        smlalle r0, r1, r2, r3
	    smlal r0, r1, r2, r3
        nop
        /* .. write flags */
        smlaleqs r0, r1, r2, r3
        smlalnes r0, r1, r2, r3
        smlalcss r0, r1, r2, r3
        smlalccs r0, r1, r2, r3
        smlalmis r0, r1, r2, r3
        smlalpls r0, r1, r2, r3
        smlalvss r0, r1, r2, r3
        smlalvcs r0, r1, r2, r3
        smlalhis r0, r1, r2, r3
        smlallss r0, r1, r2, r3
        smlalges r0, r1, r2, r3
        smlallts r0, r1, r2, r3
        smlalgts r0, r1, r2, r3
        smlalles r0, r1, r2, r3
	    smlals r0, r1, r2, r3

        /* SMULL */
        /* .. condition codes */
        smulleq r0, r1, r2, r3
        smullne r0, r1, r2, r3
        smullcs r0, r1, r2, r3
        smullcc r0, r1, r2, r3
        smullmi r0, r1, r2, r3
        smullpl r0, r1, r2, r3
        smullvs r0, r1, r2, r3
        smullvc r0, r1, r2, r3
        smullhi r0, r1, r2, r3
        smullls r0, r1, r2, r3
        smullge r0, r1, r2, r3
        smulllt r0, r1, r2, r3
        smullgt r0, r1, r2, r3
        smullle r0, r1, r2, r3
	    smull r0, r1, r2, r3
        nop
        /* .. write flags */
        smulleqs r0, r1, r2, r3
        smullnes r0, r1, r2, r3
        smullcss r0, r1, r2, r3
        smullccs r0, r1, r2, r3
        smullmis r0, r1, r2, r3
        smullpls r0, r1, r2, r3
        smullvss r0, r1, r2, r3
        smullvcs r0, r1, r2, r3
        smullhis r0, r1, r2, r3
        smulllss r0, r1, r2, r3
        smullges r0, r1, r2, r3
        smulllts r0, r1, r2, r3
        smullgts r0, r1, r2, r3
        smullles r0, r1, r2, r3
	    smulls r0, r1, r2, r3

        /* UMLAL */
        /* .. condition codes */
        umlaleq r0, r1, r2, r3
        umlalne r0, r1, r2, r3
        umlalcs r0, r1, r2, r3
        umlalcc r0, r1, r2, r3
        umlalmi r0, r1, r2, r3
        umlalpl r0, r1, r2, r3
        umlalvs r0, r1, r2, r3
        umlalvc r0, r1, r2, r3
        umlalhi r0, r1, r2, r3
        umlalls r0, r1, r2, r3
        umlalge r0, r1, r2, r3
        umlallt r0, r1, r2, r3
        umlalgt r0, r1, r2, r3
        umlalle r0, r1, r2, r3
	    umlal r0, r1, r2, r3
        nop
        /* .. write flags */
        umlaleqs r0, r1, r2, r3
        umlalnes r0, r1, r2, r3
        umlalcss r0, r1, r2, r3
        umlalccs r0, r1, r2, r3
        umlalmis r0, r1, r2, r3
        umlalpls r0, r1, r2, r3
        umlalvss r0, r1, r2, r3
        umlalvcs r0, r1, r2, r3
        umlalhis r0, r1, r2, r3
        umlallss r0, r1, r2, r3
        umlalges r0, r1, r2, r3
        umlallts r0, r1, r2, r3
        umlalgts r0, r1, r2, r3
        umlalles r0, r1, r2, r3
	    umlals r0, r1, r2, r3

        /* UMULL */
        /* .. condition codes */
        umulleq r0, r1, r2, r3
        umullne r0, r1, r2, r3
        umullcs r0, r1, r2, r3
        umullcc r0, r1, r2, r3
        umullmi r0, r1, r2, r3
        umullpl r0, r1, r2, r3
        umullvs r0, r1, r2, r3
        umullvc r0, r1, r2, r3
        umullhi r0, r1, r2, r3
        umullls r0, r1, r2, r3
        umullge r0, r1, r2, r3
        umulllt r0, r1, r2, r3
        umullgt r0, r1, r2, r3
        umullle r0, r1, r2, r3
	    umull r0, r1, r2, r3
        nop
        /* .. write flags */
        umulleqs r0, r1, r2, r3
        umullnes r0, r1, r2, r3
        umullcss r0, r1, r2, r3
        umullccs r0, r1, r2, r3
        umullmis r0, r1, r2, r3
        umullpls r0, r1, r2, r3
        umullvss r0, r1, r2, r3
        umullvcs r0, r1, r2, r3
        umullhis r0, r1, r2, r3
        umulllss r0, r1, r2, r3
        umullges r0, r1, r2, r3
        umulllts r0, r1, r2, r3
        umullgts r0, r1, r2, r3
        umullles r0, r1, r2, r3
	    umulls r0, r1, r2, r3
        
        /* Coprocessor */
        /* CDP */
        /* .. condition codes */
        cdpeq p0, 0, cr0, cr1, cr2, 0
        cdpne p0, 0, cr0, cr1, cr2, 0
        cdpcs p0, 0, cr0, cr1, cr2, 0
        cdpcc p0, 0, cr0, cr1, cr2, 0
        cdpmi p0, 0, cr0, cr1, cr2, 0
        cdppl p0, 0, cr0, cr1, cr2, 0
        cdpvs p0, 0, cr0, cr1, cr2, 0
        cdpvc p0, 0, cr0, cr1, cr2, 0
        cdphi p0, 0, cr0, cr1, cr2, 0
        cdpls p0, 0, cr0, cr1, cr2, 0
        cdpge p0, 0, cr0, cr1, cr2, 0
        cdplt p0, 0, cr0, cr1, cr2, 0
        cdpgt p0, 0, cr0, cr1, cr2, 0
        cdple p0, 0, cr0, cr1, cr2, 0
        cdp p0, 0, cr0, cr1, cr2, 0
        cdp2    p0, 0, cr0, cr1, cr2, 0

        /* MCR */
        /* .. condition codes */
        mcreq p0, 0, r0, cr0, cr1, 1
        mcrne p0, 0, r0, cr0, cr1, 1
        mcrcs p0, 0, r0, cr0, cr1, 1
        mcrcc p0, 0, r0, cr0, cr1, 1
        mcrmi p0, 0, r0, cr0, cr1, 1
        mcrpl p0, 0, r0, cr0, cr1, 1
        mcrvs p0, 0, r0, cr0, cr1, 1
        mcrvc p0, 0, r0, cr0, cr1, 1
        mcrhi p0, 0, r0, cr0, cr1, 1
        mcrls p0, 0, r0, cr0, cr1, 1
        mcrge p0, 0, r0, cr0, cr1, 1
        mcrlt p0, 0, r0, cr0, cr1, 1
        mcrgt p0, 0, r0, cr0, cr1, 1
        mcrle p0, 0, r0, cr0, cr1, 1
        mcr p0, 0, r0, cr0, cr1, 1
        mcr2    p0, 0, r0, cr0, cr1, 1

        /* MRC */
        /* .. condition codes */
        mrceq p0, 0, r0, cr0, cr1, 1
        mrcne p0, 0, r0, cr0, cr1, 1
        mrccs p0, 0, r0, cr0, cr1, 1
        mrccc p0, 0, r0, cr0, cr1, 1
        mrcmi p0, 0, r0, cr0, cr1, 1
        mrcpl p0, 0, r0, cr0, cr1, 1
        mrcvs p0, 0, r0, cr0, cr1, 1
        mrcvc p0, 0, r0, cr0, cr1, 1
        mrchi p0, 0, r0, cr0, cr1, 1
        mrcls p0, 0, r0, cr0, cr1, 1
        mrcge p0, 0, r0, cr0, cr1, 1
        mrclt p0, 0, r0, cr0, cr1, 1
        mrcgt p0, 0, r0, cr0, cr1, 1
        mrcle p0, 0, r0, cr0, cr1, 1
        mrc p0, 0, r0, cr0, cr1, 1
        mrc2    p0, 0, r0, cr0, cr1, 1

        /* LDC */
        /* .. condition codes */
        ldceq p0, cr0, [r0]
        ldcne p0, cr0, [r0]
        ldccs p0, cr0, [r0]
        ldccc p0, cr0, [r0]
        ldcmi p0, cr0, [r0]
        ldcpl p0, cr0, [r0]
        ldcvs p0, cr0, [r0]
        ldcvc p0, cr0, [r0]
        ldchi p0, cr0, [r0]
        ldcls p0, cr0, [r0]
        ldcge p0, cr0, [r0]
        ldclt p0, cr0, [r0]
        ldcgt p0, cr0, [r0]
        ldcle p0, cr0, [r0]
        ldc p0, cr0, [r0]
        ldc2 p0, cr0, [r0]

        /* STC */
        /* .. condition codes */
        stceq p0, cr0, [r0]
        stcne p0, cr0, [r0]
        stccs p0, cr0, [r0]
        stccc p0, cr0, [r0]
        stcmi p0, cr0, [r0]
        stcpl p0, cr0, [r0]
        stcvs p0, cr0, [r0]
        stcvc p0, cr0, [r0]
        stchi p0, cr0, [r0]
        stcls p0, cr0, [r0]
        stcge p0, cr0, [r0]
        stclt p0, cr0, [r0]
        stcgt p0, cr0, [r0]
        stcle p0, cr0, [r0]
        stc p0, cr0, [r0]
        stc2 p0, cr0, [r0]
        
        /* Misc */
        /* SWP */
        /* .. condition codes */
        swpeq r0, r1, [r2]
        swpne r0, r1, [r2]
        swpcs r0, r1, [r2]
        swpcc r0, r1, [r2]
        swpmi r0, r1, [r2]
        swppl r0, r1, [r2]
        swpvs r0, r1, [r2]
        swpvc r0, r1, [r2]
        swphi r0, r1, [r2]
        swpls r0, r1, [r2]
        swpge r0, r1, [r2]
        swplt r0, r1, [r2]
        swpgt r0, r1, [r2]
        swple r0, r1, [r2]
        swp r0, r1, [r2]

        /* SWPB */
        /* .. condition codes */
        swpeqb r0, r1, [r2]
        swpneb r0, r1, [r2]
        swpcsb r0, r1, [r2]
        swpccb r0, r1, [r2]
        swpmib r0, r1, [r2]
        swpplb r0, r1, [r2]
        swpvsb r0, r1, [r2]
        swpvcb r0, r1, [r2]
        swphib r0, r1, [r2]
        swplsb r0, r1, [r2]
        swpgeb r0, r1, [r2]
        swpltb r0, r1, [r2]
        swpgtb r0, r1, [r2]
        swpleb r0, r1, [r2]
        swpb r0, r1, [r2]

        /* MRS */
        /* .. condition codes */
        mrseq r0, cpsr
        mrsne r0, spsr
        mrscs r0, cpsr
        mrscc r0, spsr
        mrsmi r0, cpsr
        mrspl r0, spsr
        mrsvs r0, cpsr
        mrsvc r0, spsr
        mrshi r0, cpsr
        mrsls r0, spsr
        mrsge r0, cpsr
        mrslt r0, spsr
        mrsgt r0, cpsr
        mrsle r0, spsr
        mrs r0, cpsr

        /* MSR */
        /* .. condition codes */
        msreq cpsr_fsxc, r0
        msrne spsr_fsxc, r0
        msrcs cpsr_fsxc, r0
        msrcc spsr_fsxc, r0
        msrmi cpsr_fsxc, r0
        msrpl spsr_fsxc, r0
        msrvs cpsr_fsxc, r0
        msrvc spsr_fsxc, r0
        msrhi cpsr_fsxc, r0
        msrls spsr_fsxc, r0
        msrge cpsr_fsxc, r0
        msrlt spsr_fsxc, r0
        msrgt cpsr_fsxc, r0
        msrle spsr_fsxc, r0
        msr cpsr_fsxc, r0
        nop
        /* .. addressing modes */
        msreq cpsr_fsxc, r0
        msrne spsr_fsxc, r0
        msrcs cpsr_fsxc, r0
        msrcc spsr_fsxc, r0
        msrmi cpsr_fsxc, r0
        msrpl spsr_fsxc, r0
        msrvs cpsr_fsxc, r0
        msrvc spsr_fsxc, r0
        msrhi cpsr_fsxc, r0
        msrls spsr_fsxc, r0
        msrge cpsr_fsxc, r0
        msrlt spsr_fsxc, r0
        msrgt cpsr_fsxc, r0
        msrle spsr_fsxc, r0
        msr cpsr_fsxc, r0
        msreq cpsr_fsxc, #4
        msrne spsr_fsxc, #4
        msrcs cpsr_fsxc, #4
        msrcc spsr_fsxc, #4
        msrmi cpsr_fsxc, #4
        msrpl spsr_fsxc, #4
        msrvs cpsr_fsxc, #4
        msrvc spsr_fsxc, #4
        msrhi cpsr_fsxc, #4
        msrls spsr_fsxc, #4
        msrge cpsr_fsxc, #4
        msrlt spsr_fsxc, #4
        msrgt cpsr_fsxc, #4
        msrle spsr_fsxc, #4
        msr cpsr_fsxc, #4
        nop
        /* .. region selection */
        msr cpsr_f, #4
        msr spsr_s, #4
        msr cpsr_x, #4
        msr spsr_c, #4
        msr cpsr_fs, #4
        msr spsr_fx, #4
        msr cpsr_fc, #4
        msr spsr_sx, #4
        msr cpsr_sc, #4
        msr spsr_xc, #4
        msr cpsr_fsx, #4
        msr spsr_fsc, #4
        msr cpsr_fxc, #4
        msr spsr_sxc, #4
        msr cpsr_fsxc, #4

        /* CLZ */
        /* .. condition codes */
        clzeq r0, r1
        clzne r0, r1
        clzcs r0, r1
        clzcc r0, r1
        clzmi r0, r1
        clzpl r0, r1
        clzvs r0, r1
        clzvc r0, r1
        clzhi r0, r1
        clzls r0, r1
        clzge r0, r1
        clzlt r0, r1
        clzgt r0, r1
        clzle r0, r1
        clz r0, r1
        
        /* DSP extension */
        /* LDRD */
        /* .. condition codes */
        ldreqd r0, [r1]
        ldrned r0, [r1]
        ldrcsd r0, [r1]
        ldrccd r0, [r1]
        ldrmid r0, [r1]
        ldrpld r0, [r1]
        ldrvsd r0, [r1]
        ldrvcd r0, [r1]
        ldrhid r0, [r1]
        ldrlsd r0, [r1]
        ldrged r0, [r1]
        ldrltd r0, [r1]
        ldrgtd r0, [r1]
        ldrled r0, [r1]
        ldrd r0, [r1]
        nop
        /* .. addressing modes */
        ldrd r0, [r1]
        ldrd r0, [r1, #4]
        ldrd r0, [r1, #-4]
        ldrd r0, [r1, r2]
        ldrd r0, [r1, -r2]
        ldrd r0, [r1]!
        ldrd r0, [r1, #4]!
        ldrd r0, [r1, #-4]!
        ldrd r0, [r1, r2]!
        ldrd r0, [r1, -r2]!
        ldrd r0, [r1], #4
        ldrd r0, [r1], #-4
        ldrd r0, [r1], r2
        ldrd r0, [r1], -r2

        /* STRD */
        /* .. condition codes */
        streqd r0, [r1]
        strned r0, [r1]
        strcsd r0, [r1]
        strccd r0, [r1]
        strmid r0, [r1]
        strpld r0, [r1]
        strvsd r0, [r1]
        strvcd r0, [r1]
        strhid r0, [r1]
        strlsd r0, [r1]
        strged r0, [r1]
        strltd r0, [r1]
        strgtd r0, [r1]
        strled r0, [r1]
        strd r0, [r1]
        nop
        /* .. addressing modes */
        strd r0, [r1]
        strd r0, [r1, #4]
        strd r0, [r1, #-4]
        strd r0, [r1, r2]
        strd r0, [r1, -r2]
        strd r0, [r1]!
        strd r0, [r1, #4]!
        strd r0, [r1, #-4]!
        strd r0, [r1, r2]!
        strd r0, [r1, -r2]!
        strd r0, [r1], #4
        strd r0, [r1], #-4
        strd r0, [r1], r2
        strd r0, [r1], -r2

        /* MCRR */
        /* .. condition codes */
        mcrreq p0, 0, r0, r1, cr0
        mcrrne p0, 0, r0, r1, cr0
        mcrrcs p0, 0, r0, r1, cr0
        mcrrcc p0, 0, r0, r1, cr0
        mcrrmi p0, 0, r0, r1, cr0
        mcrrpl p0, 0, r0, r1, cr0
        mcrrvs p0, 0, r0, r1, cr0
        mcrrvc p0, 0, r0, r1, cr0
        mcrrhi p0, 0, r0, r1, cr0
        mcrrls p0, 0, r0, r1, cr0
        mcrrge p0, 0, r0, r1, cr0
        mcrrlt p0, 0, r0, r1, cr0
        mcrrgt p0, 0, r0, r1, cr0
        mcrrle p0, 0, r0, r1, cr0
        mcrr p0, 0, r0, r1, cr0

        /* MRRC */
        /* .. condition codes */
        mrrceq p0, 0, r0, r1, cr0
        mrrcne p0, 0, r0, r1, cr0
        mrrccs p0, 0, r0, r1, cr0
        mrrccc p0, 0, r0, r1, cr0
        mrrcmi p0, 0, r0, r1, cr0
        mrrcpl p0, 0, r0, r1, cr0
        mrrcvs p0, 0, r0, r1, cr0
        mrrcvc p0, 0, r0, r1, cr0
        mrrchi p0, 0, r0, r1, cr0
        mrrcls p0, 0, r0, r1, cr0
        mrrcge p0, 0, r0, r1, cr0
        mrrclt p0, 0, r0, r1, cr0
        mrrcgt p0, 0, r0, r1, cr0
        mrrcle p0, 0, r0, r1, cr0
        mrrc p0, 0, r0, r1, cr0

        /* PLD */
        /* .. addressing modes */
        pld [r0]
        pld [r0, #4]
        pld [r0, #-4]
        pld [r0, r1]
        pld [r0, -r1]
        pld [r0, r1, lsl #2]
        pld [r0, -r1, lsl #2]
        pld [r0, r1, lsr #2]
        pld [r0, -r1, lsr #2]
        pld [r0, r1, asr #2]
        pld [r0, -r1, asr #2]
        pld [r0, r1, ror #2]
        pld [r0, -r1, ror #2]
        pld [r0, r1, rrx]
        pld [r0, -r1, rrx]

        /* QADD */
        /* .. condition codes */
        qaddeq r0, r1, r2
        qaddne r0, r1, r2
        qaddcs r0, r1, r2
        qaddcc r0, r1, r2
        qaddmi r0, r1, r2
        qaddpl r0, r1, r2
        qaddvs r0, r1, r2
        qaddvc r0, r1, r2
        qaddhi r0, r1, r2
        qaddls r0, r1, r2
        qaddge r0, r1, r2
        qaddlt r0, r1, r2
        qaddgt r0, r1, r2
        qaddle r0, r1, r2
        qadd r0, r1, r2

        /* QSUB */
        /* .. condition codes */
        qsubeq r0, r1, r2
        qsubne r0, r1, r2
        qsubcs r0, r1, r2
        qsubcc r0, r1, r2
        qsubmi r0, r1, r2
        qsubpl r0, r1, r2
        qsubvs r0, r1, r2
        qsubvc r0, r1, r2
        qsubhi r0, r1, r2
        qsubls r0, r1, r2
        qsubge r0, r1, r2
        qsublt r0, r1, r2
        qsubgt r0, r1, r2
        qsuble r0, r1, r2
        qsub r0, r1, r2

        /* QDADD */
        /* .. condition codes */
        qdaddeq r0, r1, r2
        qdaddne r0, r1, r2
        qdaddcs r0, r1, r2
        qdaddcc r0, r1, r2
        qdaddmi r0, r1, r2
        qdaddpl r0, r1, r2
        qdaddvs r0, r1, r2
        qdaddvc r0, r1, r2
        qdaddhi r0, r1, r2
        qdaddls r0, r1, r2
        qdaddge r0, r1, r2
        qdaddlt r0, r1, r2
        qdaddgt r0, r1, r2
        qdaddle r0, r1, r2
        qdadd r0, r1, r2

        /* QDSUB */
        /* .. condition codes */
        qdsubeq r0, r1, r2
        qdsubne r0, r1, r2
        qdsubcs r0, r1, r2
        qdsubcc r0, r1, r2
        qdsubmi r0, r1, r2
        qdsubpl r0, r1, r2
        qdsubvs r0, r1, r2
        qdsubvc r0, r1, r2
        qdsubhi r0, r1, r2
        qdsubls r0, r1, r2
        qdsubge r0, r1, r2
        qdsublt r0, r1, r2
        qdsubgt r0, r1, r2
        qdsuble r0, r1, r2
        qdsub r0, r1, r2
        
        /* SMLAXY */
        /* .. condition codes */
        smlabbeq r0, r1, r2, r3
        smlabbne r0, r1, r2, r3
        smlabbcs r0, r1, r2, r3
        smlabbcc r0, r1, r2, r3
        smlabbmi r0, r1, r2, r3
        smlabbpl r0, r1, r2, r3
        smlabbvs r0, r1, r2, r3
        smlabbvc r0, r1, r2, r3
        smlabbhi r0, r1, r2, r3
        smlabbls r0, r1, r2, r3
        smlabbge r0, r1, r2, r3
        smlabblt r0, r1, r2, r3
        smlabbgt r0, r1, r2, r3
        smlabble r0, r1, r2, r3
        smlabb r0, r1, r2, r3
        smlabteq r0, r1, r2, r3
        smlabtne r0, r1, r2, r3
        smlabtcs r0, r1, r2, r3
        smlabtcc r0, r1, r2, r3
        smlabtmi r0, r1, r2, r3
        smlabtpl r0, r1, r2, r3
        smlabtvs r0, r1, r2, r3
        smlabtvc r0, r1, r2, r3
        smlabthi r0, r1, r2, r3
        smlabtls r0, r1, r2, r3
        smlabtge r0, r1, r2, r3
        smlabtlt r0, r1, r2, r3
        smlabtgt r0, r1, r2, r3
        smlabtle r0, r1, r2, r3
        smlabt r0, r1, r2, r3
        smlatbeq r0, r1, r2, r3
        smlatbne r0, r1, r2, r3
        smlatbcs r0, r1, r2, r3
        smlatbcc r0, r1, r2, r3
        smlatbmi r0, r1, r2, r3
        smlatbpl r0, r1, r2, r3
        smlatbvs r0, r1, r2, r3
        smlatbvc r0, r1, r2, r3
        smlatbhi r0, r1, r2, r3
        smlatbls r0, r1, r2, r3
        smlatbge r0, r1, r2, r3
        smlatblt r0, r1, r2, r3
        smlatbgt r0, r1, r2, r3
        smlatble r0, r1, r2, r3
        smlatb r0, r1, r2, r3
        smlatteq r0, r1, r2, r3
        smlattne r0, r1, r2, r3
        smlattcs r0, r1, r2, r3
        smlattcc r0, r1, r2, r3
        smlattmi r0, r1, r2, r3
        smlattpl r0, r1, r2, r3
        smlattvs r0, r1, r2, r3
        smlattvc r0, r1, r2, r3
        smlatthi r0, r1, r2, r3
        smlattls r0, r1, r2, r3
        smlattge r0, r1, r2, r3
        smlattlt r0, r1, r2, r3
        smlattgt r0, r1, r2, r3
        smlattle r0, r1, r2, r3
        smlatt r0, r1, r2, r3

        /* SMLALXY */
        /* .. condition codes */
        smlalbbeq r0, r1, r2, r3
        smlalbbne r0, r1, r2, r3
        smlalbbcs r0, r1, r2, r3
        smlalbbcc r0, r1, r2, r3
        smlalbbmi r0, r1, r2, r3
        smlalbbpl r0, r1, r2, r3
        smlalbbvs r0, r1, r2, r3
        smlalbbvc r0, r1, r2, r3
        smlalbbhi r0, r1, r2, r3
        smlalbbls r0, r1, r2, r3
        smlalbbge r0, r1, r2, r3
        smlalbblt r0, r1, r2, r3
        smlalbbgt r0, r1, r2, r3
        smlalbble r0, r1, r2, r3
        smlalbb r0, r1, r2, r3
        smlalbteq r0, r1, r2, r3
        smlalbtne r0, r1, r2, r3
        smlalbtcs r0, r1, r2, r3
        smlalbtcc r0, r1, r2, r3
        smlalbtmi r0, r1, r2, r3
        smlalbtpl r0, r1, r2, r3
        smlalbtvs r0, r1, r2, r3
        smlalbtvc r0, r1, r2, r3
        smlalbthi r0, r1, r2, r3
        smlalbtls r0, r1, r2, r3
        smlalbtge r0, r1, r2, r3
        smlalbtlt r0, r1, r2, r3
        smlalbtgt r0, r1, r2, r3
        smlalbtle r0, r1, r2, r3
        smlalbt r0, r1, r2, r3
        smlaltbeq r0, r1, r2, r3
        smlaltbne r0, r1, r2, r3
        smlaltbcs r0, r1, r2, r3
        smlaltbcc r0, r1, r2, r3
        smlaltbmi r0, r1, r2, r3
        smlaltbpl r0, r1, r2, r3
        smlaltbvs r0, r1, r2, r3
        smlaltbvc r0, r1, r2, r3
        smlaltbhi r0, r1, r2, r3
        smlaltbls r0, r1, r2, r3
        smlaltbge r0, r1, r2, r3
        smlaltblt r0, r1, r2, r3
        smlaltbgt r0, r1, r2, r3
        smlaltble r0, r1, r2, r3
        smlaltb r0, r1, r2, r3
        smlaltteq r0, r1, r2, r3
        smlalttne r0, r1, r2, r3
        smlalttcs r0, r1, r2, r3
        smlalttcc r0, r1, r2, r3
        smlalttmi r0, r1, r2, r3
        smlalttpl r0, r1, r2, r3
        smlalttvs r0, r1, r2, r3
        smlalttvc r0, r1, r2, r3
        smlaltthi r0, r1, r2, r3
        smlalttls r0, r1, r2, r3
        smlalttge r0, r1, r2, r3
        smlalttlt r0, r1, r2, r3
        smlalttgt r0, r1, r2, r3
        smlalttle r0, r1, r2, r3
        smlaltt r0, r1, r2, r3

        /* SMLAWY */
        /* .. condition codes */
        smlawbeq r0, r1, r2, r3
        smlawbne r0, r1, r2, r3
        smlawbcs r0, r1, r2, r3
        smlawbcc r0, r1, r2, r3
        smlawbmi r0, r1, r2, r3
        smlawbpl r0, r1, r2, r3
        smlawbvs r0, r1, r2, r3
        smlawbvc r0, r1, r2, r3
        smlawbhi r0, r1, r2, r3
        smlawbls r0, r1, r2, r3
        smlawbge r0, r1, r2, r3
        smlawblt r0, r1, r2, r3
        smlawbgt r0, r1, r2, r3
        smlawble r0, r1, r2, r3
        smlawb r0, r1, r2, r3
        smlawteq r0, r1, r2, r3
        smlawtne r0, r1, r2, r3
        smlawtcs r0, r1, r2, r3
        smlawtcc r0, r1, r2, r3
        smlawtmi r0, r1, r2, r3
        smlawtpl r0, r1, r2, r3
        smlawtvs r0, r1, r2, r3
        smlawtvc r0, r1, r2, r3
        smlawthi r0, r1, r2, r3
        smlawtls r0, r1, r2, r3
        smlawtge r0, r1, r2, r3
        smlawtlt r0, r1, r2, r3
        smlawtgt r0, r1, r2, r3
        smlawtle r0, r1, r2, r3
        smlawt r0, r1, r2, r3

        /* SMULXY */
        /* .. condition codes */
        smulbbeq r0, r1, r2
        smulbbne r0, r1, r2
        smulbbcs r0, r1, r2
        smulbbcc r0, r1, r2
        smulbbmi r0, r1, r2
        smulbbpl r0, r1, r2
        smulbbvs r0, r1, r2
        smulbbvc r0, r1, r2
        smulbbhi r0, r1, r2
        smulbbls r0, r1, r2
        smulbbge r0, r1, r2
        smulbblt r0, r1, r2
        smulbbgt r0, r1, r2
        smulbble r0, r1, r2
        smulbb r0, r1, r2
        smulbteq r0, r1, r2
        smulbtne r0, r1, r2
        smulbtcs r0, r1, r2
        smulbtcc r0, r1, r2
        smulbtmi r0, r1, r2
        smulbtpl r0, r1, r2
        smulbtvs r0, r1, r2
        smulbtvc r0, r1, r2
        smulbthi r0, r1, r2
        smulbtls r0, r1, r2
        smulbtge r0, r1, r2
        smulbtlt r0, r1, r2
        smulbtgt r0, r1, r2
        smulbtle r0, r1, r2
        smulbt r0, r1, r2
        smultbeq r0, r1, r2
        smultbne r0, r1, r2
        smultbcs r0, r1, r2
        smultbcc r0, r1, r2
        smultbmi r0, r1, r2
        smultbpl r0, r1, r2
        smultbvs r0, r1, r2
        smultbvc r0, r1, r2
        smultbhi r0, r1, r2
        smultbls r0, r1, r2
        smultbge r0, r1, r2
        smultblt r0, r1, r2
        smultbgt r0, r1, r2
        smultble r0, r1, r2
        smultb r0, r1, r2
        smultteq r0, r1, r2
        smulttne r0, r1, r2
        smulttcs r0, r1, r2
        smulttcc r0, r1, r2
        smulttmi r0, r1, r2
        smulttpl r0, r1, r2
        smulttvs r0, r1, r2
        smulttvc r0, r1, r2
        smultthi r0, r1, r2
        smulttls r0, r1, r2
        smulttge r0, r1, r2
        smulttlt r0, r1, r2
        smulttgt r0, r1, r2
        smulttle r0, r1, r2
        smultt r0, r1, r2

        /* SMULWY */
        /* .. condition codes */
        smulwbeq r0, r1, r2
        smulwbne r0, r1, r2
        smulwbcs r0, r1, r2
        smulwbcc r0, r1, r2
        smulwbmi r0, r1, r2
        smulwbpl r0, r1, r2
        smulwbvs r0, r1, r2
        smulwbvc r0, r1, r2
        smulwbhi r0, r1, r2
        smulwbls r0, r1, r2
        smulwbge r0, r1, r2
        smulwblt r0, r1, r2
        smulwbgt r0, r1, r2
        smulwble r0, r1, r2
        smulwb r0, r1, r2
        smulwteq r0, r1, r2
        smulwtne r0, r1, r2
        smulwtcs r0, r1, r2
        smulwtcc r0, r1, r2
        smulwtmi r0, r1, r2
        smulwtpl r0, r1, r2
        smulwtvs r0, r1, r2
        smulwtvc r0, r1, r2
        smulwthi r0, r1, r2
        smulwtls r0, r1, r2
        smulwtge r0, r1, r2
        smulwtlt r0, r1, r2
        smulwtgt r0, r1, r2
        smulwtle r0, r1, r2
        smulwt r0, r1, r2
        
        nop
        nop
        nop
        nop
        nop
        .end
        