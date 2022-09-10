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
#define enter 0
#define updown 1
uint8_t rs = 13, rw=12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);
char DiaSemana[][4] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab" };
uint8_t Salida[NumSalidas] ={6, 7, 8}; //configurar pines digitales de acuerdo al numero de salidas si se modifica num salidas el vector se modifica de acuerdo a los pines de salida
short BtnAdc = 0;
bool push_enter=0, push_back=0, push_down=0, push_up=0;
int8_t timer_select=0, setup_menu=0, item_val=0, setup_item=0, item_menu=0, item_modify=0, lastsecond=0;                         
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
  Wire.begin();
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
Temporizador_por_Salida2();
btnstate=readBtns();
delay(150);

if (btnstate==BtnEnter){push_enter=1;}
if (btnstate==BtnBack){push_back=1;}
if (btnstate==BtnDown){push_down=1;}
if (btnstate==BtnUp){push_up=1;}


if ((btnstate == BtnNone && push_enter==1) || setup_menu!=0)
{
    if (setup_menu==0){push_enter=0;}  
    setup_menu=1;
    if (setup_item==0){item_menu=val_increment(updown, item_menu, 4);
    if (btnstate==BtnNone && push_back==1){setup_menu=0; push_back=0; lcd.clear();}}
    switch (item_menu)
    {
    case 0: //SELECCIONE UN TIMER A CONF ##############################################################3
      if (setup_item==0)
      {
          lcd.setCursor(0, 0);
          lcd.print("CONFIG TIMER ");
          lcd.print(timer_select);
          lcd.setCursor(0, 1);
          lcd.print("SELECT TIMER");
          //delay(150);
          //lcd.clear(); 
      }
      if ( (btnstate==BtnNone && push_enter==1) || setup_item!=0)
      {
        if(setup_item==0){push_enter=0; lcd.clear(); }
        setup_item=1;
        item_modify=val_increment(enter, item_modify, 3);
        if (item_modify==0){timer_select=val_increment(updown, timer_select, NumTimer);item_blink(2,1);}
        if (item_modify==2){item_val=NumSalidas;item_blink(7,1);}
        if (item_modify==1){item_val=2;item_blink(12,1);} 
        if (item_modify!=0){Timer1[timer_select][item_modify+2]=val_increment(updown, Timer1[timer_select][item_modify+2], item_val);} 
        lcd.setCursor(0, 0);
        lcd.print("TIMER|OUT|ON/OFF");
        lcd.setCursor(2, 1);
        lcd.print(timer_select);
        lcd.setCursor(5, 1);
        lcd.print("|");
        lcd.setCursor(7, 1);
        lcd.print(Timer1[timer_select][4]);
        lcd.setCursor(9, 1);
        lcd.print("|");
        //lcd.print(" | ");
        if ( Timer1[timer_select][3]==1){lcd.setCursor(12, 1);lcd.print("ON");}
        if ( Timer1[timer_select][3]==0){lcd.setCursor(12, 1);lcd.print("OFF");}
        if (btnstate==BtnNone && push_back==1){setup_item=0; item_val=0; item_modify=0; push_back=0;lcd.clear();}
      }
      break;

    case 1: //SELECCIONE DIAS ################################################################
      if (setup_item == 0)
      {
        lcd.setCursor(0, 0);
        lcd.print("CONFIG TIMER ");
        lcd.print(timer_select);
        lcd.setCursor(0, 1);
        lcd.print("SELECIONAR DIAS");
      }
      if ( (btnstate==BtnNone && push_enter==1 )|| setup_item!=0)
      {
        //delay(200);
        if(setup_item==0){push_enter=0;lcd.clear();}
        setup_item=1;
        item_modify=val_increment(enter, item_modify, 7);
        DATE[timer_select][item_modify]=val_increment(updown, DATE[timer_select][item_modify], 2);
        lcd.setCursor(0, 0);
        lcd.print(" L M M J V S D");
        lcd.setCursor(0, 1);
		lcd.print("|");
        for (int8_t i=0; i<7; i++)
        {
          if (DATE[timer_select][i]!=0)
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
        item_blink((((item_modify+1)*2)-1),1);
        if (btnstate==BtnNone && push_back==1){ setup_item=0; item_modify=0;push_back=0;lcd.clear();}
      }
      break;

    case 2: //SELECCIONE HORA #####################################################################
      if (setup_item==0)
      {
        lcd.setCursor(0, 0);
        lcd.print("CONFIG TIMER ");
        lcd.print(timer_select);
        lcd.setCursor(0, 1);
        lcd.print("SELECCIONE HORA");
      }
      if ((btnstate==BtnNone && push_enter == 1) || setup_item!=0)
      {
        if(setup_item==0){push_enter=0;lcd.clear();}
        setup_item=1;
        item_modify=val_increment(enter, item_modify, 3);
        if (item_modify==0){item_val=24;item_blink(7,1);}
        if (item_modify==1){item_val=60;item_blink(11,1);}
        if (item_modify==2){item_val=60;item_blink(14,1);}
        Timer1[timer_select][item_modify]=val_increment(updown, Timer1[timer_select][item_modify], item_val);
        lcd.setCursor(0, 0);
        lcd.print("TIMER");
        lcd.print(timer_select);
        lcd.print(" H | M | S");
        lcd.setCursor(7, 1);
        lcd.print(Timer1[timer_select][0]);
        lcd.setCursor(9, 1);
        lcd.print(":");
        lcd.setCursor(11, 1);
        lcd.print(Timer1[timer_select][1]);
        lcd.setCursor(13, 1);
        lcd.print(":");
        lcd.print(Timer1[timer_select][2]);
        if (btnstate==BtnNone && push_back==1){setup_item=0;item_modify=0;push_back=0;lcd.clear();}
      }
      break;

    case 3: //SALIR#########################################################################################
      lcd.setCursor(0, 0);
      lcd.print("CONFIG TIMER ");
	    lcd.print(timer_select);
      lcd.setCursor(0, 1);;
      lcd.print("SALIR");
      if (btnstate==BtnNone && push_enter==1){push_enter=0;setup_menu=0; lcd.clear();}
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
                    //PORTD=(Timer1[i][3]<<PORT6);
                    }
                } 
          }

        }
    }
}

