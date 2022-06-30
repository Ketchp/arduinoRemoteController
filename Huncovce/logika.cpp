#include <Arduino.h>
#include "functions.h"
#include "logika.h"
#include "meranie.h"
#include <EEPROM.h>

void logika(){
  tg1Led = false;
  tg2Led = false;
  if(!sietovaOchrana){ //sietova ochrana blikanie
    tg1Led = (millis() >> 10) & 1;
    tg2Led = !((millis() >> 10) & 1);
  }
  else if(tg1 == 1 || tg2 == 1){ //signalizacia tg
    if(tg1 == 1) tg1Led = true;
    if(tg2 == 1) tg2Led = true;
  }
  if(dialkove_ovladanie){
    if((unsigned long)(millis() - cistenie_zaciatok) > 1000ul*cistenie_t && !auto_cistenie && !manual_cistenie) cistenie = 1;
    if((unsigned long)(millis() - stavidlo_zaciatok) > 1000ul*stavidla_ovladanie_t) stavidla_ovladanie = 1;
    if(stavidla_poloha == stavidla_ovladanie) stavidla_ovladanie = 1;

    if(tg1 == 0 && (unsigned long)(millis() - tg1_moveStart) > 300000ul){
      tg1_cistenie = false;
      tg1_phase = 0;
      tg1_request = 2;
    }

    if(tg2 == 0 && (unsigned long)(millis() - tg2_moveStart) > 300000ul){
      tg2_cistenie = false;
      tg2_phase = 0;
      tg2_request = 2;
    }

  }
  if(!manualne_ovladanie){
    if(((unsigned long)(millis() - cistenie_zaciatok) > 60000ul) && auto_cistenie){
      cistenie_otocka = true;
    }

    if(posledne_cistenie + (long)T_cistenie*60 < realtime && T_cistenie && cistenieActive && cistenie == 1){
      cistenie = 2;
      auto_cistenie = true;
      cistenie_zaciatok = millis();
    }

    if(perioda_cistenie_tg){
      if(tg1_last_clean + perioda_cistenie_tg*60*60 < realtime && !cleaning && tg1_request == 2 && otacky_1 != -1){
        if((unsigned long)(millis()-tg2_moveStart) > 120000ul && (tg2_request == 2 || tg2_request == 1)){
          tg1_cistenie = true;
          cleaning = true;
          tg1_riadenie_enabled = false;
        }
      }
      if(tg2_last_clean + perioda_cistenie_tg*60*60 < realtime && !cleaning && tg2_request == 2 && otacky_2 != -1){
        if((unsigned long)(millis()-tg1_moveStart) > 120000ul && (tg1_request == 2 || tg1_request == 1)){
          tg2_cistenie = true;
          cleaning = true;
          tg2_riadenie_enabled = false;
        }
      }
    }
    if(vykon1 < min_kW1 && tg1 == 2 && !tg1_priebezne){
      if((unsigned long)(millis()-kW1_pod) > 60000ul && (unsigned long)(millis()-tg1_moveStart) > 120000ul){
        tg1_request = 1;
      }
    }
    else kW1_pod = millis();

    if(vykon2 < min_kW2 && tg2 == 2 && !tg2_priebezne){
      if((unsigned long)(millis()-kW2_pod) > 60000ul && (unsigned long)(millis()-tg2_moveStart) > 120000ul){
        tg2_request = 1;
      }
    }
    else kW2_pod = millis();
  }
  if(cistenie_otocka){  //aktualizacia posledneho cistenia
    posledne_cistenie = realtime;
    EEPROM.put(posledne_cistenie_eeAdd, posledne_cistenie);
    cistenie_otocka = false;
    if(!manualne_ovladanie && auto_cistenie){
      cistenie = 1;
      auto_cistenie = false;
    }
  }
  if(cistenie != 1 && !cistenieActive){
    cistenie = 1;
    auto_cistenie = false;
  }


  if(tg1_cistenie && !manualne_ovladanie){
    if(tg1_phase == 0){
      tg1 = 1;
      tg1_ventil = 0;
      tg1_kompenzacia = 0;
      tg1_t = millis();
      tg1_phase++;
    }
/*    else if((tg1_phase == 2 || tg1_phase == 4) && tg1_kompenzacia == 0 &&
     (unsigned long)(millis() - tg1_t) > 1000ul && (unsigned long)(millis() - tg1_t) < 14000ul){
      tg1_kompenzacia = 1;
    }
    else if((tg1_phase == 2 || tg1_phase == 4) && tg1_kompenzacia == 1 && (unsigned long)(millis() - tg1_t) > 14000ul){
      tg1_kompenzacia = 0;
    }*/
    else if((unsigned long)(millis() - tg1_t) > 15000ul){
      if(tg1_phase == 2 || tg1_phase == 4){
        tg1 = 1;
        tg1_ventil = 0;
        tg1_kompenzacia = 0;
        tg1_t = millis();
        tg1_phase++;
      }
      else if(!otacky_1){
        if(tg1_phase == 1 || tg1_phase == 3){
          tg1 = 0;
          tg1_moveStart = millis();
          tg1_ventil = 0;
          tg1_kompenzacia = 0;
          tg1_t = millis();
          tg1_phase++;
        }
        else if(tg1_phase == 5){
          tg1_request = 2;
          tg1_phase = 0;
          tg1_cistenie = false;
          tg1_last_clean = realtime;
          EEPROM.put(tg1_last_clean_eeAdd, tg1_last_clean);
        }
      }
    }
  }
  else if(tg1_priebezne && dialkove_ovladanie){
    if(tg1_phase == 0){
      tg1 = 1;
      tg1_ventil = 1;
      tg1_kompenzacia = 0;
      tg1_t = millis();
      tg1_phase++;
    }
    else if((unsigned long)(millis() - tg1_t) > 15000ul){
      if(tg1_phase == 1){
        tg1 = 1;
        tg1_ventil = 0;
        tg1_kompenzacia = 0;
        tg1_t = millis();
        tg1_phase++;
      }
      else if(!otacky_1){
        if(tg1_phase == 2){
          tg1_request = 2;
          tg1_phase = 0;
          tg1_priebezne = false;
        }
      }
    }    
  }
  else if(tg1_request != tg1){
    if(manualne_ovladanie){
      if(tg1 != 1) tg1 = 1;
      else if(!otacky_1){
        tg1 = tg1_request;
        tg1_moveStart = millis();
      }
    }
    else{
      if(tg1 != 1){
        tg1 = 1;
        tg1_ventil = 0;
        tg1_kompenzacia = 0;
        kompDialkove = false; //todo kompDialkove na tg1 a tg2 oddelene a poriesit kde kompDialkove naspat false
      }
      else if(!otacky_1){
        tg1 = tg1_request;
        tg1_ventil = 1;
        tg1_moveStart = millis();
      }
    }
  }
  else if(!manualne_ovladanie && !tg1_kompenzacia && tg1 != 1 && !kompDialkove){
    if((unsigned long)(millis()-tg1_moveStart) > 1000ul){
      tg1_kompenzacia = 1;
    }
  }

  if(tg2_cistenie && !manualne_ovladanie){
    if(tg2_phase == 0){
      tg2 = 1;
      tg2_ventil = 0;
      tg2_kompenzacia = 0;
      tg2_t = millis();
      tg2_phase++;
    }
/*    else if((tg2_phase == 2 || tg2_phase == 4) && tg2_kompenzacia == 0 &&
     (unsigned long)(millis() - tg2_t) > 1000ul && (unsigned long)(millis() - tg2_t) < 14000ul){
      tg2_kompenzacia = 1;
    }
    else if((tg2_phase == 2 || tg2_phase == 4) && tg2_kompenzacia == 1 && (unsigned long)(millis() - tg2_t) > 14000ul){
      tg2_kompenzacia = 0;
    }*/
    else if((unsigned long)(millis() - tg2_t) > 15000ul){
      if(tg2_phase == 2 || tg2_phase == 4){
        tg2 = 1;
        tg2_ventil = 0;
        tg2_kompenzacia = 0;
        tg2_t = millis();
        tg2_phase++;
      }
      else if(!otacky_2){
        if(tg2_phase == 1 || tg2_phase == 3){
          tg2 = 0;
          tg2_moveStart = millis();
          tg2_ventil = 0;
          tg2_kompenzacia = 0;
          tg2_t = millis();
          tg2_phase++;
        }
        else if(tg2_phase == 5){
          tg2_request = 2;
          tg2_phase = 0;
          tg2_cistenie = false;
          tg2_last_clean = realtime;
          EEPROM.put(tg2_last_clean_eeAdd, tg2_last_clean);
        }
      }
    }
  }
  else if(tg2_priebezne && dialkove_ovladanie){
    if(tg2_phase == 0){
      tg2 = 1;
      tg2_ventil = 1;
      tg2_kompenzacia = 0;
      tg2_t = millis();
      tg2_phase++;
    }
    else if((unsigned long)(millis() - tg2_t) > 15000ul){
      if(tg2_phase == 1){
        tg2 = 1;
        tg2_ventil = 0;
        tg2_kompenzacia = 0;
        tg2_t = millis();
        tg2_phase++;
      }
      else if(!otacky_2){
        if(tg2_phase == 2){
          tg2_request = 2;
          tg2_phase = 0;
          tg2_priebezne = false;
        }
      }
    }    
  }
  else if(tg2_request != tg2){
    if(manualne_ovladanie){
      if(tg2 != 1) tg2 = 1;
      else if(!otacky_2){
        tg2 = tg2_request;
        tg2_moveStart = millis();
      }
    }
    else{
      if(tg2 != 1){
        tg2 = 1;
        tg2_ventil = 0;
        tg2_kompenzacia = 0;
        kompDialkove = false;
      }
      else if(!otacky_2){
        tg2 = tg2_request;
        tg2_ventil = 1;
        tg2_moveStart = millis();
      }
    }
  }
  else if(!manualne_ovladanie && !tg2_kompenzacia && tg2 != 1 && !kompDialkove){
    if((unsigned long)(millis()-tg2_moveStart) > 1000ul){
      tg2_kompenzacia = 1;
    }
  }
}
