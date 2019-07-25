int hang_wit ()

{       char *b2; char *b3; int d,x,a;

        x=0;
        if (bord3[wkveld]) { y=33; d=wkveld; goto hw20; }

hw10:   x=x+1; d=veldwz[x];
        if (d==-1) return 0;

        b2=bord2+d; b3=bord3+d;
        sc=bord[d]; y=sw[sc]; if (y==1) goto pswit;

hwnext: if (b3[0]==0) goto hw10;
        a=sc<<16; a=a+(b2[0]<<8); a=a+b3[0]; y=hang_tab[a];     // SEE
        if (y==0) goto hw10;                                    // ok

hw20:   if (wpnt1 == 0) goto hw30;
        if (w3 < y)  { w6=w3; w3=y; w2=d; goto hw10; }
        if (w6 < y) w6=y;
        goto hw10;

hw30:   if (z3 <  y)  { z6=z3; z5=z2; z3=y; z2=d; goto hw10; }
        if (z6 <= y)  { z6=y;  z5=d; }
        goto hw10;

pswit:  if (b2[1] & 8)  { r2=1; formpawn=formpawn+wpform6[d]; goto psw35; }
        if (b2[0] & 8)  { r2=1; formpawn=formpawn+wpform[d]; goto psw35; }
        if (b2[-1] & 8) { formpawn=formpawn+wpform[d]; goto psw30; }
        if (b2[-2] & 8)  goto psw11;

psw10:  formpawn=formpawn-wiso[d];

psw11:  z=alf[d];
        if (wpnum[z] >= zpnum[z]) { if (xwz>=3) formpawn=formpawn-wpol[d];
                                    else        formpawn=formpawn-wpol[d]/2; }
        if (wpdup[z] < 2) goto psw30;
        if (wpnumm1[z] != 9) goto psw30;
        if (wpnump1[z] != 9) goto psw30;
        if (zpnum[z]==0) formpawn=formpawn-30; else formpawn=formpawn-20;

psw30:  r2=0;                                   // pion heeft GEEN structuur
        xr3=b3[0]; z=ksw[xr3];
        if (z < 2) goto psw35;                  // geen aanv ... wp x zp
        xr4=b2[0];
        if (z > ksw[xr4]) goto psw35;           // cancel wpd4/wle3 <> ztd8
        r5=b3[0] & 7; r5=r5 << 1;               // aantal aanvallers * 2
        r7=wblok[d];                            // faktor

        if (xwz != 4) { r7=xwz; r5=r5+r5; }     // dubbel in eindspel
        r5=r5 << r7; formpawn=formpawn - r5;



//      Test op vrypion
//      ===============

psw35:  z=alf[d];
        if (num[d] < zpnum[z])   goto hwnext;
        if (num[d] < zpnump1[z]) goto hwnext;
        if (num[d] < zpnumm1[z]) goto hwnext;

psw38:  if (xz == 0) goto wkwat;

psw40:  z=wpvry[d]; if (xwz<=2) z=z+z;          // double in endgame
        paspawn=paspawn+z;                      // standaard waardering
        if (r2) paspawn=paspawn+z;              // indien structuur

        if (bordp1[d] != LL) goto psw60;        // veld bezet voor vp

psw42:  if (b3[1] == 0) goto psw43;
        if (b2[1] == 0) goto psw70;             // niets

psw43:  if (bordp2[d] <  LL) goto psw46;        // 4x
        if (bordp2[d] != LL) goto psw48;        // 1x
        if (b3[2] == 0) goto psw44;
        if (b2[2] == 0) goto psw48;             // 1x
psw44:  if (bordp3[d] <  LL) goto psw46;        // 4x
        if (bordp3[d] != LL) goto psw47;        // 2x
        if (b3[3] == 0) goto psw46;             // 4x
        if (b2[3] == 0) goto psw47;             // 2x
psw46:  z=z+z;
psw47:  z=z+z;
psw48:  paspawn=paspawn+z; goto psw70;


psw60:  if (bordp1[d] < ZP)  { z=z >>1; goto psw43; }   // eigen stuk
        if (b2[0] & 6)  paspawn=paspawn+z;              // ondersteuning
        if (b2[1] != 0) paspawn=paspawn+z;              // veld ervoor

psw70:  if (xwz==4) goto hwnext;
        if (b2[1] & 0xffffff80) { paspawn=paspawn+wksluit[wkveld]; goto hwnext; }
        if (b2[0] & 0xffffff80) { paspawn=paspawn+wksluit[wkveld]; goto hwnext; }
        goto hwnext;














wkwat:  pawn=d;                                 // bewaar pionveld
        v=zp1[d]; if (v==6) v=5;                // <wp> tot promveld
        if (wpnt1==0) v=v+1;
        e=alf[zkveld]-alf[d]; if (e<0) { e=e^-1; e=e+1; }
        a=7-num[zkveld]; if (a<e) a=e;
        a=a-v; if (a<=0) goto kw40;             // <zk> in kwadraat
        r5=num[d]; if (r5==1) r5=2;             // <r5> is kandidaat <wpval>
        e=d;                                    // wpveld

kw32:   e=e+1; if (bord[e] == LL) goto kw32;    // test vrye baan pion
        if (bord[e] == RR) goto kw34;           // tot hier toe natuurlyk
        r5=r5-1;                                // corrigeer kwadraat waarde
        a=a-1; if (a<=0) goto kw40;             // kwadraat voldoet niet meer
        goto kw32;                              // test verder op ruim baan

kw34:   if (r5 < wpval) goto psw40;             // er is al een betere
        wpval=r5; e=e-1; wpprom=e;              // promveld (wp)
        wpveld=d; quadrant=1;
        if (num[d] != 6) goto hwnext;           // wp niet op 7e ry ... klaar
        if (bord[e] != LL) goto hwnext;         // promveld gevuld ... klaar
//      prom=wpnt1;                             // ivm Afruil
        goto hwnext;

kw40:   if (wpnt1 != 0) goto psw40;             // geen <h3> update Plaus
        if (bord2p1[d] <= 127) goto psw40;      // veld voor <wp> geen <wk>
        if (zp1[zkveld] < zp1[d]) goto psw40;
        if (zp1[zkveld] < zp1[wkveld]) goto psw40;
        h3=0xc00;                               // <h3> = 12 pionnen update
        goto psw40;

        return 0; }