int8_t val_increment(bool Btn, int8_t item, int8_t num_item)
{
if(Btn==0)
{
  if (btnstate==BtnNone && push_enter==1) 
    {
      item++;
      if ( item == num_item)
      {
        item = 0;
      }
      push_enter=0;
      lcd.clear();
    }
    return item; 
}
else
  if(btnstate==BtnNone && push_up==1) 
    {
      item++;
      if ( item == num_item)
      {
        item = 0;
      }
      push_up=0;
      lcd.clear();
    }
  if (btnstate==BtnNone && push_down ==1) 
    {
      item--;
      if ( item == -1 )
      {
        item = num_item-1;
      }
      push_down=0;
      lcd.clear();
    }
    return item; 
}

int8_t readBtns()
{
  BtnAdc=analogRead(1);
  if ( BtnAdc >= 1019) return BtnEnter;
  if ( BtnAdc >= 800 && BtnAdc <= 850 ) return BtnUp;  
  if ( BtnAdc >= 600 && BtnAdc <= 630 ) return BtnDown;  
  if ( BtnAdc >= 390 && BtnAdc <= 420 ) return BtnBack;
  return BtnNone;
}

void pantalla_principal()
{
  
  DateTime now = rtc.now();
  if(now.second()!=lastsecond)
  {
  lcd.clear();
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
  }
  lastsecond=now.second();
  
  //delay(250); 
  //lcd.clear();
}
void item_blink(uint8_t col, uint8_t row)
{
  lcd.setCursor(col, row);
  lcd.print(" ");
  delay(100);
}