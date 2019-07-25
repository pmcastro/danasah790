int evaluation ()

{
        mg=0; eg=0; wmat=0;
        pmob=0; lmob=0; tmob=0; formpawn=0; h3=0;
        paspawn=0; kingsave=0; wpos=0; wsom=0; zsom=0;
        w2=w3=w6=z2=z3=z5=z6=0;
        wpval=zpval=wpveld=zpveld=pawn=loper,quadrant=0;

        p_bord2[5]=  p_bord2[6]=  p_bord2[7]=  p_bord2[8]=
        p_bord2[9]=  p_bord2[10]= p_bord2[11]= p_bord2[12]=
        p_bord2[13]= p_bord2[14]= p_bord2[15]= p_bord2[16]=
        p_bord2[17]= p_bord2[18]= p_bord2[19]= p_bord2[20]=
        p_bord2[21]= p_bord2[22]= p_bord2[23]= p_bord2[24]=0;

        p_bord3[5]=  p_bord3[6]=  p_bord3[7]=  p_bord3[8]=
        p_bord3[9]=  p_bord3[10]= p_bord3[11]= p_bord3[12]=
        p_bord3[13]= p_bord3[14]= p_bord3[15]= p_bord3[16]=
        p_bord3[17]= p_bord3[18]= p_bord3[19]= p_bord3[20]=
        p_bord3[21]= p_bord3[22]= p_bord3[23]= p_bord3[24]=0;

        if (xwp >= 4) wsom=0x7f; else wsom=xwp;
        if (xzp >= 4) zsom=0x7f; else zsom=xzp;

        vwz=veldwz_m1;

wit:    vwz=vwz+1; d=vwz[0];
        if (d ==  -1)  goto zwart;
        switch (bord[d]) {
        case WN : { wn(); goto wit; }
        case WL : { wl(); goto wit; }
        case WT : { wt(); goto wit; }
        case WD : { wd(); goto wit; }
        case WK : { wk(); goto wit; } }
        wp();

zwart:  vwz=vwz+1; d=vwz[0];
        if (d ==  -1)  goto mobiel;
        switch (bord[d]) {
        case ZN :  { zn(); goto zwart; }
        case ZL :  { zl(); goto zwart; }
        case ZT :  { zt(); goto zwart; }
        case ZD :  { zd(); goto zwart; }
        case ZK :  { zk(); goto zwart; } }
        zp();









mobiel: mob=pmob+lmob+tmob;

        if (mob >=0 ) wpos=wpos+mobtab[mob];
              else { y=mob ^ -1;
                     y=y+1;
                     wpos=wpos-mobtab[y]; }

        hang_wit();                 // zie Posgen2
        hang_zwart();               // zie Posgen2

        wpos=wpos+formpawn+paspawn;

        if (quadrant) kwadraat();


//      if (z3==33) mp = -32000;    // position is illegal, own king can be captured






































        x=wsom & 0xff; if (wbit[x]==17) goto ex11;
        y=zsom & 0xff; if (zbit[y]==17) goto ex11;

        z=wbit[x]+zbit[y];                  // bepaal de pointer
        wpos=wpos+remcor[z];                // corrigeer score

        if (remval[z] == 'N') goto ex11;    // geen remise cor.

        v=remval[z];

        if (v == 'R') goto ex10a;           // remise waarde
        if (v != '!') goto ex10b;           // kats remise

ex10a:  mp=0; goto ex70;                    // draw

ex10b:  if (v == 'L') goto ex10c;           // slechte loper wit
        if (v == 'l') goto ex10d;           // slechte loper zwart
        if (v == 'P') goto ex10g;           // wk + wp (rand)
        if (v == 'p') goto ex10h;           // zk + zp (rand)
        if (v == 'K') goto ex10k;           // mat loper/paard (w)
        if (v == 'k') goto ex10l;           // mat loper/paard (z)
        if (v == 'D') goto ex10m;           // wd tegen zp (2)
        if (v == 'd') goto ex10n;           // zd tegen wp (7)
        if (v == 'T') goto ex10p;           // wt+wp <> zt
        if (v == 't') goto ex10q;           // zt+zp <> wt

        if (v == 'X') wpos=64;              // 0.25
        if (v == 'Y') wpos=128;             // 0.50
        if (v == 'x') wpos=-64;             // -0.25
        if (v == 'y') wpos=-128;            // -0.50
























//      Standaard score + posities Wk / Zk
//      ==================================
        wpos=wpos+wkm[wkveld];
        mp=wpos-zkm[zkveld];
        if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;


//      Slechte loper + randpion (wit)
//      ==============================
ex10c:  x=plveld1[wpawn];                               // bepaal promveld
        if (x == 0) goto ex11;                          // geen rand pion
        if (x == plveld2[loper]) goto ex11;             // loper ok wit wint
        if (bord3[x] & 0xffffff80) goto ex10e;          // zk heeft promveld
        if (num[zkveld] <= num[wpawn]) goto ex11;       // zk niet voor wp
        x=alf[zkveld] - alf[wpawn];                     // test afstand zk/wp
        if (x==1 || x==0 || x==-1) goto ex10e;          // zk ok ==> remise
        goto ex11;                                      // wit wint

//      Slechte loper + randpion (zwart)
//      ================================
ex10d:  x=plveld1[zpawn];                               // bepaal promveld
        if (x == 0) goto ex11;                          // geen rand pion
        if (x != plveld2[loper]) goto ex11;             // loper ok zwart wint
        if (bord2_m7[x] & 0xffffff80) goto ex10f;       // wk heeft promveld
        if (num[wkveld] >= num[zpawn]) goto ex11;       // wk niet voor zp
        x=alf[wkveld] - alf[zpawn];                     // test afstand wk/zp
        if (x==1 || x==0 || x==-1) goto ex10f;          // wk ok ==> remise
        goto ex11;                                      // zwart wint

//      Remise gevallen hoger waarderen dan 0.00   (nb. Remscore + 10)
//      ==============================================================
ex10e:  mp=10;
        if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;

ex10f:  mp=-10;
        if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;















//      Wk + witte randpion tegen Zk
//      ============================
ex10g:  if (wpawn < A8)  { x=C8; goto ex10gg; }         // bepaal
        if (wpawn > H1)    x=F8; else goto ex11;        // sleutelveld
ex10gg: if (wpval != 0) goto ex11;                      // zk uit kwadraat
        if (bord[x] == ZK) goto ex10e;                  // <c8> <f8> = remise
        if (bordm1[x] == ZK) goto ex10e;                // <c7> <f7> = remise
        if (num[zkveld] <= num[wpawn]) goto ex11;       // zk niet voor wp
        x=alf[zkveld] - alf[wpawn];                     // test afstand zk/wp
        if (x==1 || x==0 || x==-1) goto ex10e;          // zk ok ==> remise
        goto ex11;                                      // nog geen remise

//      Zk + zwarte randpion tegen Wk
//      =============================
ex10h:  if (zpawn < A8)  { x=C1; goto ex10hh; }         // bepaal
        if (zpawn > H1)    x=F1; else goto ex11;        // sleutelveld
ex10hh: if (zpval != 0) goto ex11;                      // wk uit kwadraat
        if (bord[x] == WK) goto ex10f;                  // <c1> <f1> = remise
        if (bordp1[x] == WK) goto ex10f;                // <c2> <f2> = remise
        if (num[wkveld] >= num[zpawn]) goto ex11;       // wk niet voor zp
        x=alf[wkveld] - alf[zpawn];                     // test afstand wk/zp
        if (x==1 || x==0 || x==-1) goto ex10f;          // zk ok ==> remise
        goto ex11;                                      // nog geen remise

//      Matvoering Loper/Paard (wit)
//      ============================
ex10k:  x=num[wkveld] - num[zkveld]; if (x < 0) { x=x^-1; x=x+1; }
        y=alf[wkveld] - alf[zkveld]; if (y < 0) { y=y^-1; y=y+1; }
        if (x < y) x=y;                                 // neem grootste
        x=x << 3;                                       // afstand * 8
        mp = 0x780 - x;                                 // waardeer wk => zk
        if (plveld2[loper] == 8) mp=mp-lh1[zkveld];     // score -
         else                    mp=mp-la1[zkveld];     // juiste hoektabel
        if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;

//      Matvoering Loper/Paard (zwart)
//      ==============================
ex10l:  x=num[wkveld] - num[zkveld]; if (x < 0) { x=x^-1; x=x+1; }
        y=alf[wkveld] - alf[zkveld]; if (y < 0) { y=y^-1; y=y+1; }
        if (x < y) x=y;                                 // neem grootste
        x=x << 3;                                       // afstand * 8
        mp = x - 0x780;                                 // waardeer wk => zk
        if (plveld2[loper] == 8) mp=mp+lh1[wkveld];     // score -
         else                    mp=mp+la1[wkveld];     // juiste hoektabel
        if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;







//      Witte dame versus zwarte pion 2e ry
//      ===================================
ex10m:  if (dpveld[zpawn] == 0) goto ex11;              // zp niet op 2e ry
        if (dpveld[zpawn] >= 3) goto ex11;              // zp niet op 2e ry
        if (alf[zpawn] < 4) mp=wkc3[wkveld];            // wk naar <c3>
         else               mp=wkf3[wkveld];            // wk naar <f3>
        mp=mp << 2; mp=mp + 0x600;
        if (bordm1[zpawn] == WD)
           { mp=mp+256; goto exm10; }                   // wd op promveld
        if (bord3[zpawn] != 0) mp=mp+32;                // valt <zp> aan
        if (dpveld[zpawn] == 1) goto exm20;             // rand/loper pion
exm10:  if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;

//      <wd> tegen een zwarte Rand/Loper pion  ==>  Remise behalve ...
//      ==============================================================
exm20:  if (wpnt1 != 0) mp=mp ^ -1;         // *** VOORLOPIG ***
        goto ex70;

//      Zwarte dame versus witte pion 7e ry
//      ===================================
ex10n:  if (dpveld[wpawn] < 3) goto ex11;               // wp niet op 7e ry
        if (alf[wpawn] < 4) mp=zkc6[zkveld];            // zk naar <c6>
         else               mp=zkf6[zkveld];            // zk naar <f6>
        mp=mp << 2; mp=mp ^ -1; mp=mp - 0x600;
        if (bordp1[wpawn] == ZD)
           { mp=mp-256; goto exn10; }                   // zd op promveld
        if (bord2[wpawn] != 0) mp=mp-32;                // valt <wp> aan
        if (dpveld[wpawn] == 3) goto exn20;             // rand/loper pion
exn10:  if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;

//      <zd> tegen een witte Rand/Loper pion  ==>  Remise behalve ...
//      =============================================================
exn20:  if (wpnt1 != 0) mp=mp ^ -1;         // *** VOORLOPIG ***
        goto ex70;


















//      <wt>+<wp> tegen <zt> ... Remise wanner <zk> VOOR de witte pion staat
//      ====================================================================
ex10p:  if (num[wpawn] >= num[zkveld]) goto ex11;       // zk niet voor wp
        x=alf[zkveld]-alf[wpawn];                       // test nu de ALF
        if (x >=  2) goto ex11;                         // zk niet voor wp
        if (x <= -2) goto ex11;                         // zk niet voor wp
        if (x==0) mp=(wpos+192) >> 3;                   // zkg7 <> wpg4 .. / 8
          else    mp=(wpos+192) >> 2;                   // zkf7 <> wpg4 .. / 4
        if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;

//      <zt>+<zp> tegen <wt> ... Remise wanner <wk> VOOR de zwarte pion staat
//      =====================================================================
ex10q:  if (num[zpawn] <= num[wkveld]) goto ex11;       // wk niet voor zp
        x=alf[wkveld]-alf[zpawn];                       // test nu de ALF
        if (x >=  2) goto ex11;                         // wk niet voor zp
        if (x <= -2) goto ex11;                         // wk niet voor zp
        if (x==0) mp=(wpos-192) >> 3;                   // wkg2 <> zpg5 .. / 8
          else    mp=(wpos-192) >> 2;                   // wkf2 <> zpg5 .. / 4
        if (wpnt1 != 0) mp=mp ^ -1;
        goto ex70;

ex11:   if (opp == 0) goto ex14;                        // mat zetten
        if (xw==0) { wpos=wpos+wkm[wkveld]+wkm[wkveld]; goto ex20; }
        wpos=wpos - (zkm[zkveld] << 1); goto ex20;

ex14:   if (wsom >= 128) zksave();                      // Koningsveiligheid (zwart)
        if (zsom >= 128) wksave();                      // Koningsveiligheid (wit)

//      Pionnen eindspel
//      ================
ex20:   if (xw != 0) goto ex40;
        if (xz != 0) goto ex40;
        hl=pend[xwp]-pend[xzp];                         // verschil in pionnen
        wpos=wpos+hl;                                   // met pion meer wint



















//      <wk> + <wp> tegen ZWARTE koning eindspel
//      ========================================
ex21:   if (pawn >= H1  ||  pawn <= A8) goto ex40;      // randpion geen bonus
        if (xwp != 1) goto ex30;                        // zwarte gedeelte
        if (xzp != 0) goto ex40;                        // normaal pionending
        if (wpval != 0) goto ex40;                      // al kwadraat bonus
        if (bord2p1[pawn] > 0) goto ex22;               // ipv 0xffffff80 !!
        if (num[pawn] >= num[zkveld]) goto ex29;        // wint altyd
ex22:   if (num[pawn] == 5) goto ex23c;                 // wp (6)
        if (num[pawn] == 6) goto ex23;                  // wp (7)
        if (num[pawn] >= 4) goto ex26;                  // wp (4)
        x=zkveld-wkveld; if (x != 2) goto ex24;         // geen oppositie
        if (wpnt1==0) goto ex26; else goto ex28;

//      Indien wp(7) en <zk> op het promotie veld
//      =========================================
ex23:   if (zkveld-1 != pawn) goto ex23b;               // zk niet op promveld
        x=zkveld-wkveld;
        if (wpnt1 != 0) goto ex23a;
        if (x != 2) goto ex29; else goto ex26;          // geen opp. wit wint
ex23a:  if (x == 2) goto ex29; else goto ex26;          // wel  opp. wit wint

//      Indien wp(7) en <zk> NIET op het promotie veld
//      ==============================================
ex23b:  if (bord2[pawn] > 0) goto ex26;                 // wk heeft wp NIET
        if (wpnt1 != 0) goto ex29;                      // wit wint altyd
        goto ex26;                                      // normale procedure

//      Indien wp(6) gedeelte ... beslis winst of remise
//      ================================================
ex23c:  if (num[wkveld] != 5) goto ex26;                // <wk> niet op 6e ry
        if (bord2p1[pawn] > 0) goto ex26;               // <wk> heeft veld voor <wp> niet
        if (wpnt1 != 0) goto ex23d;
        if (bordp2[pawn] != ZK) goto ex29;              // zk NIET op promveld
        goto ex26;                                      // normale procedure

ex23d:  if (zkveld-wkveld != 2) goto ex29;              // geen opp. wit wint
        goto ex26;                                      // normale procedure

//      Witte pion op 2e / 3e / 4e ry ... beslis winst of remise
//      ========================================================
ex24:   if (zpnt1 == 0) goto ex26;
        if (bordp2[wkveld] == RR) goto ex26;            // wk+2 is randveld
        if (bord3p2[wkveld] < 0) goto ex28;             // zk kan opp. nemen
ex26:   if (bordp11[pawn] == WK) goto ex29;             // wit wint altyd
        if (bordp1[pawn]  == WK) goto ex29;             // wit wint altyd
        if (bordm9[pawn]  == WK) goto ex29;             // wit wint altyd
ex28:   if (bordp12[pawn] == WK) goto ex29;             // wit wint altyd
        if (bordp2[pawn]  == WK) goto ex29;             // wit wint altyd
        if (bordm8[pawn]  != WK) goto ex40;             // wit wint NIET
ex29:   wpos=wpos+0x400;                                // bonus
        goto ex40;


//      <zk> + <zp> tegen WITTE koning eindspel
//      ========================================
ex30:   if (xzp != 1) goto ex40;
        if (xwp != 0) goto ex40;
        if (zpval !=0) goto ex40;
        if (bord3m1[pawn] > 0) goto ex32;               // ipv 0xffffff80 !!
        if (zp1[pawn] >= zp1[wkveld]) goto ex39;        // wint altyd
ex32:   if (zp1[pawn] == 5) goto ex33c;                 // zp (3)
        if (zp1[pawn] == 6) goto ex33;                  // zp (2)
        if (zp1[pawn] >= 4) goto ex36;                  // zp (4)
        x=zkveld-wkveld; if (x != 2) goto ex34;         // geen oppositie
        if (zpnt1==0) goto ex36; else goto ex38;

//      Indien zp(2) en <wk> op het promotie veld
//      =========================================
ex33:   if (wkveld+1 != pawn) goto ex33b;               // wk niet op promveld
        x=zkveld-wkveld;
        if (wpnt1 == 0) goto ex33a;
        if (x != 2) goto ex39; else goto ex36;          // geen opp zwart wint
ex33a:  if (x == 2) goto ex39; else goto ex36;          // wel  opp zwart wint

//      Indien zp(2) en <wk> NIET op het promotie veld
//      ==============================================
ex33b:  if (bord3[pawn] > 0) goto ex36;                 // zk heeft zp NIET
        if (wpnt1 == 0) goto ex39;                      // zwart wint altyd
        goto ex36;                                      // normale procedure

//      Indien zp(3) gedeelte ... beslis winst of remise
//      ================================================
ex33c:  if (zp1[zkveld] != 5) goto ex36;                // <zk> niet op 3e ry
        if (bord3m1[pawn] > 0) goto ex36;               // <zk> heeft veld voor <zp> niet
        if (zpnt1 != 0) goto ex33d;
        if (bordm2[pawn] != WK) goto ex39;              // wk NIET op promveld
        goto ex36;                                      // normale procedure

ex33d:  if (zkveld-wkveld != 2) goto ex39;              // geen opp zwart wint
        goto ex36;                                      // normale procedure

//      Zwarte pion op 7e / 6e / 5e ry ... beslis winst of remise
//      =========================================================
ex34:   if (wpnt1 == 0) goto ex36;
        if (bordm2[zkveld] == RR) goto ex36;            // zk-2 is randveld
        if (bord2m2[zkveld] < 0) goto ex38;             // wk kan opp. nemen
ex36:   if (bordp9[pawn]  == ZK) goto ex39;             // zwart wint altyd
        if (bordm1[pawn]  == ZK) goto ex39;             // zwart wint altyd
        if (bordm11[pawn] == ZK) goto ex39;             // zwart wint altyd
ex38:   if (bordp8[pawn]  == ZK) goto ex39;             // zwart wint altyd
        if (bordm2[pawn]  == ZK) goto ex39;             // zwart wint altyd
        if (bordm12[pawn] != ZK) goto ex40;             // zwart wint NIET
ex39:   wpos=wpos-0x400;                                // bonus




ex40:   wpos=wpos+kingsave;
        if (xwz>=3) wpos+=mg; else wpos+=eg;
        if (wpnt1==0) mp=mpi+wpos+wmat; else { mp=mpi-wpos; mp=mp-wmat; }

        if (w6) mp=mp+60;

ex70:   return (mp);
}

//  Einde verhaal
