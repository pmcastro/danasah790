int kwadraat ()

{		int x,v,r2;

		if (wpval == zpval) goto kp50;				// gelyke race (1)

        if (wpnt1==0) r6=wpval-1; else r6=wpval+1;
        if (r6 == zpval) goto kp60;					// gelyke race (2)

		if (wpval >= zpval) wpos=wpos+0x800;		// bonus voor wit
        	else	    	wpos=wpos-0x800;		// bonus voor zwart

		return 0;

//	Test by gelijke race of wit promoveert met schaak
//	=================================================

kp50:	if (wpnt1 == 0) goto kp62;					// test (zwart)

kp52:	x=wpprom-zkveld;							// promveld - zk
		x=x & 0xff; 								// naar 8 bit
        v=dtab[x]; if (v == 0) return 0;			// geen schaakpromotie
	        
		if (v >= 128) v= v | 0xffffff00;			// convert naar 32 bit

		r2=wpprom;									// zoek nu vanaf 
kp54:	r2=r2-v; if (bord[r2] == LL) goto kp54;		// promveld naar <zk>
        if (r2 == wpveld) goto kp54;				// eigen vp is ok !!
        if (r2 == zkveld) wpos=wpos+0x800;			// promotie schaak !!

        return 0;


//	Test by gelijke race of zwart promoveert met schaak
//	===================================================

kp60:	if (wpnt1 == 0) goto kp52;					// test (wit)

kp62:	x=zpprom-wkveld;							// promveld - wk
		x=x & 0xff; 								// naar 8 bit
        v=dtab[x]; if (v == 0) return 0;			// geen schaakpromotie
	        
		if (v >= 128) v= v | 0xffffff00;			// convert naar 32 bit

		r2=zpprom;									// zoek nu vanaf 
kp64:	r2=r2-v; if (bord[r2] == LL) goto kp64;		// promveld naar <wk>
        if (r2 == zpveld) goto kp64;				// eigen vp is ok !!
        if (r2 == wkveld) wpos=wpos-0x800;			// promotie schaak !!

        return 0; }


// 	Einde verhaal

