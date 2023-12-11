#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <EEPROM.h>
#include "Reversi.h"
#include "U16.h"

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

char *version = "version 1.2b";
                //LSB                            MSB
char Board[8][8] = {{-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1,  0,  1, -1, -1, -1}, 
                    {-1, -1, -1,  1,  0, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}};
char process = 0;
char player = 1;
char hint = 1;
int cnt0 = 0;
unsigned long m1 = millis();
TSPoint p;


void setup()
{
  Serial.begin(9600);
  
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier==0x0101) identifier=0x9341;
  tft.begin(identifier);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop()
{
  int qx;
  int qy;
  
  GetTSPoint();
  
  switch(process){
/******************************/ //Startup initial
    case 0:
    StartupScreen(version);
    hint = EEBITREAD(1022, 1);
    NewGame();
    process=1;
    break;
/******************************/
    case 1:
    qx = p.x/30;
    qy = (p.y*1.32)/40;
    if(p.z>100 && p.z<1000)
    {
      if(qy<8)
      {
        if(Board[qy][qx] == 2)
        {
          Gameplay(qx, qy, player);
          if(player == 0) player=1;
          else player = 0;
          WhereCan(player);
          DrawChess(0);
          HowMany(player);
          switch (PassWins(player))
          {
            case 0:
            MessageBox("White wins!", 11, 1);
            RefreshP1();
            break;
            
            case 1:
            MessageBox("Black wins!", 11, 1);
            RefreshP1();
            break;
            
            case 2:
            MessageBox("Tie.", 4, 1);
            RefreshP1();
            break;
            
            case 3:
            MessageBox("Pass", 4, 0);
            if(player == 0) player=1;
            else player = 0;
            RefreshP1();
            break;
          }
          DrawWhereCan();
          saveBoard(cnt0);
          if(cnt0<960) cnt0+=16;
          Serial.println(cnt0, DEC);
        }
      }
      else if(p.x>5 && p.y>290 && p.x<115 && p.y<315) //undo
      {
        Undo();
      }
      else if(p.x>125 && p.y>290 && p.x<235 && p.y<315) //Back
      {
        RefreshP3();
        process=3;
      }
    }
    break;
/******************************/
    case 2:
    StartupScreen(version);
    NewGame();
    process=1;
    break;
/******************************/
    case 3:
    if(p.x>20 && p.z>100 && p.x<220 && p.z<1000)
    {
      if(p.y>23 && p.y<48) //Hint
      {
        if(hint==0)
        {
          hint=1;
          MessageBox("Hint on.", 7, 0);
        }
        else
        {
          hint=0;
          MessageBox("Hint off.", 8, 0);
        }
        EEBITWRITE(1022, 1, hint);
        RefreshP1();
        process=1;
      }
      else if(p.y>73 && p.y<98) //New game
      {
        if(MessageBox("Sure?", 5, 2)) NewGame();
        else RefreshP3();
        process=1;
      }
      else if(p.y>123 && p.y<148) //Save game
      {
        if(MessageBox("Sure to save?", 13, 2))
        {
          saveBoard(977);
          EEBITWRITE(1022, 0, player);
          MessageBox("Saved.", 6, 0);
          RefreshP3();
        }
        else RefreshP3();
      }
      else if(p.y>173 && p.y<198) //Call game
      {
        if(MessageBox("Sure to call?", 13, 2))
        {
          player = EEBITREAD(1022, 0);
          cnt0=0;
          callBoard(977);
          saveBoard(cnt0);
          cnt0+=16;
          WhereCan(player);
          RefreshP1();
          process=1;
        }
      }
      else if(p.y>223 && p.y<248) //Start up screen
      {
        if(MessageBox("Sure?", 5, 2)) process=2;
        else RefreshP3();
      }
      else if(p.y>273 && p.y<298) //Back
      {
        RefreshP1();
        process=1;
      }
    }
/******************************/
  }
}

