int wp ()

{       char *q; char *p;
        wpawn=d;

        q=bord2m9+d; q[0]=++q[0] | 8;           // -9
        q=q+20;      q[0]=++q[0] | 8;           // 11

        wmat=wmat+hsw[WP];
        if (white_castle) mg=mg+wpl[d];
         else { mg=mg+wpm[d]; eg=eg+wpe[d]; }

        p=vwz;

wp40:   p=p+1;
        if (p[0] ==  -1)  goto einde;
        q=p[0]+bord2m9; q[0]=++q[0] | 8;        // -9
        q=q+20;         q[0]=++q[0] | 8;        // 11

        d=p[0]; wmat=wmat+hsw[WP];
        if (white_castle) mg=mg+wpl[d];
        else { mg=mg+wpm[d]; eg=eg+wpe[d]; }

        goto wp40;

einde:  vwz=p;

        return 0;
}

























int zp ()

{       char *q; char *p;
        zpawn=d;

        q=bord3p9+d; q[0]=++q[0] | 8;           // + 9
        q=q-20;      q[0]=++q[0] | 8;           // -11

        wmat=wmat-hsw[ZP];
        if (black_castle) mg=mg-zpl[d];
        else { mg=mg-zpm[d]; eg=eg-zpe[d]; }

        p=vwz;

zp40:   p=p+1;
        if (p[0] ==  -1)  goto einde;
        q=p[0]+bord3p9; q[0]=++q[0] | 8;        // + 9
        q=q-20;         q[0]=++q[0] | 8;        // -11

        d=p[0]; wmat=wmat-hsw[ZP];
        if (black_castle) mg=mg-zpl[d];
        else { mg=mg-zpm[d]; eg=eg-zpe[d]; }

        goto zp40;

einde:  vwz=p;

        return 0;
}

























int wk()

{       char *q;

        q=bord2+d;
        q[1]=129;  q[9]=129;  q[10]=129;  q[11]=129;
        q[-1]=129; q[-9]=129; q[-10]=129; q[-11]=129;

        if (xwz==3) mg=mg+wko[d];
        if (xwz==4) mg=mg+wkm[d];
        eg=eg+wkzk[d];

        return 0;
}


int zk()

{       char *q;

        q=bord3+d;
        q[1]=129;  q[9]=129;  q[10]=129;  q[11]=129;
        q[-1]=129; q[-9]=129; q[-10]=129; q[-11]=129;

        if (xwz==3) mg=mg-wko[d];
        if (xwz==4) mg=mg-zkm[d];
        eg=eg-wkzk[d];

        return 0;
}
























int wn ()

{       char *q; char h1; char *p;

        wmat=wmat+hsw[WN];
        mg=mg+wnm[d];
        eg=eg+wne[d];

        q=bord2p8+d;

                q[0]=++q[0] | 16;                   // + 8
        q=q+4;  q[0]=++q[0] | 16;                   // +12
        q=q+7;  q[0]=++q[0] | 16;                   // +19
        q=q+2;  q[0]=++q[0] | 16;                   // +21
        q=q-42; q[0]=++q[0] | 16;                   // -21
        q=q+2;  q[0]=++q[0] | 16;                   // -19
        q=q+7;  q[0]=++q[0] | 16;                   // -12
        q=q+4;  q[0]=++q[0] | 16;                   // - 8

        if (wsom &8) wsom=wsom | 0x7f; else wsom=wsom+4;

        p=pveld+d;                                  // point naar pveld,x
        q=bord+d;                                   // point naar bord,x

wn00:   h1=wntab[q[8]]; if (h1==0) goto wn10;       // rand/eigen stuk
                        if (h1==1) goto wn02;       // zn..zk = bonus
        if (q[-1]==ZP)  goto wn10;                  // zp = geen bonus
        if (q[19]==ZP)  goto wn10;                  // zp = geen bonus
wn02:   pmob=pmob+p[8];                             // mob + bonus

wn10:   h1=wntab[q[12]]; if (h1==0) goto wn20;
                         if (h1==1) goto wn12;
        if (q[3]==ZP)  goto wn20;
        if (q[23]==ZP) goto wn20;
wn12:   pmob=pmob+p[12];

wn20:   h1=wntab[q[19]]; if (h1==0) goto wn30;
                         if (h1==1) goto wn22;
        if (q[10]==ZP)  goto wn30;
        if (q[30]==ZP)  goto wn30;
wn22:   pmob=pmob+p[19];


wn30:   h1=wntab[q[21]]; if (h1==0) goto wn40;
                         if (h1==1) goto wn32;
        if (q[12]==ZP)  goto wn40;
        if (q[32]==ZP)  goto wn40;
wn32:   pmob=pmob+p[21];






wn40:   h1=wntab[q[-8]]; if (h1==0) goto wn50;
                         if (h1==1) goto wn42;
        if (q[-17]==ZP)  goto wn50;
        if (q[3]==ZP)    goto wn50;
wn42:   pmob=pmob+p[-8];

wn50:   h1=wntab[q[-12]]; if (h1==0) goto wn60;
                          if (h1==1) goto wn52;
        if (q[-21]==ZP)  goto wn60;
        if (q[-1]==ZP)   goto wn60;
wn52:   pmob=pmob+p[-12];

wn60:   h1=wntab[q[-19]]; if (h1==0) goto wn70;
                          if (h1==1) goto wn62;
        if (q[-28]==ZP)  goto wn70;
        if (q[-8]==ZP)   goto wn70;
wn62:   pmob=pmob+p[-19];

wn70:   h1=wntab[q[-21]]; if (h1==0) goto wn80;
                          if (h1==1) goto wn72;
        if (q[-30]==ZP)  goto wn80;
        if (q[-10]==ZP)  goto wn80;
wn72:   pmob=pmob+p[-21];

wn80:   return 0;

}



























