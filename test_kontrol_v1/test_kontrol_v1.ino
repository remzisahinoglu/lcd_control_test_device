/*
prg adi: test_kontrol_v1.ino
tarih: 01.12.2014
yazan: Remzi Şahinoğlu

bu program içinde  4 adet test şartnamesini içinde barındırır.
bunların ilk  ikisi mercedesin  servis-imdat  ve  imdat-servis
testidir.  üçüncüsü klasik ömür testi ve sonuncusu scania için
oluşturulmuştur.  LCD ekrandan test seçimleri periyod süreleri
ve istenen çevrim adetleri butonlarile ayarlanıp start verilir
her çevrimde eeproma o  anki bilgi  yazılır.  bu sayede enerji
kesilse bile mikrodenetleyici açıldığında ilk  olarak eepramda
yazan çevrimi okuyarak oradan devam eder.

sistemde;

- 4x20 karakter LCD ekran
- 2 adet röle
- 4 adet buton kullanılmıştır

*/

#include <LiquidCrystal.h>
#include <EEPROM.h>

int servis_sn = 2000;       // milisaniye
int imdat_sn = 2000;        // imdat açık kalma süresi
int periyod_sn = 4000;      // ömür testi için bir döngünün süresi
//long cycle = 0;           // döngü sayısı ekranda görünen
long cycle_check = 50000;   // istenile cycle değerinde durmak için girilen değer  

String test_name = "Servis-Imdat Testi";

int satir = 0;  // satır lcd ekran
int x = 0;      // satır menu ekranları
int y = 0;      // sutun menu ekranları

const int start_stopButon = 10;
const int highButon = 13;
const int lowButon = 12;
const int secButon = 11;

const int servis_valf = 9;  // servis valfini kontrol eden rölenin bağlı olduğu pin
const int imdat_valf = 8;   // imdat valfini kontrol eden rölenin bağlı olduğu pin

//int backlight = 13; // arduino pin13 
/* connections
RS(LCD pin4) to arduino pin 2
RW(LCD pin5) to ground for write mode
eneable(LCD pin6) to arduino pin 3
LCD pin 15 to arduino pin (arka ekran ışığını kontrol etmek için)
LCD data pins d4,d5,d6,d7 to arduino pins 4,5,6,7
*/
LiquidCrystal LCD(2,3,4,5,6,7);

signed long cycle=0;

void ee_write_long(unsigned int addr , signed long *longptr)
{
unsigned char edata;
unsigned char I;
    for(I=0;I<4;I++){
      edata = *((unsigned char *)longptr+I);
      EEPROM.write(addr+I,edata);
   }
}

void ee_read_long(unsigned int addr , signed long *longptr)
{
unsigned char edata;
unsigned char I;
   for(I=0;I<4;I++){
      edata=EEPROM.read(I+addr);   
        *((unsigned char *)longptr+I) = edata;
   }
}

void setup()
{
  pinMode(servis_valf, OUTPUT);    // çıkış olarak ayarla
  pinMode(imdat_valf, OUTPUT);
  pinMode(highButon, INPUT);    // giriş olarak ayarla
  pinMode(lowButon, INPUT);
  pinMode(secButon, INPUT);
  pinMode(start_stopButon, INPUT);
  
  
  digitalWrite(servis_valf, LOW);    
  digitalWrite(imdat_valf, LOW);
  
  LCD.begin(20,4);  // 20 sutun 4 satır ekran kullanılıyor
  delay(1000);
  LCD.setCursor(6,1);
  LCD.print("ARFESAN");
  LCD.setCursor(2,2);
  LCD.print("AR-GE DEPARTMANI");
  delay(1000);
  
  ee_read_long(0x00, &cycle);    // epromdaki cycle değerini oku
  ee_read_long(0xA0, &cycle_check);
  lcd_yaz();
}
//******************* LCD Ekranların menülerinin bulunduğu kısım ************//
void lcd_yaz()
{
    LCD.clear();
    LCD.setCursor(0,satir);
    LCD.print(">");   // menü giriş oku
      switch(y)    // ekran sutunları
      {
        case 0:
          switch(x)
          {
            case 0:
              LCD.setCursor(1,0);    // birinci satır
              LCD.print(test_name);      
              LCD.setCursor(1,1);    // ikinci satır
              LCD.print("Servis Sn= "); LCD.print(servis_sn); LCD.print("ms");
              LCD.setCursor(1,2);    // üçüncü satır
              LCD.print("Imdat Sn = "); LCD.print(imdat_sn); LCD.print("ms");
              LCD.setCursor(1,3);    // üçüncü satır
              LCD.print("Cycle    = "); LCD.print(cycle);
            break;
              
            case 1:
              LCD.setCursor(1,0);    // birinci satır
              LCD.print(test_name);      
              LCD.setCursor(1,1);    // ikinci satır
              LCD.print("Servis Sn= "); LCD.print(servis_sn); LCD.print("ms");
              LCD.setCursor(1,2);    // üçüncü satır
              LCD.print("Peryd Sn = "); LCD.print(periyod_sn); LCD.print("ms");
              LCD.setCursor(1,3);    // üçüncü satır
              LCD.print("Cycle    = "); LCD.print(cycle);
            break;
          }
        break;
        
        case 1:      // 1.sutun ekranlarını göster
          switch(x)      // satır numarası
          {
            case 0:      // 1.sutun 1.ekran = test seçim ekranı
              LCD.setCursor(1,0);    // birinci satır
              LCD.print("Servis-Imdat Testi");      
              LCD.setCursor(1,1);    // ikinci satır
              LCD.print("Imdat-Servis Testi");      
              LCD.setCursor(1,2);    // üçüncü satır
              LCD.print("Omur Testi        "); 
              LCD.setCursor(1,3);    // üçüncü satır
              LCD.print("SCANIA Testi      "); break;    
        
            case 1:      // 1.sutun 2.ekran = servis süresi seçim ekranı
              LCD.setCursor(1,0);    // birinci satır
              LCD.print("Servis Sn= "); LCD.print(servis_sn); break;
                               
            case 2:      // 1.sutun 3.ekran = imdat süresi seçim ekranı
              LCD.setCursor(1,0);    // birinci satır
              LCD.print("Imdat Sn = "); LCD.print(imdat_sn); break;
              
            case 3:      // 1.sutun 4.ekran = periyod süresi seçim ekranı
              LCD.setCursor(1,0);    // birinci satır
              LCD.print("Peryd Sn = "); LCD.print(periyod_sn); break; 
   
            case 4:      // 1.sutun 5.ekran = cycle ömür değeri
              LCD.setCursor(1,0);
              LCD.print("Cycle = "); LCD.print(cycle_check); break;        
            }                
      }
}