void StartupScreen(char *ver)
{
  unsigned long m = millis();
  char f = 0;
  tft.fillScreen(Purple);
  tft.setTextColor(Green);
  tft.setCursor(22, 77);
  tft.setTextSize(3);
  tft.print("8-bit");
  tft.setCursor(45, 100);
  tft.setTextSize(4);
  tft.print("Reversi!");
  tft.setCursor(138, 135);
  tft.setTextSize(1);
  tft.print(ver);
  tft.fillRect(130, 170, 40, 40, BLOCK2);
  tft.fillRect(130, 210, 40, 40, BLOCK1);
  tft.fillRect(170, 170, 40, 40, BLOCK1);
  tft.fillRect(170, 210, 40, 40, BLOCK2);
  tft.fillCircle(150, 190, 16, Black);
  tft.fillCircle(150, 230, 16, White);
  tft.fillCircle(190, 190, 16, White);
  tft.fillCircle(190, 230, 16, Black);
  if(process==0) delay(2000);
  tft.setTextColor(Yellow);
  tft.setCursor(36, 277);
  tft.setTextSize(2);
  tft.print("Press to Start");
  while(1)
  {
    GetTSPoint();
    if(p.z>10 && p.z<1000) break;
    
    if(millis() - m > 800)
    {
      if(f==0)
      {
        tft.setTextColor(Purple);
        tft.setCursor(36, 277);
        tft.print("Press to Start");
        f = 1;
      }
      else
      {
        tft.setTextColor(Yellow);
        tft.setCursor(36, 277);
        tft.print("Press to Start");
        f = 0;
      }
      m = millis();
    }
  }
}

int MessageBox(char *text, char len, char mode)
{
  Serial.print(len, DEC);
  tft.fillRect(30, 75, 180, 105, Navy);
  tft.fillRect(32, 77, 176, 101, DarkGrey);
  tft.setCursor(tft.width()/2 - len*6, tft.height()/2 - 56);
  tft.setTextColor(Yellow);
  tft.setTextSize(2);
  tft.print(text);
  switch(mode){
    case 0:
      delay(1500);
      return 1;
    case 1:
      DrawButton(92, 143, 56, 24, "ok", 2, 17, 5);
      while(1)
      {
        GetTSPoint();
        if(p.z>100 && p.z<1000)
        {
          if(p.x>94 && p.y>145 && p.x<146 && p.y<165)
            return 1;
        }
      }
    case 2:
      DrawButton(53, 143, 56, 24, "Yes", 2, 11, 5);
      DrawButton(130, 143, 56, 24, "No", 2, 17, 5);
      while(1)
      {
        GetTSPoint();
        if(p.z>100 && p.z<1000)
        {
          if(p.x>58 && p.y>145 && p.x<104 && p.y<165)
            return 1;
          if(p.x>135 && p.y>145 && p.x<181 && p.y<165)
            return 0;
        }
      }
  }
}

void DrawChkBoard()
{
  tft.fillScreen(BACKGROUND);

//tft.fillRect( xpoint, ypoint, xw, yw, color);
//draw a Board
  for(int i=0; i<10; i+=2){
    for(int j1=0; j1<8; j1+=2)
      tft.fillRect(i*BOXSIZE, j1*BOXSIZE, BOXSIZE, BOXSIZE,BLOCK1);
    for(int j2=1; j2<8; j2+=2)
      tft.fillRect(i*BOXSIZE, j2*BOXSIZE, BOXSIZE, BOXSIZE, BLOCK2);
  }
  for(int i=1; i<10; i+=2){
    for(int j1=0; j1<8; j1+=2)
      tft.fillRect(i*BOXSIZE, j1*BOXSIZE, BOXSIZE, BOXSIZE, BLOCK2);
    for(int j2=1; j2<8; j2+=2)
      tft.fillRect(i*BOXSIZE, j2*BOXSIZE, BOXSIZE, BOXSIZE, BLOCK1);
  }
}

void PutChess(char qx, char qy, char chesscolor, char undo)
{
  if(chesscolor == 0)
    tft.fillCircle(BOXSIZE*qx+BOXSIZE/2, BOXSIZE*qy+BOXSIZE/2, CHESS, White);
  else if(chesscolor == 1)
    tft.fillCircle(BOXSIZE*qx+BOXSIZE/2, BOXSIZE*qy+BOXSIZE/2, CHESS, Black);
  else if((chesscolor == -1 || chesscolor == 2)&&(undo!=0))
  {
    if((qx+qy)%2==1) tft.fillCircle(BOXSIZE*qx+BOXSIZE/2, BOXSIZE*qy+BOXSIZE/2, CHESS, BLOCK2);
    else tft.fillCircle(BOXSIZE*qx+BOXSIZE/2, BOXSIZE*qy+BOXSIZE/2, CHESS, BLOCK1);
  }
}

void DrawChess(char undo)
{
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      PutChess(i, j, Board[j][i], undo);
    }
  }
    
}

