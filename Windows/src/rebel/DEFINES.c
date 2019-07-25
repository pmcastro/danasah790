#define DP          20		// Value double pawn

#define WHITE 	     0
#define BLACK 	     1

#define RR  0				// out of board
#define LL  1				// empty square

#define WP  2
#define WN  3
#define WL  4
#define WT  5
#define WD  6
#define WK  7

#define ZP  8
#define ZN  9
#define ZL  10
#define ZT  11
#define ZD  12
#define ZK  13

#define A1  1
#define A2  2
#define A3  3
#define A4  4
#define A5  5
#define A6  6
#define A7  7
#define	A8  8

#define B1  11
#define B2  12
#define B3  13
#define B4  14
#define B5  15
#define B6  16
#define B7  17
#define B8  18

#define C1  21
#define C2  22
#define C3  23
#define C4  24
#define C5  25
#define C6  26
#define C7  27
#define C8  28

#define D1  31
#define D2  32
#define D3  33
#define D4  34
#define D5  35
#define D6  36
#define D7  37
#define D8  38

#define E1  41
#define E2  42
#define E3  43
#define E4  44
#define E5  45
#define E6  46
#define E7  47
#define E8  48

#define F1  51
#define F2  52
#define F3  53
#define F4  54
#define F5  55
#define F6  56
#define F7  57
#define F8  58

#define G1  61
#define G2  62
#define G3  63
#define G4  64
#define G5  65
#define G6  66
#define G7  67
#define G8  68

#define H1  71
#define H2  72
#define H3  73
#define H4  74
#define H5  75
#define H6  76
#define H7  77
#define H8  78


//	Functions
//	=========

int evaluation();
int hang_wit ();
int hang_zwart ();
int zksave ();
int wksave ();
int wn ();
int zn ();
int wl ();
int zl ();
int wt ();
int zt ();
int wd ();
int zd ();
int wp ();
int zp ();
int wk ();
int zk ();

int kwadraat ();						// quadrant rule in pawn endings

int init ();
int zoekveld ();
int restant ();
int setpnt ();
int vrypion ();
int wpinit ();
int zpinit ();

void get_fen();
void debug();

//	Einde verhaal