int hang_zwart ()

{       char *b2; char *b3; int d,x,a;

        x=zpnt3;

        if (bord2[zkveld] !=0) { y=33; d=zkveld; goto hz20; }

hz10:   x=x+1; d=veldwz[x];
        if (d==-1) return 0;

        b2=bord2+d; b3=bord3+d;
        sc=bord[d]; y=sw[sc]; if (y==1) goto pszwart;

hznext: if (b2[0]==0) goto hz10;
        a=sc<<16; a=a+(b2[0]<<8); a=a+b3[0]; y=hang_tab[a];     // SEE
        if (y==0) goto hz10;                                    // ok

hz20:   if (wpnt1 != 0) goto hz30;
        if (w3 < y)  { w6=w3; w3=y; w2=d; goto hz10; }
        if (w6 < y) w6=y;
        goto hz10;

hz30:   if (z3 <  y)  { z6=z3; z5=z2; z3=y; z2=d; goto hz10; }
        if (z6 <= y)  { z6=y;  z5=d; }
        goto hz10;

pszwart: if (b3[-1] & 8) { r2=1; formpawn=formpawn-zpform6[d]; goto psz35; }
         if (b3[0] & 8)   { r2=1; formpawn=formpawn-zpform[d]; goto psz35; }
         if (b3[1] & 8)   { formpawn=formpawn-zpform[d]; goto psz30; }
         if (b3[2] & 8)  goto psz11;
psz10:  formpawn=formpawn + ziso[d];

psz11:  z=alf[d];
        if (wpnum[z] >= zpnum[z]) { if (xwz>=3) formpawn=formpawn+zpol[d];
                                    else        formpawn=formpawn+zpol[d]/2; }
        if (zpdup[z] < 2) goto psz30;
        if (zpnumm1[z] != 0) goto psz30;
        if (zpnump1[z] != 0) goto psz30;
        if (wpnum[z]==9) formpawn=formpawn+30; else formpawn=formpawn+20;

psz30:  r2=0;                                       // pion heeft GEEN structuur
        xr3=b2[0]; z=ksw[xr3];
        if (z < 2) goto psz35;                      // geen aanv ... wp x zp
        xr4=b3[0];
        if (z > ksw[xr4]) goto psz35;               // cancel zpd5/zle6 <> wtd1
        r5=b2[0] & 7; r5=r5 << 1;                   // aantal aanvallers * 2
        r7=zblok[d];                                // faktor
        if (xwz != 4) { r7=xwz; r5=r5+r5; }         // dubbel in eindspel
        r5=r5 << r7; formpawn=formpawn + r5;




//      Test op vrypion
//      ===============

psz35:  z=alf[d];
        if (num[d] > wpnum[z])   goto hznext;
        if (num[d] > wpnump1[z]) goto hznext;
        if (num[d] > wpnumm1[z]) goto hznext;

psz38:  if (xw == 0) goto zkwat;

psz40:  z=zpvry[d]; if (xwz<=2) z=z+z;
        paspawn=paspawn-z;                          // standaard waardering
        if (r2) paspawn=paspawn-z;                  // indien structuur

        if (bordm1[d] != LL) goto psz60;            // veld bezet voor vp

psz42:  if (b2[-1] == 0) goto psz43;
        if (b3[-1] == 0) goto psz70;                // niets
psz43:  if (bordm2[d] <  LL) goto psz46;            // 4x
        if (bordm2[d] != LL) goto psz48;            // 1x
        if (b2[-2] == 0) goto psz44;
        if (b3[-2] == 0) goto psz48;                // 1x
psz44:  if (bordm3[d] <  LL) goto psz46;            // 4x
        if (bordm3[d] != LL) goto psz47;            // 2x
        if (b2[-3] == 0) goto psz46;                // 4x
        if (b3[-3] == 0) goto psz47;                // 2x
psz46:  z=z+z;
psz47:  z=z+z;
psz48:  paspawn=paspawn-z; goto psz70;

psz60:  if (bordm1[d] >= ZP)  { z=z >>1; goto psz43; }  // eigen stuk
        if (b3[0] & 6)   paspawn=paspawn-z;             // ondersteuning
        if (b3[-1] != 0) paspawn=paspawn-z;             // veld ervoor

psz70:  if (xwz==4) goto hznext;
        if (b3[-1] & 0xffffff80) { paspawn=paspawn-zksluit[zkveld]; goto hznext; }
        if (b3[0]  & 0xffffff80) { paspawn=paspawn-zksluit[zkveld]; goto hznext; }
        goto hznext;
















zkwat:  pawn=d;                                     // bewaar pionveld
        v=num[d]; if (v==6) v=5;                    // <zp> tot promveld
        if (wpnt1 != 0) v=v+1;
        e=alf[wkveld]-alf[d]; if (e<0) { e=e^-1; e=e+1; }
        a=num[wkveld]; if (a<e) a=e;
        a=a-v; if (a<=0) goto kz40;                 // <wk> in kwadraat
        r14=zp1[d]; if (r14==1) r14=2;              // <r14> is kandidaat <zpval>
        e=d;                                        // zpveld

kz32:   e=e-1; if (bord[e] == LL) goto kz32;        // test vrye baan pion
        if (bord[e] == RR) goto kz34;               // tot hier toe natuurlyk
        r14=r14-1;                                  // corrigeer kwadraat waarde
        a=a-1; if (a<=0) goto kz40;                 // kwadraat voldoet niet meer
        goto kz32;                                  // test verder op ruim baan

kz34:   if (r14 < zpval) goto psz40;                // er is al een betere
        zpval=r14; e=e+1; zpprom=e;                 // promveld (zp)
        zpveld=d; quadrant=2;
        if (num[d] != 1) goto hznext;               // zp niet op 2e ry ... klaar
        if (bord[e] != LL) goto hznext;             // promveld gevuld ... klaar
//      if (zpnt1 != 0) prom=1;                     // ivm Afruil
        goto hznext;

kz40:   if (wpnt1 == 0) goto psz40;                 // geen <h3> update Plaus
        if (bord3m1[d] <= 127) goto psz40;          // veld voor <zp> geen <zk>
        if (num[wkveld] < num[d]) goto psz40;
        if (num[wkveld] < num[zkveld]) goto psz40;
        h3=0xc00;                                   // <h3> = 12 pionnen update
        goto psz40;

        return 0; }