int zn ()

{       char *q; char *r; char h1; char *p;

        wmat=wmat-hsw[ZN];
        mg=mg-znm[d];
        eg=eg-zne[d];

        q=bord3p8+d;

                q[0]=++q[0] | 16;                   // + 8
        q=q+4;  q[0]=++q[0] | 16;                   // +12
        q=q+7;  q[0]=++q[0] | 16;                   // +19
        q=q+2;  q[0]=++q[0] | 16;                   // +21
        q=q-42; q[0]=++q[0] | 16;                   // -21
        q=q+2;  q[0]=++q[0] | 16;                   // -19
        q=q+7;  q[0]=++q[0] | 16;                   // -12
        q=q+4;  q[0]=++q[0] | 16;                   // - 8

        if (zsom &8) zsom=zsom | 0x7f; else zsom=zsom+4;

        p=pveld+d;                                  // point naar pveld,x
        q=bord+d;                                   // point naar bord,x
        r=bord2+d;

zn00:   h1=zntab[q[8]]; if (h1==0) goto zn10;       // rand/eigen stuk
                        if (h1==1) goto zn02;       // wn..wk = bonus
        if (r[8] & 0x08)  goto zn10;                // wp = geen bonus
zn02:   pmob=pmob-p[8];                             // mob + bonus

zn10:   h1=zntab[q[12]]; if (h1==0) goto zn20;
                         if (h1==1) goto zn12;
        if (r[12] & 0x08)  goto zn20;
zn12:   pmob=pmob-p[12];

zn20:   h1=zntab[q[19]]; if (h1==0) goto zn30;
                         if (h1==1) goto zn22;
        if (r[19] & 0x08)  goto zn30;
zn22:   pmob=pmob-p[19];

zn30:   h1=zntab[q[21]]; if (h1==0) goto zn40;
                         if (h1==1) goto zn32;
        if (r[21] & 0x08)  goto zn40;
zn32:   pmob=pmob-p[21];










zn40:   h1=zntab[q[-8]]; if (h1==0) goto zn50;
                         if (h1==1) goto zn42;
        if (r[-8] & 0x08)  goto zn50;
zn42:   pmob=pmob-p[-8];

zn50:   h1=zntab[q[-12]]; if (h1==0) goto zn60;
                          if (h1==1) goto zn52;
        if (r[-12] & 0x08)  goto zn60;
zn52:   pmob=pmob-p[-12];

zn60:   h1=zntab[q[-19]]; if (h1==0) goto zn70;
                          if (h1==1) goto zn62;
        if (r[-19] & 0x08)  goto zn70;
zn62:   pmob=pmob-p[-19];

zn70:   h1=zntab[q[-21]]; if (h1==0) goto zn80;
                          if (h1==1) goto zn72;
        if (r[-21] & 0x08)  goto zn80;
zn72:   pmob=pmob-p[-21];

zn80:   return 0;

}































int wl ()