void DrawButton(int x, int y, int size_x, int size_y, char *text, char size_txt, char offset_x, char offset_y)
{
  tft.fillRect(x, y, size_x, size_y, LightGrey);
  tft.drawRect(x, y, size_x, size_y, Yellow);
  tft.setTextColor(Navy);
  tft.setCursor(x+offset_x, y+offset_y);
  tft.setTextSize(size_txt);
  tft.print(text);
}

void NewGame()
{
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      if((i==3 && j==3) || (i==4 && j==4)) Board[j][i] = 0;
      else if((i==3 && j==4) || (i==4 && j==3)) Board[j][i] = 1;
      else Board[j][i] = -1;
    }
  }
  player = 1;
  cnt0=0;
  saveBoard(cnt0);
  cnt0+=16;
  DrawChkBoard();
  WhereCan(player);
  DrawChess(0);
  DrawWhereCan();
  HowMany(player);
  DrawButton(0, 285, 120, 35, "Undo", 2, 37, 11);
  DrawButton(120, 285, 120, 35, "Options", 2, 19, 11);
}

void GetTSPoint()
{
  p = ts.getPoint();
  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  p.x = map(p.x, 189, 901, 240, 0);
  p.y = map(p.y, 167, 911, 320, 0);
}

void WhereCan(char player)
{
  char me = player;
  char notme = (player+1)%2;
  for(int x=0; x<8; x++){
    for(int y=0; y<8; y++){
      if(Board[y][x]==me){
        for(char j=1; j<7; j++){
          //top(y--)
          if(Board[y-j][x]==2 || Board[y-j][x]==-1 || (y-j)<1){
            break;
          }else if(Board[y-j][x]==notme && (Board[y-j-1][x]==-1 || Board[y-j-1][x]==2)){
            Board[y-j-1][x]=2;
            break;
          }
        }
        for(char j=1; j<7; j++){
          //bottom(y++)
          if(Board[y+j][x]==2 || Board[y+j][x]==-1 || (y+j)>6){
            break;
          }else if(Board[y+j][x]==notme && (Board[y+j+1][x]==-1 || Board[y+j+1][x]==2)){
            Board[y+j+1][x]=2;
            break;
          }
        }
        for(char j=1; j<7; j++){
          //left(x--)
          if(Board[y][x-j]==2 || Board[y][x-j]==-1 || (x-j)<1){
            break;
          }else if(Board[y][x-j]==notme && (Board[y][x-j-1]==-1 || Board[y][x-j-1]==2)){
            Board[y][x-j-1]=2;
            break;
          }
        }
        for(char j=1; j<7; j++){
          //right(x++)
          if(Board[y][x+j]==2 || Board[y][x+j]==-1 || (x+j)>6){
            break;
          }else if(Board[y][x+j]==notme && (Board[y][x+j+1]==-1 || Board[y][x+j+1]==2)){
            Board[y][x+j+1]=2;
            break;
          }
        }
        for(char j=1; j<7; j++){
          //top left(y--x--)
          if(Board[y-j][x-j]==2 || Board[y-j][x-j]==-1 || (y-j)<1 || (x-j)<1){
            break;
          }else if(Board[y-j][x-j]==notme && (Board[y-j-1][x-j-1]==-1 || Board[y-j-1][x-j-1]==2)){
            Board[y-j-1][x-j-1]=2;
            break;
          }
        }
        for(char j=1; j<7; j++){
          //bottom right(y++x++)
          if(Board[y+j][x+j]==2 || Board[y+j][x+j]==-1 || (y+j)>6 || (x+j)>6){
            break;
          }else if(Board[y+j][x+j]==notme && (Board[y+j+1][x+j+1]==-1 || Board[y+j+1][x+j+1]==2)){
            Board[y+j+1][x+j+1]=2;
            break;
          }
        }
        for(char j=1; j<7; j++){
          //bottom left(y++x--)
          if(Board[y+j][x-j]==2 || Board[y+j][x-j]==-1 || (y+j)>6 || (x-j)<1){
            break;
          }else if(Board[y+j][x-j]==notme && (Board[y+j+1][x-j-1]==-1 || Board[y+j+1][x-j-1]==2)){
            Board[y+j+1][x-j-1]=2;
            break;
          }
        }
        for(char j=1; j<7; j++){
          //top right(y--x++)
          if(Board[y-j][x+j]==2 || Board[y-j][x+j]==-1 || (y-j)<1 || (x+j)>6){
            break;
          }else if(Board[y-j][x+j]==notme && (Board[y-j-1][x+j+1]==-1 || Board[y-j-1][x+j+1]==2)){
            Board[y-j-1][x+j+1]=2;
            break;
          }
        }
      }
    }
  }
}