int zksave ()

{       int y; char *brd; char *b2; char *b3; int e;

        y=zkcor[zkveld]; if (xw < 15) y=0;
        d=zkveld;
        brd=bord+d; b2=bord2+d; b3=bord3+d;

        if (b2[9]==0)     goto zks10;
        if (brd[9]==RR)   goto zks10;
        y=y+1; if (brd[9]==LL) y=y+1;
        if (b3[9]== -127) y=y+1;

zks10:  if (b2[-1]==0)     goto zks20;
        if (brd[-1]==RR)   goto zks20;
        y=y+1; if (brd[-1]==LL) y=y+1;
        if (b3[-1]== -127) y=y+1;

zks20:  if (b2[-11]==0)     goto zks30;
        if (brd[-11]==RR)   goto zks30;
        y=y+1; if (brd[-11]==LL) y=y+1;
        if (b3[-11]== -127) y=y+1;

zks30:  if (b2[1]==0)     goto zks40;
        if (brd[1]==RR)   goto zks40;
        y=y+1; if (b3[1]== -127) y=y+1;

zks40:  if (b2[10]==0)     goto zks50;
        if (brd[10]==RR)   goto zks50;
        y=y+1; if (b3[10]== -127) y=y+1;

zks50:  if (b2[11]==0)     goto zks60;
        if (brd[11]==RR)   goto zks60;
        y=y+1; if (b3[11]== -127) y=y+1;

zks60:  if (b2[-9]==0)     goto zks70;
        if (brd[-9]==RR)   goto zks70;
        y=y+1; if (b3[-9]== -127) y=y+1;

zks70:  if (b2[-10]==0)     goto zks80;
        if (brd[-10]==RR)   goto zks80;
        y=y+1; if (b3[-10]== -127) y=y+1;

zks80:  kingsave=ks[y];

ex15n:  if (kequ[d]==kequ[wkveld]) return 0;        // evaluate when opposite castling
        y=kcor[d]; e=wpnumm1[y]; kingsave=kingsave+wpaf[e];
                   e=wpnum[y]  ; kingsave=kingsave+wpbg[e];
        e=wpnump1[y]; kingsave=kingsave+wpbg[e];

        return 0;
}