{       char *b; char *q; char sc; int mob;

        wmat=wmat+hsw[WL];
        mg=mg+wlm[d];
        eg=eg+wle[d];

        if (wsom & 32) wsom=wsom | 0x7f; else wsom=wsom+16;

        loper=d; mob=lmob;                      // Lmob via register (!!)

        if (wlm[d] != 0) goto wl08;             // <wla7> <wlh7>
        if (bordp9[d] == ZP  ||  bordm11[d] == ZP) wpos=wpos-192;

wl08:   b=bord+d;  copyb=b;
        q=bord2+d; copyq=q;

wl10:   b=b+9; if (b[0] == 0) goto wl90;
        mob=mob+wlmob[b-bord];
        q=q+9; q[0]=++q[0] | 16;
        if (b[0] == LL) goto wl10;

        sc=wl1[b[0]];
        if (sc == 4) goto wl90;
        if (sc <  4) goto wl40;

wl30:   b=b+9; if (b[0] == 0) goto wl90;
        mob=mob+wlmob[b-bord]; q=q+9; q[0]=q[0]+1;
        if (b[0] == LL) goto wl30;
        goto wl90;

wl40:   if (sc < 3) goto wl60;

wl50:   b=b+9; if (b[0] == 0) goto wl90;
        mob=mob+1; if (b[0] == LL) goto wl50;
        wpos=wpos+wl3[b[0]]; goto wl90;

wl60:   b=b+9; if (b[0] == 0) goto wl90;
        mob=mob+wlmob[b-bord]; if (b[0] == LL) goto wl60;
        if (wl3[b[0]] != 0) wpos=wpos+256;
wl90:   b=copyb; q=copyq;












wl100:  b=b-11; if (b[0] == 0) goto wl190;
        mob=mob+wlmob[b-bord];
        q=q-11; q[0]=++q[0] | 16;
        if (b[0] == LL) goto wl100;

        sc=wl1[b[0]];
        if (sc == 4) goto wl190;
        if (sc <  4) goto wl140;

wl130:  b=b-11; if (b[0] == 0) goto wl190;
        mob=mob+wlmob[b-bord]; q=q-11; q[0]=q[0]+1;
        if (b[0] == LL) goto wl130;
        goto wl190;

wl140:  if (sc < 3) goto wl160;

wl150:  b=b-11; if (b[0] == 0) goto wl190;
        mob=mob+1; if (b[0] == LL) goto wl150;
        wpos=wpos+wl3[b[0]]; goto wl190;

wl160:  b=b-11; if (b[0] == 0) goto wl190;
        mob=mob+wlmob[b-bord]; if (b[0] == LL) goto wl160;
        if (wl3[b[0]] != 0) wpos=wpos+256;
wl190:  b=copyb; q=copyq;






























wl200:  b=b-9; if (b[0] == 0) goto wl290;
        mob=mob+wlmob[b-bord];
        q=q-9; q[0]=++q[0] | 16;
        if (b[0] == LL) goto wl200;

        sc=wl2[b[0]];
        if (sc == 4) goto wl290;
        if (sc <  4) goto wl240;

wl230:  b=b-9; if (b[0] == 0) goto wl290;
        mob=mob+wlmob[b-bord]; q=q-9; q[0]=q[0]+1;
        if (b[0] == LL) goto wl230;
        goto wl290;

wl240:  if (sc <  2) goto wl260;
        if (sc == 2) { q[-9]=q[-9]+1; goto wl290; }

wl250:  b=b-9; if (b[0] == 0) goto wl290;
        mob=mob+1; if (b[0] == LL) goto wl250;
        wpos=wpos+wl3[b[0]]; goto wl290;

wl260:  b=b-9; if (b[0] == 0) goto wl290;
        mob=mob+wlmob[b-bord]; if (b[0] == LL) goto wl260;
        if (wl3[b[0]] != 0) wpos=wpos+256;
wl290:  b=copyb; q=copyq;



wl300:  b=b+11; if (b[0] == 0) goto wl390;
        mob=mob+wlmob[b-bord];
        q=q+11; q[0]=++q[0] | 16;
        if (b[0] == LL) goto wl300;

        sc=wl2[b[0]];
        if (sc == 4) goto wl390;
        if (sc <  4) goto wl340;

wl330:  b=b+11; if (b[0] == 0) goto wl390;
        mob=mob+wlmob[b-bord]; q=q+11; q[0]=q[0]+1;
        if (b[0] == LL) goto wl330;
        goto wl390;

wl340:  if (sc <  2) goto wl360;
        if (sc == 2) { q[11]=q[11]+1; goto wl390; }

wl350:  b=b+11; if (b[0] == 0) goto wl390;
        mob=mob+1; if (b[0] == LL) goto wl350;
        wpos=wpos+wl3[b[0]]; goto wl390;

wl360:  b=b+11; if (b[0] == 0) goto wl390;
        mob=mob+wlmob[b-bord]; if (b[0] == LL) goto wl360;
        if (wl3[b[0]] != 0) wpos=wpos+256;
wl390:  b=copyb;

        lmob=mob;

wl400:  b=b-9; if (b[0] == 0) goto wl490;
        if (b[0] != WP) goto wl400;
        sc=wlsc[b[1]];                      // waarde stuk voor de pion
        y=wlbad[b-bord];                    // faktor hoe erg
        sc=sc << y; wpos=wpos-sc;
        goto wl400;

wl490:  b=copyb;

wl500:  b=b+11; if (b[0] == 0) goto wl590;
        if (b[0] != WP) goto wl500;
        sc=wlsc[b[1]];                      // waarde stuk voor de pion
        y=wlbad[b-bord];                    // faktor hoe erg
        sc=sc << y; wpos=wpos-sc;
        goto wl500;

wl590:  return 0;

}

































int zl ()