void DrawWhereCan()
{
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      if(Board[j][i]==2 && hint==1) tft.fillCircle(BOXSIZE*i+BOXSIZE/2, BOXSIZE*j+BOXSIZE/2, 6, DarkGrey);
      else if(Board[j][i]==-1)
      {
        if((i+j)%2==1) tft.fillCircle(BOXSIZE*i+BOXSIZE/2, BOXSIZE*j+BOXSIZE/2, 6, BLOCK2);
        else tft.fillCircle(BOXSIZE*i+BOXSIZE/2, BOXSIZE*j+BOXSIZE/2, 6, BLOCK1);
      }
    }
  }
}

void Gameplay(char x, char y, char player)
{
  //Q
  char me=player;
  //Q'
  char notme=(player+1)%2;
  if(Board[y][x] == 2){
    Board[y][x]=3;
    //Top
    char much = 0;
    for(char i=1; i<7; i++){
      if((y-i)>0 && Board[y-i][x]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y-1-much][x]==me){
      for(char j=1; j<=much; j++){
        Board[y-j][x]=3;
      }
    }
    //Bottom
    much = 0;
    for(char i=1; i<7; i++){
      if((y+i)<7 && Board[y+i][x]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y+much+1][x]==me){
      for(char j=1; j<=much; j++){
        Board[y+j][x]=3;
      }
    }
    //Left
    much = 0;
    for(char i=1; i<7; i++){
      if((x-i)>0 && Board[y][x-i]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y][x-much-1]==me){
      for(char j=1; j<=much; j++){
        Board[y][x-j]=3;
      }
    }
    //Right
    much = 0;
    for(char i=1; i<7; i++){
      if((x+i)<7 && Board[y][x+i]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y][x+much+1]==me){
      for(char j=1; j<=much; j++){
        Board[y][x+j]=3;
      }
    }
    //Left Top
    much = 0;
    for(char i=1; i<7; i++){
      if((y-i)>0 && (x-i)>0 && Board[y-i][x-i]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y-much-1][x-much-1]==me){
      for(char j=1; j<=much; j++){
        Board[y-j][x-j]=3;
      }
    }
    //Right Bottom
    much = 0;
    for(char i=1; i<7; i++){
      if((y+i)<7 && (x+i)<7 && Board[y+i][x+i]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y+much+1][x+much+1]==me){
      for(char j=1; j<=much; j++){
        Board[y+j][x+j]=3;
      }
    }
    //Left Bottom
    much = 0;
    for(char i=1; i<7; i++){
      if((y+i)<7 && (x-i)>0 && Board[y+i][x-i]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y+much+1][x-much-1]==me){
      for(char j=1; j<=much; j++){
        Board[y+j][x-j]=3;
      }
    }
    //Right Top
    much = 0;
    for(char i=1; i<7; i++){
      if((y-i)>0 && (x+i)<7 && Board[y-i][x+i]==notme){
        much++;
      }else{
        break;
      }
    }
    if(Board[y-much-1][x+much+1]==me){
      for(char j=1; j<=much; j++){
        Board[y-j][x+j]=3;
      }
    }
    
  }
  for(char m=0; m<8; m++){
    for(char n=0; n<8; n++){
      if(Board[m][n]==3)Board[m][n]=me;
      if(Board[m][n]==2)Board[m][n]=-1;
    }
  }
}

void HowMany(char player_disp)
{
  int many0 = 0;
  int many1 = 0;
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      if(Board[j][i]==0) many0++;
      if(Board[j][i]==1) many1++;
    }
  }

  if(player_disp==1) tft.fillRect(0, 240, 90, 45, Orange);
  else tft.fillRect(0, 240, 90, 45, LightGrey);
  tft.fillRect(2, 242, 86, 41, LightGrey);
  tft.fillCircle(19, 263, CHESS*1.2, Black);
  tft.setCursor(40, 253);
  tft.setTextSize(3);
  tft.setTextColor(Navy);
  tft.print(many1, DEC);
  
  if(player_disp==0) tft.fillRect(90, 240, 90, 45, Orange);
  else tft.fillRect(90, 240, 90, 45, LightGrey);
  tft.fillRect(92, 242, 86, 41, LightGrey);
  tft.fillCircle(109, 263, CHESS*1.2, White);
  tft.setCursor(130, 253);
  tft.print(many0, DEC);
}

