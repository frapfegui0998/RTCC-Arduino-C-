/**************************************
 *                                     *
 * Universidad Fidelitas               *
 * Carrera: Sistemas de Computación    *
 * Curso: Paradigmas de Programación   *
 * Archivo: CasoEstudio2.ino           *
 * Fecha: 14/08/24                     *
 * Autor: Grupo 07                     *
 *                                     *
 ***************************************/
// Bibliotecas del Sistema
#include <TimeLib.h>
#include <LiquidCrystal.h>

// Definición de Constantes
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int diasMaximos[] = {
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// NECSARIOS PARA LA ANIMACION DE CARGA:
byte p1[8] = {
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10
};

byte p2[8] = {
  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18
};

byte p3[8] = {
  0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C
};

byte p4[8] = {
  0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E
};

byte p5[8] = {
  0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F
};

// VARIABLES


//Variable Reloj
int segundos = 0;
int minutos = 0;
int horas = 0;
int dia = 1;
int mes = 8;
int anho = 2024;

//Variables Alarma
int segundosAlarma = 0;
int minutosAlarma = 0;
int horasAlarma = 0;
int diaAlarma = 1;
int mesAlarma = 8;
int anhoAlarma = 2024;
int tono = 0;
int numAlarma = 0;
bool alarmaActivada = false;  //variable para evitar que la alarma se active más de una vez en el mismo minuto

int estadoParpadeo = LOW;
unsigned long temporizador1;
int posicionXActual;
int posicionYActual;
int configuracion = 6;
bool formato24Horas = true; // Variable para alternar entre 24 horas y 12 horas
bool vistaReloj = true;
const int LED_TONO1 = 13;
const int LED_TONO2 = 10;
const int LED_TONO3 = 9;
const int LED_TONO4 = 8;
const int LED_TONO5 = 7;

// Clase Alarma
struct Alarma {
    int hora;
    int minuto;
    int dia;
    int mes;
    int anho;
};

#define MAX_ALARMAS 10
Alarma alarmas[MAX_ALARMAS];


void setup() {
  // Configurar el número de columnas y filas del LCD:
  pinMode(A0, INPUT); // botón de configuración
  pinMode(A1, INPUT); // botón de subir
  pinMode(A2, INPUT); // botón de bajar
  pinMode(A3, INPUT); // botón para salir del modo de configuración
  pinMode(A4, INPUT); // botón para vista Alarma y moverse entre Alarmas
  pinMode(A5, INPUT); // botón para cambiar formato 12/24 horas

  pinMode(LED_TONO1, OUTPUT); // LED tono 1 para la alarma
  pinMode(LED_TONO2, OUTPUT); // LED tono 2 para la alarma
  pinMode(LED_TONO3, OUTPUT); // LED tono 3 para la alarma
  pinMode(LED_TONO4, OUTPUT); // LED tono 4 para la alarma
  pinMode(LED_TONO5, OUTPUT); // LED tono 5 para la alarma

  // NECESARIOS PARA LA ANIMACION DE CARGA:
  lcd.createChar(1, p1);
  lcd.createChar(2, p2);
  lcd.createChar(3, p3);
  lcd.createChar(4, p4);
  lcd.createChar(5, p5);
  lcd.begin(16, 4);
  cargarAnimacion();

  setTime(horas, minutos, segundos, dia, mes, anho);
}

void loop() {
  unsigned long milisegundosActuales = millis();
  int botonArriba = digitalRead(A0);
  int botonAbajo = digitalRead(A1);
  int botonConfigurar = digitalRead(A2);
  int botonFormato = digitalRead(A5);
  int botonSalir = digitalRead(A3);
  int botonAlarma = digitalRead(A4);
  int botonTono = digitalRead(0);

  segundos = second();
  minutos = minute();
  horas = hour();
  dia = day();
  mes = month();
  anho = year();


  ///////////////////////////////BOTONES//////////////////////////////////////////
  // Botón Cambio de Tono
  if (botonTono == HIGH) {
    lcd.clear();
    apagarLED();
    tono++;
    if (tono == 6) {
      tono = 1;
    }
    lcd.setCursor(0, 0);
    lcd.print("       TONO: " + String(tono));
    lcd.setCursor(0, 1);
    lcd.print("              ");
    delay(350);
    lcd.setCursor(0, 0);
    lcd.print("              ");
    lcd.setCursor(0, 1);
    lcd.print("              ");
    encenderLED();
  }

  // Botón cambiar formato 12 o 24 horas
  if (botonFormato == HIGH) {
    formato24Horas = !formato24Horas;
    delay(250);
  }

  // Botón configurar valores reloj de los espacios del 0 al 5
  if (botonConfigurar == HIGH) {
    configuracion ++;
    if (configuracion > 5) {
      configuracion = 0;
    }
    if (!vistaReloj && configuracion > 4) {
      configuracion = 0;
    }
    delay(350);
  }

  // Botón para entrar al modo Alarma y cambiar entre números de alarma
  if (botonAlarma == HIGH) {
    lcd.clear();
    numAlarma++;
    vistaReloj = false;
    if (numAlarma >= MAX_ALARMAS) {
      numAlarma = 0;
    }
    delay(350);
  }

  // Botón guardar o salir
  if (botonSalir == HIGH) {
    if (vistaReloj) {
      apagarLED();
      setTime(horas, minutos, segundos, dia, mes, anho); // Guardar los cambios
      configuracion = 6; // Salir del modo de configuración en el espacio 6 se sale de la configuración
      delay(350); // Retraso para evitar múltiples lecturas
    } else {
      agregarAlarma(numAlarma, horasAlarma, minutosAlarma, diaAlarma, mesAlarma, anhoAlarma);
      numAlarma = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarma ");
      lcd.setCursor(0, 1);
      lcd.print("Guardada!");
      delay(350);
      lcd.clear();
      configuracion = 6;
      vistaReloj = true;
      delay(350);
    }
  }
  
  //manejo del parpadeo para el modo configuracion
  if (milisegundosActuales - temporizador1 > 300) {
    temporizador1 = milisegundosActuales;
    estadoParpadeo = !estadoParpadeo;
  }

  ///////////////////////////////BOTONES//////////////////////////////////////////
  
  // Comprobar alarmas solo si no ha sido activada en el minuto actual, ya que la alarma no tiene un segundo especifico para sonar, entonces asi no se vuelve activar si ya se apagó
  if (!alarmaActivada && comprobarAlarma(horas, minutos, dia, mes, anho)) {
    encenderLED();
    alarmaActivada = true;  // Marcar la alarma como activada para este minuto
  }

  // Si el minuto ha cambiado, reiniciamos la bandera de alarma
  if (segundos == 0) {
    alarmaActivada = false;
  }

////////////////////////////////Modo Configuracion Reloj//////////////////////////
  // Configuración en el espacio 0 para cambiar los días
  if (configuracion == 0 && vistaReloj) {
    posicionXActual = 0;
    posicionYActual = 1;
    if (botonArriba == HIGH) {
      dia++;
      delay(350);
      if (dia > diasMaximos[mes]) {
        dia = 1;
      }
    }
    if (botonAbajo == HIGH) {
      dia--;
      delay(350);
      if (dia < 1) {
        dia = diasMaximos[mes];
      }
    }
    setTime(horas, minutos, segundos, dia, mes, anho);
  }
  // Configuración en el espacio 1 para cambiar los meses
  if (configuracion == 1 && vistaReloj) {
    posicionXActual = 3;
    posicionYActual = 1;
    if (botonArriba == HIGH) {
      mes++;
      delay(350);
      if (mes > 12) {
        mes = 1;
      }
    }
    if (botonAbajo == HIGH) {
      mes--;
      delay(350);
      if (mes < 1) {
        mes = 12;
      }
    }
    setTime(horas, minutos, segundos, dia, mes, anho);
  }
  // Configuración en el espacio 2 para cambiar los años
  if (configuracion == 2 && vistaReloj) {
    posicionXActual = 6;
    posicionYActual = 1;
    if (botonArriba == HIGH) {
      anho++;
      delay(350);
      if (anho > 3000) {
        anho = 2000;
      }
    }
    if (botonAbajo == HIGH) {
      anho--;
      delay(350);
      if (anho < 2000) {
        anho = 3000;
      }
    }
    setTime(horas, minutos, segundos, dia, mes, anho);
  }
  // Configuración en el espacio 3 para cambiar las horas
  if (configuracion == 3 && vistaReloj) {
    posicionXActual = 0;
    posicionYActual = 2;
    if (botonArriba == HIGH) {
      horas++;
      delay(350);
      if (horas > 23) {
        horas = 0;
      }
    }
    if (botonAbajo == HIGH) {
      horas--;
      delay(350);
      if (horas < 0) {
        horas = 23;
      }
    }
    setTime(horas, minutos, segundos, dia, mes, anho);
  }
  // Configuración en el espacio 4 para cambiar los minutos
  if (configuracion == 4 && vistaReloj) {
    posicionXActual = 3;
    posicionYActual = 2;
    if (botonArriba == HIGH) {
      minutos++;
      delay(350);
      if (minutos > 59) {
        minutos = 0;
      }
    }
    if (botonAbajo == HIGH) {
      minutos--;
      delay(350);
      if (minutos < 0) {
        minutos = 59;
      }
    }
    setTime(horas, minutos, segundos, dia, mes, anho);
  }
  // Configuración en el espacio 5 para cambiar los segundos
  if (configuracion == 5 && vistaReloj) {
    posicionXActual = 6;
    posicionYActual = 2;
    if (botonArriba == HIGH) {
      segundos++;
      delay(350);
      if (segundos > 59) {
        segundos = 0;
      }
    }
    if (botonAbajo == HIGH) {
      segundos--;
      delay(350);
      if (segundos < 0) {
        segundos = 59;
      }
    }
    setTime(horas, minutos, segundos, dia, mes, anho);
  }

////////////////////////////////Modo Configuracion Alarma//////////////////////////
  // Configuración en el espacio 0 para cambiar los días
  if (configuracion == 0 && !vistaReloj) {
    posicionXActual = 0;
    posicionYActual = 1;
    if (botonArriba == HIGH) {
      diaAlarma++;
      delay(350);
      if (diaAlarma > diasMaximos[mes]) {
        diaAlarma = 1;
      }
    }
    if (botonAbajo == HIGH) {
      diaAlarma--;
      delay(350);
      if (diaAlarma < 1) {
        diaAlarma = diasMaximos[mes];
      }
    }
  }
  // Configuración en el espacio 1 para cambiar los meses
  if (configuracion == 1 && !vistaReloj) {
    posicionXActual = 3;
    posicionYActual = 1;
    if (botonArriba == HIGH) {
      mesAlarma++;
      delay(350);
      if (mesAlarma > 12) {
        mesAlarma = 1;
      }
    }
    if (botonAbajo == HIGH) {
      mesAlarma--;
      delay(350);
      if (mesAlarma < 1) {
        mesAlarma = 12;
      }
    }
  }
  // Configuración en el espacio 2 para cambiar los años
  if (configuracion == 2 && !vistaReloj) {
    posicionXActual = 6;
    posicionYActual = 1;
    if (botonArriba == HIGH) {
      anhoAlarma++;
      delay(350);
      if (anhoAlarma > 3000) {
        anhoAlarma = 2000;
      }
    }
    if (botonAbajo == HIGH) {
      anhoAlarma--;
      delay(350);
      if (anhoAlarma < 2000) {
        anhoAlarma = 3000;
      }
    }
  }
  // Configuración en el espacio 3 para cambiar las horas
  if (configuracion == 3 && !vistaReloj) {
    posicionXActual = 0;
    posicionYActual = 2;
    if (botonArriba == HIGH) {
      horasAlarma++;
      delay(350);
      if (horasAlarma > 23) {
        horasAlarma = 0;
      }
    }
    if (botonAbajo == HIGH) {
      horasAlarma--;
      delay(350);
      if (horasAlarma < 0) {
        horasAlarma = 23;
      }
    }
  }
  // Configuración en el espacio 4 para cambiar los minutos
  if (configuracion == 4 && !vistaReloj) {
    posicionXActual = 3;
    posicionYActual = 2;
    if (botonArriba == HIGH) {
      minutosAlarma++;
      delay(350);
      if (minutosAlarma > 59) {
        minutosAlarma = 0;
      }
    }
    if (botonAbajo == HIGH) {
      minutosAlarma--;
      delay(350);
      if (minutosAlarma < 0) {
        minutosAlarma = 59;
      }
    }
  }
  // Configuración en el espacio 5 para cambiar los segundos
  if (configuracion == 5 && !vistaReloj) {
    posicionXActual = 6;
    posicionYActual = 2;
    if (botonArriba == HIGH) {
      segundos++;
      delay(350);
      if (segundosAlarma > 59) {
        segundosAlarma = 0;
      }
    }
    if (botonAbajo == HIGH) {
      segundosAlarma--;
      delay(350);
      if (segundosAlarma < 0) {
        segundosAlarma = 59;
      }
    }
  }


////////////////////////////////Mostrar Reloj//////////////////////////
  if (vistaReloj) {
    lcd.setCursor(0, 0);
    lcd.print("RTCC Proy.Final");
    // Mostrar reloj y calendario
    if (estadoParpadeo == HIGH) {
      lcd.setCursor(0, 1);
      if (dia < 10) lcd.print('0');
      lcd.print(dia);
      lcd.print("/");
      if (mes < 10) lcd.print('0');
      lcd.print(mes);
      lcd.print("/");
      lcd.print(anho - 2000); // mostrar anho con dos dígitos

      // HORAS
      lcd.setCursor(0, 2);
      if (formato24Horas) {
        // Formato de 24 horas
        if (horas < 10) lcd.print('0');
        lcd.print(horas);
      } else {
        // Formato de 12 horas con A.M./P.M.
        int horas12 = horas % 12;
        if (horas12 == 0) horas12 = 12; // Ajustar para mostrar 12 A.M./P.M. en lugar de 0
        if (horas12 < 10) lcd.print('0');
        lcd.print(horas12);
      }

      // MINUTOS
      lcd.print(":");
      if (minutos < 10) lcd.print('0');
      lcd.print(minutos);

      // SEGUNDOS
      lcd.print(":");
      if (segundos < 10) lcd.print('0');
      lcd.print(segundos);
      if (!formato24Horas) {
        lcd.print(horas < 12 ? " A.M." : " P.M.");
      } else {
        lcd.print("     ");
      }
    }
  } else {
////////////////////////////////Mostrar Alarma//////////////////////////
    lcd.setCursor(0, 0);
    lcd.print(" Alarma " + String(numAlarma) + ": ");
    // Mostrar reloj y calendario de la alarma
    if (estadoParpadeo == HIGH) {
      lcd.setCursor(0, 1);
      if (diaAlarma < 10) lcd.print('0');
      lcd.print(diaAlarma);
      lcd.print("/");
      if (mesAlarma < 10) lcd.print('0');
      lcd.print(mesAlarma);
      lcd.print("/");
      lcd.print(anhoAlarma - 2000); // mostrar anho con dos dígitos

      // HORAS
      lcd.setCursor(0, 2);
      if (formato24Horas) {
        // Formato de 24 horas
        if (horasAlarma < 10) lcd.print('0');
        lcd.print(horasAlarma);
      } else {
        // Formato de 12 horas con A.M./P.M.
        int horas12Alarma = horasAlarma % 12;
        if (horas12Alarma == 0) horas12Alarma = 12; // Ajustar para mostrar 12 A.M./P.M. en lugar de 0
        if (horas12Alarma < 10) lcd.print('0');
        lcd.print(horas12Alarma);
      }

      // MINUTOS
      lcd.print(":");
      if (minutosAlarma < 10) lcd.print('0');
      lcd.print(minutosAlarma);
    }
  }

  // Para que parpadee en el espacio del 0 al 5 en el que se encuentre
  if (estadoParpadeo == LOW && configuracion != 6) {
    lcd.setCursor(posicionXActual, posicionYActual);
    lcd.print("  ");
  }
}

////////////////////////////////Funciones//////////////////////////
void cargarAnimacion() {
  lcd.print("     CARGANDO");
  lcd.setCursor(0, 1);
  for (int barraCarga = 0; barraCarga < 16; barraCarga++) {
    lcd.setCursor(barraCarga, 1);
    lcd.write(1);
    delay(10);
    lcd.setCursor(barraCarga, 1);
    lcd.write(2);
    delay(10);
    lcd.setCursor(barraCarga, 1);
    lcd.write(3);
    delay(10);
    lcd.setCursor(barraCarga, 1);
    lcd.write(4);
    delay(10);
    lcd.setCursor(barraCarga, 1);
    lcd.write(5);
    delay(10);
  }
  delay(500);
  lcd.clear();
}

void encenderLED() {
  switch (tono) {
  case 0:
    digitalWrite(LED_TONO1, HIGH);
    break;
  case 1:
    digitalWrite(LED_TONO1, HIGH);
    break;
  case 2:
    digitalWrite(LED_TONO2, HIGH);
    break;
  case 3:
    digitalWrite(LED_TONO3, HIGH);
    break;
  case 4:
    digitalWrite(LED_TONO4, HIGH);
    break;
  case 5:
    digitalWrite(LED_TONO5, HIGH);
    break;
  }
}

void apagarLED() {
  digitalWrite(LED_TONO1, LOW);
  digitalWrite(LED_TONO2, LOW);
  digitalWrite(LED_TONO3, LOW);
  digitalWrite(LED_TONO4, LOW);
  digitalWrite(LED_TONO5, LOW);
}

void agregarAlarma(int index, int hora, int minuto, int dia, int mes, int anho) {
  if (index >= 0 && index < MAX_ALARMAS) {
    alarmas[index].hora = hora;
    alarmas[index].minuto = minuto;
    alarmas[index].dia = dia;
    alarmas[index].mes = mes;
    alarmas[index].anho = anho;
  }
}

bool comprobarAlarma(int hora, int minuto, int dia, int mes, int anho) {
  for (int i = 0; i < MAX_ALARMAS; i++) {
    if (alarmas[i].hora == hora && alarmas[i].minuto == minuto && 
        alarmas[i].dia == dia && alarmas[i].mes == mes && 
        alarmas[i].anho == anho) {
      return true;
    }
  }
  return false;
}