{       char *b; char *q; char sc; int mob;

        wmat=wmat-hsw[ZL];
        mg=mg-zlm[d];
        eg=eg-zle[d];

        if (zsom & 32) zsom=zsom | 0x7f; else zsom=zsom+16;

        loper=d; mob=lmob;

        if (wlm[d] != 0) goto zl08;             // <zla2> <zlh2>
        if (bordm9[d] == WP  ||  bordp11[d] == WP) wpos=wpos+192;

zl08:   b=bord+d;  copyb=b;
        q=bord3+d; copyq=q;

zl10:   b=b-9; if (b[0] == 0) goto zl90;
        mob=mob-zlmob[b-bord];
        q=q-9; q[0]=++q[0] | 16;
        if (b[0] == LL) goto zl10;

        sc=zl1[b[0]];
        if (sc == 4) goto zl90;
        if (sc <  4) goto zl40;

zl30:   b=b-9; if (b[0] == 0) goto zl90;
        mob=mob-zlmob[b-bord]; q=q-9; q[0]=q[0]+1;
        if (b[0] == LL) goto zl30;
        goto zl90;

zl40:   if (sc < 3) goto zl60;

zl50:   b=b-9; if (b[0] == 0) goto zl90;
        mob=mob-1; if (b[0] == LL) goto zl50;
        wpos=wpos-zl3[b[0]]; goto zl90;

zl60:   b=b-9; if (b[0] == 0) goto zl90;
        mob=mob-zlmob[b-bord]; if (b[0] == LL) goto zl60;
        if (zl3[b[0]] != 0) wpos=wpos-256;
zl90:   b=copyb; q=copyq;












zl100:  b=b+11; if (b[0] == 0) goto zl190;
        mob=mob-zlmob[b-bord];
        q=q+11; q[0]=++q[0] | 16;
        if (b[0] == LL) goto zl100;

        sc=zl1[b[0]];
        if (sc == 4) goto zl190;
        if (sc <  4) goto zl140;

zl130:  b=b+11; if (b[0] == 0) goto zl190;
        mob=mob-zlmob[b-bord]; q=q+11; q[0]=q[0]+1;
        if (b[0] == LL) goto zl130;
        goto zl190;

zl140:  if (sc < 3) goto zl160;

zl150:  b=b+11; if (b[0] == 0) goto zl190;
        mob=mob-1; if (b[0] == LL) goto zl150;
        wpos=wpos-zl3[b[0]]; goto zl190;

zl160:  b=b+11; if (b[0] == 0) goto zl190;
        mob=mob-zlmob[b-bord]; if (b[0] == LL) goto zl160;
        if (zl3[b[0]] != 0) wpos=wpos-256;
zl190:  b=copyb; q=copyq;


zl200:  b=b+9; if (b[0] == 0) goto zl290;
        mob=mob-zlmob[b-bord];
        q=q+9; q[0]=++q[0] | 16;
        if (b[0] == LL) goto zl200;

        sc=zl2[b[0]];
        if (sc == 4) goto zl290;
        if (sc <  4) goto zl240;

zl230:  b=b+9; if (b[0] == 0) goto zl290;
        mob=mob-zlmob[b-bord]; q=q+9; q[0]=q[0]+1;
        if (b[0] == LL) goto zl230;
        goto zl290;

zl240:  if (sc < 2 ) goto zl260;
        if (sc == 2) { q[9]=q[9]+1; goto zl290; }

zl250:  b=b+9; if (b[0] == 0) goto zl290;
        mob=mob-1; if (b[0] == LL) goto zl250;
        wpos=wpos-zl3[b[0]]; goto zl290;

zl260:  b=b+9; if (b[0] == 0) goto zl290;
        mob=mob-zlmob[b-bord]; if (b[0] == LL) goto zl260;
        if (zl3[b[0]] != 0) wpos=wpos-256;
zl290:  b=copyb; q=copyq;



zl300:  b=b-11; if (b[0] == 0) goto zl390;
        mob=mob-zlmob[b-bord];
        q=q-11; q[0]=++q[0] | 16;
        if (b[0] == LL) goto zl300;

        sc=zl2[b[0]];
        if (sc == 4) goto zl390;
        if (sc <  4) goto zl340;

zl330:  b=b-11; if (b[0] == 0) goto zl390;
        mob=mob-zlmob[b-bord]; q=q-11; q[0]=q[0]+1;
        if (b[0] == LL) goto zl330;
        goto zl390;

zl340:  if (sc < 2 ) goto zl360;
        if (sc == 2) { q[-11]=q[-11]+1; goto zl390; }

zl350:  b=b-11; if (b[0] == 0) goto zl390;
        mob=mob-1; if (b[0] == LL) goto zl350;
        wpos=wpos-zl3[b[0]]; goto zl390;

zl360:  b=b-11; if (b[0] == 0) goto zl390;
        mob=mob-zlmob[b-bord]; if (b[0] == LL) goto zl360;
        if (zl3[b[0]] != 0) wpos=wpos-256;
zl390:  b=copyb;

        lmob=mob;



























zl400:  b=b+9; if (b[0] == 0) goto zl490;
        if (b[0] != ZP) goto zl400;
        sc=zlsc[b[-1]];                     // waarde stuk voor de pion
        y=zlbad[b-bord];                    // faktor hoe erg
        sc=sc << y; wpos=wpos+sc;
        goto zl400;

zl490:  b=copyb;

zl500:  b=b-11; if (b[0] == 0) goto zl590;
        if (b[0] != ZP) goto zl500;
        sc=zlsc[b[-1]];                     // waarde stuk voor de pion
        y=zlbad[b-bord];                    // faktor hoe erg
        sc=sc << y; wpos=wpos+sc;
        goto zl500;

zl590:  return 0;

}



































int wt ()