int wksave ()

{       int y; char *brd; char *b2; char *b3; int e;

        y=wkcor[wkveld]; if (xz < 15) y=0;
        d=wkveld;
        brd=bord+d; b2=bord2+d; b3=bord3+d;

        if (b3[-9]==0)     goto wks10;
        if (brd[-9]==RR)   goto wks10;
        y=y+1; if (brd[-9]==LL) y=y+1;
        if (b2[-9]== -127) y=y+1;

wks10:  if (b3[1]==0)     goto wks20;
        if (brd[1]==RR)   goto wks20;
        y=y+1; if (brd[1]==LL) y=y+1;
        if (b2[1]== -127) y=y+1;

wks20:  if (b3[11]==0)     goto wks30;
        if (brd[11]==RR)   goto wks30;
        y=y+1; if (brd[11]==LL) y=y+1;
        if (b2[11]== -127) y=y+1;

wks30:  if (b3[-1]==0)     goto wks40;
        if (brd[-1]==RR)   goto wks40;
        y=y+1; if (b2[-1]== -127) y=y+1;

wks40:  if (b3[10]==0)     goto wks50;
        if (brd[10]==RR)   goto wks50;
        y=y+1; if (b2[10]== -127) y=y+1;

wks50:  if (b3[-11]==0)     goto wks60;
        if (brd[-11]==RR)   goto wks60;
        y=y+1; if (b2[-11]== -127) y=y+1;

wks60:  if (b3[9]==0)     goto wks70;
        if (brd[9]==RR)   goto wks70;
        y=y+1; if (b2[9]== -127) y=y+1;

wks70:  if (b3[-10]==0)     goto wks80;
        if (brd[-10]==RR)   goto wks80;
        y=y+1; if (b2[-10]== -127) y=y+1;

wks80:  kingsave=kingsave-ks[y];

ex16n:  if (kequ[d]==kequ[zkveld]) return 0;        // ongelyke koningen
        y=kcor[d]; e=zpnumm1[y]; kingsave=kingsave-zpaf[e];
                   e=zpnum[y]  ; kingsave=kingsave-zpbg[e];
        e=zpnump1[y]; kingsave=kingsave-zpbg[e];

        return 0;
}

//  Einde verhaal