int PassWins(char player_calc)
{
  int flag = 3;
  int many0 = 0;
  int many1 = 0;
  int player_temp = player_calc;
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      if(Board[j][i]==2) flag=-1;
    }
  }
  
  if(flag==3)
  {
    if(player_calc==0) player_calc=1;
    else player_calc=0;
    WhereCan(player_calc);
    for(int i=0; i<8; i++)
    {
      for(int j=0; j<8; j++)
      {
        if(Board[j][i]==0) many0++;
        if(Board[j][i]==1) many1++;
        if(Board[j][i]==2) flag=-1;
      }
    }
    
    if(player_calc==0) player_calc=1;
    else player_calc=0;
    WhereCan(player_calc);
    
    if(flag==3)
    {
      if(many0>many1) return 0;
      if(many1>many0) return 1;
      if(many1==many0) return 2;
    }
    else return 3;
    
  }
  else return flag;
}

void RefreshP1()
{
  DrawChkBoard();
  DrawChess(0);
  DrawWhereCan();
  HowMany(player);
  DrawButton(0, 285, 120, 35, "Undo", 2, 37, 11);
  DrawButton(120, 285, 120, 35, "Options", 2, 19, 11);
}

void RefreshP3()
{
  tft.fillScreen(BACKGROUND);
  #define YOFFSET 12
  DrawButton(15, 30-YOFFSET, 210, 35, "Hint on/off", 2, 41, 11);
  DrawButton(15, 80-YOFFSET, 210, 35, "New game", 2, 56, 11);
  DrawButton(15, 130-YOFFSET, 210, 35, "Save game", 2, 51, 11);
  DrawButton(15, 180-YOFFSET, 210, 35, "Call game", 2, 51, 11);
  DrawButton(15, 230-YOFFSET, 210, 35, "Start up screen", 2, 15, 11);
  DrawButton(15, 280-YOFFSET, 210, 35, "Back", 2, 83, 11);
}

void saveBoard(int start)
{
  for(int i=0; i<8; i++)
  {
    unsigned char value0 = 0;
    unsigned char value1 = 0; 
    for(int j=0; j<8; j++)
    {
      if(Board[i][j]==0)
        value0 += power(2, j);
      else if(Board[i][j]==1)
        value1 += power(2, j);
    }
    EEPROM.write(start+i*2, value0);
    EEPROM.write(start+i*2+1, value1);
  }
}

void callBoard(int start)
{
  for(int i=0; i<8; i++)
  {
    unsigned char value0 = EEPROM.read(start+i*2);
    unsigned char value1 = EEPROM.read(start+i*2+1);
    for(int j=0; j<8; j++)
    {
      if(value0%2==1)
        Board[i][j]=0;
      else if(value1%2==1)
        Board[i][j]=1;
      else
        Board[i][j]=-1;
      value0=value0/2;
      value1=value1/2;
    }
  }
}

int power(int x, int y)
{
  if(y>0)
  {
    x = x*power(x, y-1);
    return x;
  }
  else
  {
    return 1;
  }
}

void printChess()
{
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      if(Board[i][j]<0) Serial.print(Board[i][j], DEC);
      else
      {
        Serial.print(" ");
        Serial.print(Board[i][j], DEC);
      }
    }
    Serial.println();
  }
  Serial.println();
}

void Undo()
{
  if(cnt0>=32)
  {
    cnt0-=32;
    callBoard(cnt0);
    if(player == 0) player=1;
    else player = 0;
    WhereCan(player);
    DrawChess(1);
    if(PassWins(player)==3)
    {
      if(player == 0) player=1;
      else player = 0;
    }
    HowMany(player);
    DrawWhereCan();
    if(cnt0<960) cnt0+=16;
  }
  else
  {
    MessageBox("Not available.", 14, 0);
    RefreshP1();
  }
}

void EEBITWRITE(int Address, char Bit, unsigned char Value)
{
  unsigned char val = EEPROM.read(Address);
  if((val/power(2, Bit))%2==1 && Value==0)
    val-=power(2, Bit);
  else if((val/power(2, Bit))%2==0 && Value==1)
    val+=power(2, Bit);
  EEPROM.write(Address, val);
}

char EEBITREAD(int Address, char Bit)
{
  unsigned char val = EEPROM.read(Address);
  val = val / power(2, Bit);
  return val%2;
}