{       char *b; char *q; char sc; int mob;

        wmat=wmat+hsw[WT];
        mg=mg+wtm[d];
        eg=eg+wte[d];

        if (wsom & 64) wsom=wsom | 0x7f;
        wsom=wsom | 64;

        mob=tmob;

        if (num[d] != 6) goto wt02;             // toren op 7e ry
        if (xwz == 4)    goto wt02;             // middenspel
        if (num[zkveld] == 7) wpos=wpos+50;     // zk op 8e ry

wt02:   if (wpnum[alf[d]] != 9) goto wt08;      // wp op toren lyn
        y=xwp >> 1; y=y+1; mob=mob+y;
        if (zpnum[alf[d]] == 0) mob=mob+y;      // indien lyn geheel open

wt08:   b=bord+d;  copyb=b;
        q=bord2+d; copyq=q;































wt10:   b=b+1; if (b[0] == 0) goto wt90;
        mob=mob+wtmob[b-bord];
        q=q+1; q[0]=++q[0] | 32;
        if (b[0] == LL) goto wt10;

        sc=wt1[b[0]];
        if (sc == 4) goto wt90;                 // zp/wk/zk
        if (sc <  4) goto wt40;                 // wn/wl/wp/zn/zl/zd/zk

        if (sc == 5) wpos=wpos+wtm[b-bord];     // toren aansluiting

wt30:   b=b+1; if (b[0] == 0) goto wt90;        // wt/wd
        mob=mob+wtmob[b-bord]; q=q+1; q[0]=q[0]+1;
        if (b[0] == LL) goto wt30;
        if (wt1[b[0]] < 5) goto wt90;
        goto wt30;

wt40:   if (sc == 2) goto wt70;                 // wp
        if (sc  > 2) goto wt60;                 // zd/zk

wt50:   b=b+1; if (b[0] == 0) goto wt90;        // wn/wl/zn/zl
        mob=mob+1; if (b[0] == LL) goto wt50;
        wpos=wpos+wt3[b[0]]; goto wt90;

wt60:   b=b+1; if (b[0] == 0) goto wt90;        // zd/zk
        mob=mob+wtmob[b-bord]; if (b[0] == LL) goto wt60;
        if (b[0] >= ZD) wpos=wpos+256;
        goto wt90;

wt70:   if (b[1] != LL) goto wt90;
        q[1]=q[1]+1;
        if (b[2] != LL) goto wt90;
        q[2]=q[2]+1;
        if (b[3] != LL) goto wt90;
        q[3]=q[3]+1;
wt90:   b=copyb; q=copyq;


















wt100:  b=b-1; if (b[0] == 0) goto wt190;
        mob=mob+wtmob[b-bord];
        q=q-1; q[0]=++q[0] | 32;
        if (b[0] == LL) goto wt100;

        sc=wt4[b[0]];
        if (sc == 4) goto wt190;                // wp/wn/wl/wk/zt
        if (sc <  4) goto wt140;                // zn/zl/zd/zk/zp

        if (sc == 6) goto wt130;                // toren verdubbeling
        sc=b-bord; wpos=wpos+wtm[sc]+11;        // <sc> is oude <e>
        if (wpnum[alf[sc]] != 9) goto wt130;
        if (zpnum[alf[sc]] != 0) goto wt130;
        y=lyncor[alf[sc]]; y=y+aptab[xzp]; if (y>0) wpos=wpos+y;

wt130:  b=b-1; if (b[0] == 0) goto wt190;       // wt/wd
        mob=mob+wtmob[b-bord]; q=q-1; q[0]=q[0]+1;
        if (b[0] == LL) goto wt130;
        if (wt2[b[0]] < 5) goto wt190;
        goto wt130;

wt140:  if (sc == 2) goto wt170;                // zp
        if (sc  > 2) goto wt160;                // zd/zk

wt150:  b=b-1; if (b[0] == 0) goto wt190;       // zn/zl
        wpos=wpos+1; if (b[0] == LL) goto wt150;
        wpos=wpos+wt3[b[0]]; goto wt190;

wt160:  b=b-1; if (b[0] == 0) goto wt190;       // zd/zk
        mob=mob+wtmob[b-bord]; if (b[0] == LL) goto wt160;
        if (b[0] >= ZD) wpos=wpos+256;
        goto wt190;

wt170:  if (b[-1] != LL) goto wt190;
        q[-1]=q[-1]+1;
        if (b[-2] != LL) goto wt190;
        q[-2]=q[-2]+1;
        if (b[-3] != LL) goto wt190;
        q[-3]=q[-3]+1;
wt190:  b=copyb; q=copyq;














wt200:  b=b+10; if (b[0] == 0) goto wt290;
        mob=mob+1;
        q=q+10; q[0]=++q[0] | 32;
        if (b[0] == LL) goto wt200;

        sc=wt2[b[0]];
        if (sc == 4) goto wt290;                // wp/wn/wl/wk/zp/zt
        if (sc <  4) goto wt240;                // zn/zl/zd/zk
        if (sc == 5) wpos=wpos+wtm[b-bord];     // toren aansluiting

wt230:  b=b+10; if (b[0] == 0) goto wt290;      // wt/wd
        mob=mob+1; q=q+10; q[0]=q[0]+1;
        if (b[0] == LL) goto wt230;
        if (wt2[b[0]] < 5) goto wt290;
        goto wt230;

wt240:  if (sc  >= 2) goto wt260;               // zd/zk

wt250:  b=b+10; if (b[0] == 0) goto wt290;      // zn/zl
        wpos=wpos+1; if (b[0] == LL) goto wt250;
        wpos=wpos+wt3[b[0]]; goto wt290;

wt260:  b=b+10; if (b[0] == 0) goto wt290;      // zd/zk
        mob=mob+1; if (b[0] == LL) goto wt260;
        if (b[0] >= ZD) wpos=wpos+256;
        goto wt290;

wt290:  b=copyb; q=copyq;


























wt300:  b=b-10; if (b[0] == 0) goto wt390;
        mob=mob+1;
        q=q-10; q[0]=++q[0] | 32;
        if (b[0] == LL) goto wt300;

        sc=wt2[b[0]];
        if (sc == 4) goto wt390;                // wp/wn/wl/wk/zp/zt
        if (sc <  4) goto wt340;                // zn/zl/zd/zk
        if (sc == 5) wpos=wpos+wtm[b-bord];     // toren aansluiting

wt330:  b=b-10; if (b[0] == 0) goto wt390;       // wt/wd
        mob=mob+1; q=q-10; q[0]=q[0]+1;
        if (b[0] == LL) goto wt330;
        if (wt2[b[0]] < 5) goto wt390;
        goto wt330;

wt340:  if (sc  >= 2) goto wt360;               // zd/zk

wt350:  b=b-10; if (b[0] == 0) goto wt390;      // zn/zl
        wpos=wpos+1; if (b[0] == LL) goto wt350;
        wpos=wpos+wt3[b[0]]; goto wt390;

wt360:  b=b-10; if (b[0] == 0) goto wt390;      // zd/zk
        mob=mob+1; if (b[0] == LL) goto wt360;
        if (b[0] >= ZD) wpos=wpos+256;

wt390:  tmob=mob;

        return 0;

}























int zt ()

