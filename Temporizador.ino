#include <Wire.h>
#include <Sodaq_DS3231.h>
#include <LiquidCrystal.h>
#define NumTimer 3 
#define BtnEnter 1
#define BtnUp 2
#define BtnDown 3
#define BtnBack 4
#define BtnNone 0
#define NumSalidas 3
uint8_t rs = 13, rw=12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);
char DiaSemana[][4] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab" };
uint8_t Salida[NumSalidas] ={6, 7, 8}; //configurar pines digitales de acuerdo al numero de salidas si se modifica num salidas el vector se modifica de acuerdo a los pines de salida
short BtnAdc = 0;
int8_t C_NAV=0, C_TIMER=0, C_MENU=1, C_ExitMenu=0, C_ITEMH=0, C_ENTER=0;                         
bool DATE[NumTimer][7]={{1,1,1,1,1,1,1},  //configuracion de dias de cada timer#####################################
                        {1,1,1,1,1,1,1},
                        {1,1,1,1,1,1,1}};
uint8_t Days[7]={1,2,3,4,5,6,7};// DIAS L,M,M,J,V,S,D ##############################################################
uint8_t Timer1[NumTimer][5]= {{0,0,3,1,0}, // ## H-M-S-ON/OFF-OUT ####CONFIGURACION DE TIEMPO PARA CADA TIMER
                              {0,0,3,1,0},
                              {0,0,0,1,0}};
uint8_t btnstate=0;
void setup ()
{
  Serial.begin(9600);
  //Wire.begin();
  rtc.begin();
  lcd.begin(16, 2);
  lcd.clear();
  //rtc.setDateTime(dt);
  for (byte i = 0; i < NumSalidas; i++)
  {
   pinMode(Salida[i], OUTPUT);
  }
}

void loop ()
{
  if(analogRead(1)==0 && analogRead(1)<=150){btnstate=BtnNone;}
  if(analogRead(1)>=151){btnstate=readBtns();}
delay(2);
Temporizador_por_Salida2();

if (btnstate == BtnEnter || C_ExitMenu!=0)
{
    if(C_ExitMenu==0){btnstate=BtnNone;}
    C_ExitMenu=1;
    if (C_NAV==0){C_MENU=Navegacion(C_MENU, 4);}
    if (btnstate==BtnBack){C_ExitMenu=0;delay(1);btnstate=BtnNone;}
    switch (C_MENU)
    {
    case 1: //SELECCIONE UN TIMER A CONF ##############################################################3
      if (C_NAV==0)
      {
          lcd.setCursor(0, 0);
          lcd.print("CONFIG TIMER ");
          lcd.print(C_TIMER);
          lcd.setCursor(0, 1);
          lcd.print("SELECT TIMER");
          delay(300);
          lcd.clear(); 
      }
      if ( btnstate==BtnEnter || C_NAV!=0)
      {
        if(C_NAV==0)btnstate=BtnNone;
        delay(1);
        C_NAV=1;
        C_ENTER=Navegacion1(C_ENTER, 3);
        if (C_ENTER==0){C_TIMER=incremento_01(C_TIMER, NumTimer);}
        if (C_ENTER==2){C_ITEMH=NumSalidas;}
        if (C_ENTER==1){C_ITEMH=2;} 
        if (C_ENTER!=0){Timer1[C_TIMER][C_ENTER+2]=incremento_01(Timer1[C_TIMER][C_ENTER+2], C_ITEMH);} 
        lcd.setCursor(0, 0);
        lcd.print("TIMER |OUT| O/F");
        lcd.setCursor(2, 1);
        lcd.print(C_TIMER);
        lcd.setCursor(7, 1);
        lcd.print(Timer1[C_TIMER][4]);
        lcd.print(" | ");
        if ( Timer1[C_TIMER][3]==1)lcd.print(" ON ");
        if ( Timer1[C_TIMER][3]==0)lcd.print(" OFF ");
        delay(300);
        lcd.clear();
        if (btnstate==BtnBack){C_NAV=0; C_ITEMH=0; C_ENTER=0; btnstate=BtnNone;}
      }
      break;

    case 2: //SELECCIONE DIAS ################################################################
      if (C_NAV == 0)
      {
        lcd.setCursor(0, 0);
        lcd.print("CONFIG TIMER ");
        lcd.print(C_TIMER);
        lcd.setCursor(0, 1);
        lcd.print("SELECIONAR DIAS");
        delay(150);
        lcd.clear();
      }
      if ( btnstate==BtnEnter || C_NAV!=0)
      {
        //delay(200);
        if(C_NAV==0)btnstate=BtnNone;
        C_NAV=1;
        C_ENTER=Navegacion1(C_ENTER, 7);
        DATE[C_TIMER][C_ENTER]=incremento_01(DATE[C_TIMER][C_ENTER], 2);
        lcd.setCursor(0, 0);
        lcd.print("L M M J V S D");
        lcd.setCursor(0, 1);
        for (int8_t i=0; i<7; i++)
        {
          if (DATE[C_TIMER][i]!=0)
          {
            lcd.print("O");
            lcd.print("|");
          }
          else
          {
            lcd.print("X");
            lcd.print("|");
          }  
        }
        delay(100);
        lcd.clear();
        if (btnstate==BtnBack){ C_NAV=0; C_ENTER=0;btnstate=BtnNone;}
      }
      break;

    case 3: //SELECCIONE HORA #####################################################################
      if (C_NAV==0)
      {
        lcd.setCursor(0, 0);
        lcd.print("CONFIG TIMER ");
        lcd.print(C_TIMER);
        lcd.setCursor(0, 1);
        lcd.print("Seleccione Hora");
        delay(150);
        lcd.clear();
      }
      if ( btnstate==BtnEnter || C_NAV!=0)
      {
        if(C_NAV==0)btnstate=BtnNone;
        delay(1);
        C_NAV=1;
        C_ENTER=Navegacion1(C_ENTER, 3);
        if (C_ENTER==0){C_ITEMH=24;}
        if (C_ENTER==1){C_ITEMH=60;}
        if (C_ENTER==2){C_ITEMH=60;}
        Timer1[C_TIMER][C_ENTER]=incremento_01(Timer1[C_TIMER][C_ENTER], C_ITEMH);
        lcd.setCursor(0, 0);
        lcd.print("TIMER");
        lcd.print(C_TIMER);
        lcd.print(" H | M| S");
        lcd.setCursor(7, 1);
        lcd.print(Timer1[C_TIMER][0]);
        lcd.print(":");
        lcd.print(Timer1[C_TIMER][1]);
        lcd.print(":");
        lcd.print(Timer1[C_TIMER][2]);
        delay(150);
        lcd.clear();
        if (btnstate==BtnBack){C_NAV=0;C_ENTER=0;delay(50);btnstate=BtnNone;}
      }
      break;

    case 4: //SALIR#########################################################################################
      lcd.setCursor(0, 0);
      lcd.print("CONFIG TIMER ");
	    lcd.print(C_TIMER);
      lcd.setCursor(0, 1);;
      lcd.print("SALIR");
      delay(150);
      lcd.clear();
      if (btnstate==BtnEnter)
       {
        btnstate=BtnNone;
        C_ExitMenu=0;
        delay(1);
       }
      break;

    default:
      break;
    }
}
//######################################################################################################
else
{
  pantalla_principal();
}

}
//FUNCIONES #############################################################################################
void Temporizador_por_Salida2()
{
  DateTime now = rtc.now();
  for (int i = 0; i < NumTimer; i++)//verifica la programacion de cada timer
    {
        for (int j = 0; j < 7; j++)
        {
          if (DATE[i][j]==1)
          {
            if ((now.dayOfWeek()) >= j+1) //Verifica los dias de la semana asignados a cada timer
                {
                if(now.hour()==Timer1[i][0] && now.minute()==Timer1[i][1] && now.second()==Timer1[i][2])  //verifica el tiempo de cada timer
                    {
                                  //Salida      on/off
                    digitalWrite( Salida[Timer1[i][4]], Timer1[i][3]);
                    }
                } 
          }

        }
    }
}

