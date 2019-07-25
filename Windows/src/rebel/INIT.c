int init ()

{       int x;

        if (first_time==0)
         { for (x=0; x<276; x++) bgboard[x]=bgstel[x];      // initialize board
           fp = fopen("attack.bin","rb");                   // initialize attack table
           if (fp==NULL) { printf("File attack.bin not found"); exit(1); }
           x=0; while ((hang_tab[x]=getc(fp))!=EOF) x++;
           fclose(fp); }

        first_time++;

        opp=0;

        zoekveld();             // aanmaken veldwz
        vrypion();              // aanmaken wpnum/zpnum
        setpnt();               // aanmaken wpm ... zlm
        restant();              // diversen

        return 0;
}
































int zoekveld ()

{       int x,d,e;

        xw=0; xz=0; xwp=0; xzp=0; wpnt1=0; mpi=0;
        for (x=0; x<8; x++)  { wpdup[x]=0; zpdup[x]=0; }

        x=0;
        for (d=7; d>1; d--)
         { if (d==WP) wpnt2=x;
           for (e=H8; e>=A1; e--)
             if (bord[e]==d) { veldwz[x]=e; x++; } }
        veldwz[x]=255; x++; zpnt1=x; zpnt3=x;

        for (d=13; d>7; d--)
         { if (d==ZP) zpnt2=x;
           for (e=H8; e>=A1; e--)
             if (bord[e]==d) { veldwz[x]=e; x++; } }

        veldwz[x]=255;

        wkveld=veldwz[0];
        zkveld=veldwz[zpnt3];

//      Aanmaken xw/xz/xwp/xzp ... wpdup/zpdup
//      ======================================

        for (x=1; x!=wpnt2; x++)         { xw=xw+sw[bord[veldwz[x]]]; }
        x=wpnt2; while (veldwz[x] != -1) { xwp=xwp+1; x++; }
        for (x=zpnt1+1; x!=zpnt2; x++)   { xz=xz+sw[bord[veldwz[x]]]; }
        x=zpnt2; while (veldwz[x] != -1) { xzp=xzp+1; x++; }

        for (x=A1; x<=H8; x++)
         { if (bord[x]==WP)   wpdup[alf[x]]=wpdup[alf[x]]+1;
           if (bord[x]==WP && wpdup[alf[x]] > 1) mpi=mpi-DP;
           if (bord[x]==ZP)   zpdup[alf[x]]=zpdup[alf[x]]+1;
           if (bord[x]==ZP && zpdup[alf[x]] > 1) mpi=mpi+DP; }

        return 0;
}














int vrypion ()

{       wpnum[-1]=9; wpnum[8]=9;
        zpnum[-1]=0; zpnum[8]=0;

        wpinit();
        zpinit();

        return 0;
}

int setpnt ()

{       int x,y;

        if (xw+xz >= 27) { xwz=4; goto sp00; }      // middenspel
        if (xw+xz <= 10) { xwz=2; goto sp00; }      // eindspel 2
        xwz=1;                                      // eindspel 1
sp00:   if (xwz==4 && bord[veldwz[1]]!=WD && bord[veldwz[zpnt3+1]]!=ZD) xwz=3;

        if (xw==0 && xwp==0 && xz>=5) { opp=3; }    // activeren opp
        if (xz==0 && xzp==0 && xw>=5) { opp=3; }    // activeren opp

        white_castle=0; black_castle=0;
        if (xwz==4 && alf[wkveld]<4 && bord[veldwz[zpnt3+1]]==ZD) white_castle=1;
        if (xwz==4 && alf[zkveld]<4 && bord[veldwz[1]]==WD) black_castle=1;

        return 0;
}


int restant ()
{       int x;

        if (kleur != 0)
         { mpi=mpi ^ -1; x=wpnt1; wpnt1=zpnt1; zpnt1=x; }

        remscore=0;

        return 0;
}













int wpinit ()
{       char *x;

        p_wpnum[0]=0x09090909; p_wpnum[1]=0x09090909;
        x=veldwz_m1+wpnt2; ;

wpv10:  x=x+1;
        if (x[0] == 100) goto wpv10;
        if (x[0] ==  -1) goto einde;
        if (num[x[0]] <  wpnum[alf[x[0]]]) wpnum[alf[x[0]]] = num[x[0]];
        goto wpv10;

wpv20:  x=x+1;
        if (x[0] == 100) goto wpv20;
        if (x[0] ==  -1) goto einde;
        if (bord[x[0]] != WP) goto wpv20;
        if (num[x[0]] <  wpnum[alf[x[0]]]) wpnum[alf[x[0]]] = num[x[0]];
        goto wpv20;

einde:  return 0;
}