{       int e,mob; char *q; int sc;

        wmat=wmat-hsw[ZT];
        mg=mg-ztm[d];
        eg=eg-zte[d];

        if (zsom & 64) zsom=zsom | 0x7f;
        zsom=zsom | 64;

        e=d; mob=tmob;

        if (num[d] != 1) goto zt02;             // toren op 2e ry
        if (xwz == 4)    goto zt02;             // middenspel
        if (num[wkveld] == 0) wpos=wpos-50;     // wk op 1e ry

zt02:   if (zpnum[alf[d]] != 0) goto zt10;      // zp op toren lyn
        y=xzp >> 1; y=y+1; mob=mob-y;
        if (wpnum[alf[d]] == 9) mob=mob-y;      // indien lyn geheel open


































zt10:   e=e-1; if (bord[e] == 0) goto zt90;
        mob=mob-ztmob[e];
        q=bord3+e; q[0]=++q[0] | 32;
        if (bord[e] == LL) goto zt10;

        sc=zt1[bord[e]];
        if (sc == 4) goto zt90;
        if (sc <  4) goto zt40;

        if (sc == 5) wpos=wpos-ztm[e];

zt30:   e=e-1; if (bord[e] == 0) goto zt90;
        mob=mob-ztmob[e]; bord3[e]=bord3[e]+1;
        if (bord[e] == LL) goto zt30;
        if (zt1[bord[e]] < 5) goto zt90;
        goto zt30;

zt40:   if (sc == 2) goto zt70;
        if (sc  > 2) goto zt60;

zt50:   e=e-1; if (bord[e] == 0) goto zt90;
        mob=mob-1; if (bord[e] == LL) goto zt50;
        wpos=wpos-zt3[bord[e]]; goto zt90;

zt60:   e=e-1; if (bord[e] == 0) goto zt90;
        mob=mob-ztmob[e]; if (bord[e] == LL) goto zt60;
        if (zt3[bord[e]] != 0) wpos=wpos-256;
        goto zt90;

zt70:   if (bordm1[e] != LL) goto zt90; else bord3m1[e]=bord3m1[e]+1;
        if (bordm2[e] != LL) goto zt90; else bord3m2[e]=bord3m2[e]+1;
        if (bordm3[e] == LL) bord3m3[e]=bord3m3[e]+1;
zt90:   e=d;





















zt100:  e=e+1; if (bord[e] == 0) goto zt190;
        mob=mob-ztmob[e];
        q=bord3+e; q[0]=++q[0] | 32;
        if (bord[e] == LL) goto zt100;

        sc=zt4[bord[e]];
        if (sc == 4) goto zt190;
        if (sc <  4) goto zt140;
        if (sc == 6) goto zt130;

        wpos=wpos-(ztm[e]+11);
        if (wpnum[alf[e]] != 9) goto zt130;
        if (zpnum[alf[e]] != 0) goto zt130;
        y=lyncor[alf[e]]; y=y+aptab[xwp]; if (y>0) wpos=wpos-y;

zt130:  e=e+1; if (bord[e] == 0) goto zt190;
        mob=mob-ztmob[e]; bord3[e]=bord3[e]+1;
        if (bord[e] == LL) goto zt130;
        if (zt2[bord[e]] < 5) goto zt190;
        goto zt130;

zt140:  if (sc == 2) goto zt170;
        if (sc  > 2) goto zt160;

zt150:  e=e+1; if (bord[e] == 0) goto zt190;
        wpos=wpos-1; if (bord[e] == LL) goto zt150;
        wpos=wpos-zt3[bord[e]]; goto zt190;

zt160:  e=e+1; if (bord[e] == 0) goto zt190;
        mob=mob-ztmob[e]; if (bord[e] == LL) goto zt160;
        if (zt3[bord[e]] != 0) wpos=wpos-256;
        goto zt190;

zt170:  if (bordp1[e] != LL) goto zt190; else bord3p1[e]=bord3p1[e]+1;
        if (bordp2[e] != LL) goto zt190; else bord3p2[e]=bord3p2[e]+1;
        if (bordp3[e] == LL) bord3p3[e]=bord3p3[e]+1;
zt190:  e=d;

















zt200:  e=e+10; if (bord[e] == 0) goto zt290;
        mob=mob-1;
        q=bord3+e; q[0]=++q[0] | 32;
        if (bord[e] == LL) goto zt200;

        sc=zt2[bord[e]];
        if (sc == 4) goto zt290;
        if (sc <  4) goto zt240;
        if (sc == 5) wpos=wpos-ztm[e];

zt230:  e=e+10; if (bord[e] == 0) goto zt290;
        mob=mob-1; bord3[e]=bord3[e]+1;
        if (bord[e] == LL) goto zt230;
        if (zt2[bord[e]] < 5) goto zt290;
        goto zt230;

zt240:  if (sc  >= 2) goto zt260;

zt250:  e=e+10; if (bord[e] == 0) goto zt290;
        wpos=wpos-1; if (bord[e] == LL) goto zt250;
        wpos=wpos-zt3[bord[e]]; goto zt290;

zt260:  e=e+10; if (bord[e] == 0) goto zt290;
        mob=mob-1; if (bord[e] == LL) goto zt260;
        if (zt3[bord[e]] != 0) wpos=wpos-256;
        goto zt290;
zt290:  e=d;



























zt300:  e=e-10; if (bord[e] == 0) goto zt390;
        mob=mob-1;
        q=bord3+e; q[0]=++q[0] | 32;
        if (bord[e] == LL) goto zt300;

        sc=zt2[bord[e]];
        if (sc == 4) goto zt390;
        if (sc <  4) goto zt340;
        if (sc == 5) wpos=wpos-ztm[e];

zt330:  e=e-10; if (bord[e] == 0) goto zt390;
        mob=mob-1; bord3[e]=bord3[e]+1;
        if (bord[e] == LL) goto zt330;
        if (zt2[bord[e]] < 5) goto zt390;
        goto zt330;

zt340:  if (sc  >= 2) goto zt360;

zt350:  e=e-10; if (bord[e] == 0) goto zt390;
        wpos=wpos-1; if (bord[e] == LL) goto zt350;
        wpos=wpos-zt3[bord[e]]; goto zt390;

zt360:  e=e-10; if (bord[e] == 0) goto zt390;
        mob=mob-1; if (bord[e] == LL) goto zt360;
        if (zt3[bord[e]] != 0) wpos=wpos-256;
zt390:  tmob=mob;

        return 0;
}

























int wd ()