int8_t incremento_01(int8_t item, int8_t num_item)
{
if (btnstate==BtnUp) 
    {
      item++;
      if ( item == num_item)
      {
        item = 0;
      }
      btnstate=BtnNone;
    }
    if (btnstate ==BtnDown) 
    {
      item--;
      if ( item == -1 )
      {
        item = num_item-1;
      }
      btnstate=BtnNone;
    }
    return item; 
}
int8_t Navegacion(int8_t item, int8_t num_item)
{
if (btnstate==BtnUp) 
    {
      item++;
      if ( item == num_item+1)
      {
        item = 1;
      }
      btnstate=BtnNone;
    }
    if (btnstate ==BtnDown) 
    {
      item--;
      if ( item == 0 )
      {
        item = num_item;
      }
      btnstate=BtnNone;
    }
    return item; 
}

int8_t Navegacion1(int8_t item, int8_t num_item)
{
if (btnstate==BtnEnter) 
    {
      item++;
      if ( item == num_item)
      {
        item = 0;
      }
      btnstate=BtnNone;
    }
    return item; 
}

int8_t readBtns()
{
  BtnAdc=analogRead(1);
delay(100);
  if ( BtnAdc >= 1019) return BtnEnter;
  if ( BtnAdc >= 800 && BtnAdc <= 850 ) return BtnUp;  
  if ( BtnAdc >= 600 && BtnAdc <= 630 ) return BtnDown;  
  if ( BtnAdc >= 390 && BtnAdc <= 420 ) return BtnBack;
  return BtnNone;
}

void PrintDateSerial()
{
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.date(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(' ');
  Serial.print(DiaSemana[now.dayOfWeek()]);
  Serial.println();
}

void pantalla_principal()
{
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.date(), DEC);
  lcd.setCursor(0, 1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  lcd.print(' ');
  lcd.print(DiaSemana[now.dayOfWeek()]);
  delay(250); 
  lcd.clear();
}