int zpinit ()
{       char *x;
        p_zpnum[0]=0; p_zpnum[1]=0;
        x=veldwz_m1+zpnt2;

zpv10:  x=x+1;
        if (x[0] == 100) goto zpv10;
        if (x[0] ==  -1) goto einde;
        if (num[x[0]] >  zpnum[alf[x[0]]]) zpnum[alf[x[0]]] = num[x[0]];
        goto zpv10;

zpv20:  x=x+1;
        if (x[0] == 100) goto zpv20;
        if (x[0] ==  -1) goto einde;
        if (bord[x[0]] != ZP) goto zpv20;
        if (num[x[0]] >  zpnum[alf[x[0]]]) zpnum[alf[x[0]]] = num[x[0]];
        goto zpv20;

einde:  return 0;
}












void get_fen()

{       int x,y; char ch;

        bugnr=0;

        for (x=A1; x<=H8; x++) if (bord[x] > RR) bord[x]=LL;

        x=-1; y=0;
next:   x++; ch=epd[x];
        if (ch=='/') goto next;
        if (ch >= '1' && ch <= '8') { v=ch-48; y=y+v; goto test; }
        for (v=2; v<=15; v++) if (fentab[v]==ch) { bord[bordpos[y]]=v; y++; break; }
        if (v>13) goto fout;
test:   if (y < 64) goto next;

        x++; if (epd[x] != ' ') goto fout;

        x++; if (epd[x]==fenkleur[0]) { kleur=BLACK; goto ok; }
             if (epd[x]==fenkleur[1]) { kleur=WHITE; goto ok; }

        goto fout;

ok:     return;


fout:   bugnr=1;

}

























void debug()

{       int x,y; FILE *fp; char s[300]; float f;
        char sc1[] = {'X','.','w','W','W','W','W','W','b','B','B','B','B','B'};
        char sc2[] = {'X','.','p','N','B','R','Q','K','p','N','B','R','Q','K'};

        x=0; while (bordpos[x] != 0)
          { y=bordpos[x];
            x++; z=bordpos[x];
            printf ("%c%c ",sc1[bord[y]],sc2[bord[y]]);
            if (z==7 && kleur==1) printf("      From black point of view\n");
            if (z==7 && kleur==0) printf("      From white point of view\n");
            if (z==6)             printf("\n");
            if (z==5)             printf("\n");
            if (z==4)             printf("\n");
            if (z==3)             printf("\n");
            if (z==2)             printf("\n");
            if (z==1)             printf("\n");
        }

        f=mp;   f=f/256;      printf("\nScore\t\t %.2f\n",f);
        f=wmat; f=f/256;      printf("Material\t %.2f\n",f);
        f=mg;   f=f/256;      printf("PST (mg)\t %.2f\n",f);
        f=eg;   f=f/256;      printf("PST (eg)\t %.2f\n\n",f);

        x=pmob+lmob+tmob; y=abs(x); y=mobtab[y]; if (x<0) y=y*-1;
        f=y; f=f/256;        printf("Mobility\t %.2f  (pmob=%d) (lmob=%d) (tmob=%d)\n",f,pmob,lmob,tmob);

        f=formpawn; f=f/256;  printf("Pawn eval\t %.2f\n",f);
        f=paspawn;  f=f/256;  printf("Pass Pawn eval\t %.2f\n",f);
        f=kingsave; f=f/256;  printf("King Safety\t %.2f\n",f);

        f=w3; printf("Attacked piece\t\t %c%c (expected gain %.2f)\n",ta[w2],tn[w2],f);
        f=z3; printf("Hanging piece (1)\t %c%c (expected loss %.2f)\n",ta[z2],tn[z2],f);
        f=z6; printf("Hanging piece (2)\t %c%c (expected loss %.2f)\n",ta[z5],tn[z6],f);

        printf("Press a key to continue "); getch(); printf("\n\n");
}

//  Einde verhaal