{       char *b; char *q; int pos; char sc;

        wmat=wmat+hsw[WD];
        mg=mg+wdm[d];
        eg=eg+wde[d];

        if (wsom & 128) wsom=wsom | 0x100;
        wsom=wsom | 128;

        pos=wpos;

        b=bord+d;  copyb=b;
        q=bord2+d; copyq=q;

wd10:   b=b+9; if (b[0] == 0) goto wd90;
        pos=pos+1;
        q=q+9; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd10;

wd20:   sc=wd1[b[0]];
        if (sc == 2) goto wd90;
        if (sc <  2) goto wd50;

wd30:   b=b+9; if (b[0] == 0) goto wd90;
        pos=pos+1; q=q+9; q[0]=q[0]+1;
        if (b[0] == LL) goto wd30;
        goto wd20;

wd50:   b=b+9; if (b[0] == 0) goto wd90;
        pos=pos+1; if (b[0] == LL) goto wd50;
        pos=pos+wd4[b[0]];
wd90:   b=copyb; q=copyq;




















wd100:  b=b-11; if (b[0] == 0) goto wd190;
        pos=pos+1;
        q=q-11; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd100;

wd120:  sc=wd1[b[0]];
        if (sc == 2) goto wd190;
        if (sc <  2) goto wd150;

wd130:  b=b-11; if (b[0] == 0) goto wd190;
        pos=pos+1; q=q-11; q[0]=q[0]+1;
        if (b[0] == LL) goto wd130;
        goto wd120;

wd150:  b=b-11; if (b[0] == 0) goto wd190;
        pos=pos+1; if (b[0] == LL) goto wd150;
        pos=pos+wd4[b[0]];
wd190:  b=copyb; q=copyq;



wd200:  b=b-9; if (b[0] == 0) goto wd290;
        pos=pos+1;
        q=q-9; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd200;

wd220:  sc=wd2[b[0]];
        if (sc == 2) goto wd290;
        if (sc <  2) goto wd240;

wd230:  b=b-9; if (b[0] == 0) goto wd290;
        pos=pos+1; q=q-9; q[0]=q[0]+1;
        if (b[0] == LL) goto wd230;
        goto wd220;

wd240:  if (sc == 0) { q[-9]=q[-9]+1; goto wd290; }

wd250:  b=b-9; if (b[0] == 0) goto wd290;
        pos=pos+1; if (b[0] == LL) goto wd250;
        pos=pos+wd4[b[0]];
wd290:  b=copyb; q=copyq;













wd300:  b=b+11; if (b[0] == 0) goto wd390;
        pos=pos+1;
        q=q+11; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd300;

wd320:  sc=wd2[b[0]];
        if (sc == 2) goto wd390;
        if (sc <  2) goto wd340;

wd330:  b=b+11; if (b[0] == 0) goto wd390;
        pos=pos+1; q=q+11; q[0]=q[0]+1;
        if (b[0] == LL) goto wd330;
        goto wd320;

wd340:  if (sc == 0) { q[11]=q[11]+1; goto wd390; }

wd350:  b=b+11; if (b[0] == 0) goto wd390;
        pos=pos+1; if (b[0] == LL) goto wd350;
        pos=pos+wd4[b[0]];
wd390:  b=copyb; q=copyq;



wd400:  b=b+1; if (b[0] == 0) goto wd490;
        pos=pos+1;
        q=q+1; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd400;

wd420:  sc=wd3[b[0]];
        if (sc == 2) goto wd490;
        if (sc <  2) goto wd450;

wd430:  b=b+1; if (b[0] == 0) goto wd490;
        pos=pos+1; q=q+1; q[0]=q[0]+1;
        if (b[0] == LL) goto wd430;
        goto wd420;

wd450:  b=b+1; if (b[0] == 0) goto wd490;
        pos=pos+1; if (b[0] == LL) goto wd450;
        pos=pos+wd4[b[0]];
wd490:  b=copyb; q=copyq;













wd500:  b=b-1; if (b[0] == 0) goto wd590;
        pos=pos+1;
        q=q-1; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd500;

wd520:  sc=wd3[b[0]];
        if (sc == 2) goto wd590;
        if (sc <  2) goto wd550;

wd530:  b=b-1; if (b[0] == 0) goto wd590;
        pos=pos+1; q=q-1; q[0]=q[0]+1;
        if (b[0] == LL) goto wd530;
        goto wd520;

wd550:  b=b-1; if (b[0] == 0) goto wd590;
        pos=pos+1; if (b[0] == LL) goto wd550;
        pos=pos+wd4[b[0]];
wd590:  b=copyb; q=copyq;



wd600:  b=b+10; if (b[0] == 0) goto wd690;
        pos=pos+1;
        q=q+10; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd600;

wd620:  sc=wd3[b[0]];
        if (sc == 2) goto wd690;
        if (sc <  2) goto wd650;

wd630:  b=b+10; if (b[0] == 0) goto wd690;
        pos=pos+1; q=q+10; q[0]=q[0]+1;
        if (b[0] == LL) goto wd630;
        goto wd620;

wd650:  b=b+10; if (b[0] == 0) goto wd690;
        pos=pos+1; if (b[0] == LL) goto wd650;
        pos=pos+wd4[b[0]];
wd690:  b=copyb; q=copyq;















wd700:  b=b-10; if (b[0] == 0) goto wd790;
        pos=pos+1;
        q=q-10; q[0]=++q[0] | 64;
        if (b[0] == LL) goto wd700;

wd720:  sc=wd3[b[0]];
        if (sc == 2) goto wd790;
        if (sc <  2) goto wd750;

wd730:  b=b-10; if (b[0] == 0) goto wd790;
        pos=pos+1; q=q-10; q[0]=q[0]+1;
        if (b[0] == LL) goto wd730;
        goto wd720;

wd750:  b=b-10; if (b[0] == 0) goto wd790;
        pos=pos+1; if (b[0] == LL) goto wd750;
        pos=pos+wd4[b[0]];
wd790:  wpos=pos;

        return 0;
}

































int zd ()

