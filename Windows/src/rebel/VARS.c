// 		EPD
//		===
static char epd[255];
static char bugnr;
static char fentab[]   = "??PNBRQKpnbrqk??";
static char fenkleur[] = "wb??";
static int  first_time=0;

static char bordpos[] =
                {A8,B8,C8,D8,E8,F8,G8,H8,
                 A7,B7,C7,D7,E7,F7,G7,H7,
        		 A6,B6,C6,D6,E6,F6,G6,H6,
                 A5,B5,C5,D5,E5,F5,G5,H5,
                 A4,B4,C4,D4,E4,F4,G4,H4,
                 A3,B3,C3,D3,E3,F3,G3,H3,
		         A2,B2,C2,D2,E2,F2,G2,H2,
                 A1,B1,C1,D1,E1,F1,G1,H1,RR};

static int mg,eg,wmat,quadrant;
static char white_castle,black_castle;

FILE *fp;

static char hang_tab[920000];                           // hang_wit (zwart)

static char bgboard[280];
static char *bord = &bgboard[21];
static char *bordm1  = &bgboard[20];
static char *bordm2  = &bgboard[19];
static char *bordm9  = &bgboard[12];
static char *bordm10 = &bgboard[11];
static char *bordm11 = &bgboard[10];
static char *bordp1  = &bgboard[22];
static char *bordp2  = &bgboard[23];
static char *bordp9  = &bgboard[30];
static char *bordp10 = &bgboard[31];
static char *bordp11 = &bgboard[32];
static char *bordp8  = &bgboard[29];
static char *bordp12 = &bgboard[33];
static char *bordp19 = &bgboard[40];
static char *bordp21 = &bgboard[42];
static char *bordm8  = &bgboard[13];
static char *bordm12 = &bgboard[9];
static char *bordm19 = &bgboard[2];
static char *bordm21 = &bgboard[0];

static char *bordp3  = &bgboard[24];		// specials ivm Pmob
static char *bordp23 = &bgboard[44];
static char *bordp30 = &bgboard[51];
static char *bordp32 = &bgboard[53];
static char *bordm17 = &bgboard[4];
static char *bordm28 = &bgboard[-7];
static char *bordm30 = &bgboard[-9];

static char *bordm3  = &bgboard[18];		// specials ivm vrypion
long *p_bord = (long *) &bgboard[21];       // 32 bits herdefinitie BORD

static char veldwz[40];
static char *veldwz_m1 = &veldwz[-1];
static char *veldwz_xx = &veldwz[0];
static char *vwz       = &veldwz[0];

static char bgwpnum[16];
static char *wpnum   = &bgwpnum[1];
static char *wpnumm1 = &bgwpnum[0];
static char *wpnump1 = &bgwpnum[2];
long *p_wpnum = (long *) &bgwpnum[1];

static char bgzpnum[16];
static char *zpnum   = &bgzpnum[1];
static char *zpnumm1 = &bgzpnum[0];
static char *zpnump1 = &bgzpnum[2];
long *p_zpnum = (long *) &bgzpnum[1];

static char wpdup[8];
static char zpdup[8];

//  	Algemene variabelen   (1)
//		=========================

static char xw,xz,xwp,xzp,xwz;
static char opp;
static char wkveld,zkveld;
static char wpawn,zpawn,wpprom,zpprom;

static int  h1,h2,h4,h5,a,hl;
static char unsigned xr3,xr4;
static int  r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14;
static int  x,y,z;
static int  d,e,v;
static long signed mp,mpi;

static int  kleur;
static int  sc,xsc,y0;
static int  wpnt1,wpnt2,zpnt1,zpnt2,zpnt3;

static long signed remscore;

static char *copyb;				// zie Attack (wd)
static char *copyq;				// zie Attack (wd)

//	Variabelen Posgen
//	=================

static int  wpos,wsom,zsom,h3;
static int  pmob,lmob,tmob,formpawn,paspawn,kingsave,mob;

static char w2,w3,w6,z2,z3,z5,z6;
static char wpval,zpval,wpveld,zpveld;
static char pawn,loper;


// 	Declaratie <bord2> en <bord3> met de herdefinities
//	==================================================

static char bgbord3[256];
static char *bord3 = &bgbord3[21];
long *p_bord3 = (long *) bgbord3;		// voor 32 bit clear !!
static char *bord3m1  = &bgbord3[20];
static char *bord3m2  = &bgbord3[19];
static char *bord3m3  = &bgbord3[18];
static char *bord3m9  = &bgbord3[12];
static char *bord3m10 = &bgbord3[11];
static char *bord3m11 = &bgbord3[10];
static char *bord3p1  = &bgbord3[22];
static char *bord3p2  = &bgbord3[23];
static char *bord3p3  = &bgbord3[24];
static char *bord3p9  = &bgbord3[30];
static char *bord3p10 = &bgbord3[31];
static char *bord3p11 = &bgbord3[32];
static char *bord3p8  = &bgbord3[29];
static char *bord3p12 = &bgbord3[33];
static char *bord3p19 = &bgbord3[40];
static char *bord3p21 = &bgbord3[42];
static char *bord3m8  = &bgbord3[13];
static char *bord3m12 = &bgbord3[9];
static char *bord3m19 = &bgbord3[2];
static char *bord3m21 = &bgbord3[0];


static char bgbord2[256];
static char *bord2 = &bgbord2[21];
long *p_bord2 = (long *) bgbord2;
static char *bord2m1  = &bgbord2[20];
static char *bord2m2  = &bgbord2[19];
static char *bord2m3  = &bgbord2[18];
static char *bord2m9  = &bgbord2[12];
static char *bord2m10 = &bgbord2[11];
static char *bord2m11 = &bgbord2[10];
static char *bord2p1  = &bgbord2[22];
static char *bord2p2  = &bgbord2[23];
static char *bord2p3  = &bgbord2[24];
static char *bord2p9  = &bgbord2[30];
static char *bord2p10 = &bgbord2[31];
static char *bord2p11 = &bgbord2[32];
static char *bord2p8  = &bgbord2[29];
static char *bord2p12 = &bgbord2[33];
static char *bord2p19 = &bgbord2[40];
static char *bord2p21 = &bgbord2[42];
static char *bord2m8  = &bgbord2[13];
static char *bord2m12 = &bgbord2[9];
static char *bord2m19 = &bgbord2[2];
static char *bord2m21 = &bgbord2[0];
static char *bord2_m7 = &bgbord2[14];		// zie ex10d


//	Einde verhaal


