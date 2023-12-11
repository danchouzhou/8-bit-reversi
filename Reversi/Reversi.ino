#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include "Reversi.h"
#include "U16.h"

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

char *version = "version 1.0";
char Board[8][8] = {{-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1,  0,  1, -1, -1, -1}, 
                    {-1, -1, -1,  1,  0, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}, 
                    {-1, -1, -1, -1, -1, -1, -1, -1}};
char process = 0;
char player = 0;
char tsflag = 0;
char wins = -1;
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
          DrawChess();
          HowMany(player);
          switch (PassWins(player))
          {
            case 0:
            MessageBox("White wins!", 11, 1);
            DrawChkBoard();
            DrawChess();
            DrawNWBTN();
            DrawButton(90, 285, 90, 35, "Back", 2, 22, 11);
            HowMany(player);
            break;
            
            case 1:
            MessageBox("Black wins!", 11, 1);
            DrawChkBoard();
            DrawChess();
            DrawNWBTN();
            DrawButton(90, 285, 90, 35, "Back", 2, 22, 11);
            HowMany(player);
            break;
            
            case 2:
            MessageBox("Tie.", 4, 1);
            DrawChkBoard();
            DrawChess();
            DrawNWBTN();
            DrawButton(90, 285, 90, 35, "Back", 2, 22, 11);
            HowMany(player);
            break;
            
            case 3:
            MessageBox("Pass", 4, 1);
            DrawChkBoard();
            DrawChess();
            DrawNWBTN();
            DrawButton(90, 285, 90, 35, "Back", 2, 22, 11);
            if(player == 0) player=1;
            else player = 0;
            HowMany(player);
            WhereCan(player);
            break;
          }
          DrawWhereCan();
        }
      }
      else if(p.x>5 && p.y>290 && p.x<85 && p.y<315) //New Game
      {
        NewGame();
      }
      else if(p.x>95 && p.y>290 && p.x<175 && p.y<315) //Back
      {
        process=2;
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
    DrawChkBoard();
    MessageBox("Hello World", 11, 2);
    process++;
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
  tft.fillRect(45, 75, 150, 105, Navy);
  tft.fillRect(47, 77, 146, 101, DarkGrey);
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
      DrawButton(61, 143, 56, 24, "Yes", 2, 11, 5);
      DrawButton(122, 143, 56, 24, "No", 2, 17, 5);
      while(1)
      {
        GetTSPoint();
        if(p.z>100 && p.z<1000)
        {
          if(p.x>63 && p.y>145 && p.x<115 && p.y<165)
            return 1;
          if(p.x>124 && p.y>145 && p.x<176 && p.y<165)
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

void PutChess(int qx, int qy, int chesscolor)
{
  if(chesscolor == 0)
    tft.fillCircle(BOXSIZE*qx+BOXSIZE/2, BOXSIZE*qy+BOXSIZE/2, CHESS, White);
  else if(chesscolor == 1)
    tft.fillCircle(BOXSIZE*qx+BOXSIZE/2, BOXSIZE*qy+BOXSIZE/2, CHESS, Black);
}

void DrawChess()
{
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      PutChess(i, j, Board[j][i]);
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

void DrawNWBTN()
{
  tft.fillRect(0, 285, 90, 35, LightGrey);
  tft.drawRect(0, 285, 90, 35, Yellow);
  tft.setTextColor(Navy);
  tft.setCursor(2, 296);
  tft.setTextSize(2);
  tft.print("New");
  tft.setCursor(43, 296);
  tft.print("Game");
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
  wins = -1;
  DrawChkBoard();
  WhereCan(player);
  DrawChess();
  DrawWhereCan();
  HowMany(player);
  DrawNWBTN();
  DrawButton(90, 285, 90, 35, "Back", 2, 22, 11);
  DrawButton(180, 285, 60, 35, "options", 1, 10, 14);
}

void GetTSPoint()
{
  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);
  
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
      if(Board[j][i]==2) tft.fillCircle(BOXSIZE*i+BOXSIZE/2, BOXSIZE*j+BOXSIZE/2, 6, DarkGrey);
      else if(Board[j][i]==-1)
      {
        if((i+j)%2==1) tft.fillCircle(BOXSIZE*i+BOXSIZE/2, BOXSIZE*j+BOXSIZE/2, 6, BLOCK2);
        else tft.fillCircle(BOXSIZE*i+BOXSIZE/2, BOXSIZE*j+BOXSIZE/2, 6, BLOCK1);
      }
      Serial.print(Board[j][i], DEC);
    }
    Serial.println();
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
    else return flag;
    
  }
  else return flag;
}