{       char *b; char *q; int pos; char sc;

        wmat=wmat-hsw[ZD];
        mg=mg-zdm[d];
        eg=eg-zde[d];

        if (zsom & 128) zsom=zsom | 0x100;
        zsom=zsom | 128;

        pos=wpos;

        b=bord+d;  copyb=b;
        q=bord3+d; copyq=q;

zd10:   b=b-9; if (b[0] == 0) goto zd90;
        pos=pos-1;
        q=q-9; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd10;

zd20:   sc=zd1[b[0]];
        if (sc == 2) goto zd90;
        if (sc <  2) goto zd50;

zd30:   b=b-9; if (b[0] == 0) goto zd90;
        pos=pos-1; q=q-9; q[0]=q[0]+1;
        if (b[0] == LL) goto zd30;
        goto zd20;

zd50:   b=b-9; if (b[0] == 0) goto zd90;
        pos=pos-1; if (b[0] == LL) goto zd50;
        pos=pos-zd4[b[0]];
zd90:   b=copyb; q=copyq;




















zd100:  b=b+11; if (b[0] == 0) goto zd190;
        pos=pos-1;
        q=q+11; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd100;

zd120:  sc=zd1[b[0]];
        if (sc == 2) goto zd190;
        if (sc <  2) goto zd150;

zd130:  b=b+11; if (b[0] == 0) goto zd190;
        pos=pos-1; q=q+11; q[0]=q[0]+1;
        if (b[0] == LL) goto zd130;
        goto zd120;

zd150:  b=b+11; if (b[0] == 0) goto zd190;
        pos=pos-1; if (b[0] == LL) goto zd150;
        pos=pos-zd4[b[0]];
zd190:  b=copyb; q=copyq;



zd200:  b=b+9; if (b[0] == 0) goto zd290;
        pos=pos-1;
        q=q+9; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd200;

zd220:  sc=zd2[b[0]];
        if (sc == 2) goto zd290;
        if (sc <  2) goto zd240;

zd230:  b=b+9; if (b[0] == 0) goto zd290;
        pos=pos-1; q=q+9; q[0]=q[0]+1;
        if (b[0] == LL) goto zd230;
        goto zd220;

zd240:  if (sc == 0) { q[9]=q[9]+1; goto zd290; }

zd250:  b=b+9; if (b[0] == 0) goto zd290;
        pos=pos-1; if (b[0] == LL) goto zd250;
        pos=pos-zd4[b[0]];
zd290:  b=copyb; q=copyq;













zd300:  b=b-11; if (b[0] == 0) goto zd390;
        pos=pos-1;
        q=q-11; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd300;

zd320:  sc=zd2[b[0]];
        if (sc == 2) goto zd390;
        if (sc <  2) goto zd340;

zd330:  b=b-11; if (b[0] == 0) goto zd390;
        pos=pos-1; q=q-11; q[0]=q[0]+1;
        if (b[0] == LL) goto zd330;
        goto zd320;

zd340:  if (sc == 0) { q[-11]=q[-11]+1; goto zd390; }

zd350:  b=b-11; if (b[0] == 0) goto zd390;
        pos=pos-1; if (b[0] == LL) goto zd350;
        pos=pos-zd4[b[0]];
zd390:  b=copyb; q=copyq;



zd400:  b=b+1; if (b[0] == 0) goto zd490;
        pos=pos-1;
        q=q+1; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd400;

zd420:  sc=zd3[b[0]];
        if (sc == 2) goto zd490;
        if (sc <  2) goto zd450;

zd430:  b=b+1; if (b[0] == 0) goto zd490;
        pos=pos-1; q=q+1; q[0]=q[0]+1;
        if (b[0] == LL) goto zd430;
        goto zd420;

zd450:  b=b+1; if (b[0] == 0) goto zd490;
        pos=pos-1; if (b[0] == LL) goto zd450;
        pos=pos-zd4[b[0]];
zd490:  b=copyb; q=copyq;













zd500:  b=b-1; if (b[0] == 0) goto zd590;
        pos=pos-1;
        q=q-1; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd500;

zd520:  sc=zd3[b[0]];
        if (sc == 2) goto zd590;
        if (sc <  2) goto zd550;

zd530:  b=b-1; if (b[0] == 0) goto zd590;
        pos=pos-1; q=q-1; q[0]=q[0]+1;
        if (b[0] == LL) goto zd530;
        goto zd520;

zd550:  b=b-1; if (b[0] == 0) goto zd590;
        pos=pos-1; if (b[0] == LL) goto zd550;
        pos=pos-zd4[b[0]];
zd590:  b=copyb; q=copyq;



zd600:  b=b+10; if (b[0] == 0) goto zd690;
        pos=pos-1;
        q=q+10; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd600;

zd620:  sc=zd3[b[0]];
        if (sc == 2) goto zd690;
        if (sc <  2) goto zd650;

zd630:  b=b+10; if (b[0] == 0) goto zd690;
        pos=pos-1; q=q+10; q[0]=q[0]+1;
        if (b[0] == LL) goto zd630;
        goto zd620;

zd650:  b=b+10; if (b[0] == 0) goto zd690;
        pos=pos-1; if (b[0] == LL) goto zd650;
        pos=pos-zd4[b[0]];
zd690:  b=copyb; q=copyq;















zd700:  b=b-10; if (b[0] == 0) goto zd790;
        pos=pos-1;
        q=q-10; q[0]=++q[0] | 64;
        if (b[0] == LL) goto zd700;

zd720:  sc=zd3[b[0]];
        if (sc == 2) goto zd790;
        if (sc <  2) goto zd750;

zd730:  b=b-10; if (b[0] == 0) goto zd790;
        pos=pos-1; q=q-10; q[0]=q[0]+1;
        if (b[0] == LL) goto zd730;
        goto zd720;

zd750:  b=b-10; if (b[0] == 0) goto zd790;
        pos=pos-1; if (b[0] == LL) goto zd750;
        pos=pos-zd4[b[0]];
zd790:  wpos=pos;

        return 0;
}
































//  Einde verhaal