#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

//TFT pin define
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

//TS pin define
#define YP A3
#define XM A2
#define YM 9
#define XP 8

//Color
#define  BLOCK1       0x8201
#define  BLOCK2       0x9300
#define  BACKGROUND   0x000A

//Size
#define CHESS 12
#define BOXSIZE 30
#define BOARD 240

void StartupScreen();
int MessageBox(char*, char, char);
void DrawChkBoard();
void PutChess(char, char, char, char);
void DrawChess(char);
void DrawButton(int, int, int, int, char*, char, char, char);
void DrawNWBTN();
void NewGame();
void GetTSPoint();
void WhereCan();
void DrawWhereCan();
void Gameplay();
void HowMany(char);
int PassWins(char);
void RefreshP1();
void RefreshP3();
void saveBoard(int);
void callBoard(int);
int power(int, int);
void printChess();
void Undo();
void EEBITWRITE(int Address, char Bit, unsigned char Value);
char EEBITREAD(int Address, char Bit);



