//************************** sadece cycle değeri değiştiğinde yaz **************************//
void lcd_cycle_yaz()
{
  LCD.setCursor(12,3);    // üçüncü satır
  LCD.print(cycle); 
}


void loop()  // ana programı kısır döngüye sok
{
//**********************************// testi başlat //**************************************//
  if(digitalRead(start_stopButon) == 0)
  {// testi başlat
    if(cycle < cycle_check)
    {
      if(test_name == "Servis-Imdat Testi")    // SERVİS-İMDAT TESTİ
      {
        if(cycle < cycle_check)
        {
          digitalWrite(servis_valf, LOW);
          digitalWrite(imdat_valf, HIGH);
          delay(2000);
          digitalWrite(servis_valf, HIGH);
          digitalWrite(imdat_valf, HIGH);
          delay(servis_sn);
          digitalWrite(servis_valf, HIGH);
          digitalWrite(imdat_valf, LOW);
          delay(imdat_sn);
          digitalWrite(servis_valf, LOW);
          digitalWrite(imdat_valf, HIGH);
          
          cycle = cycle + 1;
        }
        
      }
      if(test_name == "Imdat-Servis Testi")  // İMDAT-SERVİS TESTİ
      {
        if(cycle < cycle_check)
        {
          digitalWrite(imdat_valf, LOW);
          digitalWrite(servis_valf, HIGH);
          delay(2000);
          digitalWrite(imdat_valf, HIGH);
          digitalWrite(servis_valf, HIGH);
          delay(servis_sn);
          digitalWrite(imdat_valf, HIGH);
          digitalWrite(servis_valf, LOW);
          delay(imdat_sn);
          digitalWrite(imdat_valf, LOW);
          digitalWrite(servis_valf, HIGH);
          
          cycle = cycle + 1;
        }
      }
      if(test_name == "Omur Testi        ")        // ÖMÜR TESTİ
      {
        digitalWrite(servis_valf, LOW);
        digitalWrite(imdat_valf, HIGH);
        delay(periyod_sn - servis_sn);
        digitalWrite(servis_valf, HIGH);
        delay(servis_sn);
        
        cycle = cycle + 1;
      }
      if(test_name == "SCANIA Testi      ")            // SCANIA testi
      {
        for(int d = 0; d < 6; d++)
        {
          digitalWrite(servis_valf, HIGH);
          delay(servis_sn/2);
          digitalWrite(servis_valf, LOW);
          delay(servis_sn/2);
              
          cycle = cycle + 1;
          lcd_cycle_yaz();
        }
        
        digitalWrite(imdat_valf, HIGH);
        delay(imdat_sn/2);
        digitalWrite(imdat_valf, LOW);
        delay(imdat_sn/2);
              
        cycle = cycle + 1;
        
      }
      ee_write_long(0x00, &cycle);    // cycle değerini eproma yaz
      lcd_ckeck_yaz();
    }
    else
    {
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print(test_name);
      LCD.setCursor(0,1);
      LCD.print("Test Bitti");
      LCD.setCursor(0,3);
      LCD.print("cycle = "); LCD.print(cycle);
      delay(6000);
    }
  }
  else
  {// testi durdur/hazırla
  //******************************// seç butonu menu içinegir //******************************//
      digitalWrite(servis_valf, LOW);
      digitalWrite(imdat_valf, LOW);
      
      if(digitalRead(secButon) == 0)
      {
        switch(y)    // sutun ekranları
        {
          case 0:     // ilk ana ekran
            switch(x)    // satır ekranları
            {
              case 0:   // test seçim ekranı
                switch(satir)    // ekran satırları
                {
                  case 0: y=1; x=0; satir=0; break;
                  case 1: y=1; x=1; satir=0; break;
                  case 2: y=1; x=2; satir=0; break;
                  case 3: y=1; x=4; satir=0; break;
                }
              break;
              
              case 1:
                switch(satir)    // ömür testi periyod saniyesi
                {
                  case 0: y=1; x=0; satir=0; break;
                  case 1: y=1; x=1; satir=0; break;
                  case 2: y=1; x=3; satir=0; break;
                  case 3: y=1; x=4; satir=0; break;
                } 
              break;
  
            }
          break;
          
          case 1:   // test seçim ekranı
            switch(x) 
            {
              case 0: 
                switch(satir)    
                {
                  case 0: y=0; x=0; satir=0; test_name = "Servis-Imdat Testi";
                          servis_sn = 2000; imdat_sn = 2000; break;
                  case 1: y=0; x=0; satir=0; test_name = "Imdat-Servis Testi";
                          servis_sn = 2000; imdat_sn = 2000; break;
                  case 2: y=0; x=1; satir=0; test_name = "Omur Testi        ";
                          servis_sn = 2000; imdat_sn = 2000; break;
                  case 3: y=0; x=0; satir=0; test_name = "SCANIA Testi      "; 
                          servis_sn = 6000; imdat_sn = 6000; break;
                }
              break;
              
              case 1:  // servis sn ekranı
                y=0; x=0; satir=0;    // ana ekrana dön
              break;
                
              case 2:  // imdat sn ekranı
                y=0; x=0; satir=0;
              break;
              
              case 3:  // periyod sn ekranı
                y=0; x=1; satir=0;
              break;
              
              case 4:  // cycle seçin ekranı
                cycle=0;  // yeni teste başlamak için eski durumdaki cycle değeri sıfırlanmalıdır
                y=0; 
                
                ee_write_long(0xA0, &cycle_check);
                
                if(test_name == "Omur Testi        "){x=1;}
                else{x=0;} 
                satir=0;
            }
          break;
        }
   
        lcd_yaz();  
        delay(200);
      }
  }
//**************************// satır yukarı yada değer arttır //****************************//
    if(digitalRead(highButon) == 0)
    {      
      satir = satir + 1;
      if(satir > 3){satir = 0;}       // lcd cursoru (>) satır yukarı çıkart
      
      if(y==1)        // servis_sn süresini arttır
      {
        if(x==1)
        {
          servis_sn = servis_sn + 100;
          satir = 0;
        }
      }
      
      if(y==1)        //  imdat_sn süresini arttır
      {
        if(x==2)
        {
          imdat_sn = imdat_sn + 100;
          satir=0;
        }
      }
      
      if(y==1)        //  periyod_sn süresini arttır
      {
        if(x==3)
        {
          periyod_sn = periyod_sn + 100;
          satir=0;
        }
      }
      
      if(y==1)        //  cycle sayısını arttır
      {
        if(x==4)
        {
          cycle_check = cycle_check + 1000;
          satir=0;
        }
      }
      
      lcd_yaz();      
      delay(200);  
    }
//**************************// satır aşağı yada değer azalt //******************************//    
    if(digitalRead(lowButon) == 0)   
    {
      satir = satir - 1; 
      if(satir < 0){satir = 3;}       // lcd cursoru (>) satır aşağı indir
            
      if(y==1)      // servis_sn süresini azalt
      {
        if(x==1)
        {
          servis_sn = servis_sn - 100;
          if(servis_sn < 0){servis_sn=0;}
          satir=0;
        }
      }
      
      if(y==1)      // imdat_sn süresini azalt
      {
        if(x==2)
        {
          imdat_sn = imdat_sn - 100;
          if(imdat_sn < 0){imdat_sn=0;}
          satir=0;
        }
      }
      
      if(y==1)        //  periyod_sn süresini arttır
      {
        if(x==3)
        {
          periyod_sn = periyod_sn - 100;
          if(periyod_sn < 0){periyod_sn=0;}
          satir=0;
        }
      }
      
      if(y==1)        //  cycle sayısını arttır
      {
        if(x==4)
        {
          cycle_check = cycle_check - 1000;
          if(cycle_check < 0){cycle_check=0;}
          satir=0;
        }
      }
      
      lcd_yaz();
      delay(200);
    }
}










