// Пины разрядов (общий катод: LOW = включить разряд)
int com[4] = {12, 9, 8, 6};

// Коды цифр для 7 сегментов (биты: A B C D E F G)
int sym[10] = {
  126,  // 0
  12,   // 1
  182,  // 2
  158,  // 3
  204,  // 4
  218,  // 5
  250,  // 6
  14,   // 7
  254,  // 8
  222   // 9
};

// Пины сегментов: A,B,C,D,E,F,G,DP
int seg[8] = {11, 7, 4, 2, 1, 10, 5, 3};

int btn = A0;           // Кнопка (INPUT_PULLUP, LOW при нажатии)

int mode = 0;           // 0=сброс(5555), 1=счёт, 2=стоп
int val = 5555;         // Текущее значение для вывода
int prev = 5555;        // Предыдущее значение (для точки)
unsigned long start;    // Время старта счёта
unsigned long flash;    // До скольки мигать точкой

// Антидребезг
bool lastRead = HIGH;
bool btnState = HIGH;
unsigned long debTime = 0;

void setup() {
  for (int i = 0; i < 4; i++) pinMode(com[i], OUTPUT);
  for (int i = 0; i < 8; i++) pinMode(seg[i], OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  clearAll();
}

void loop() {
  // Антидребезг кнопки
  bool now = digitalRead(btn);
  if (now != lastRead) { debTime = millis(); lastRead = now; }
  if (millis() - debTime > 50 && now != btnState) {
    btnState = now;
    if (btnState == LOW) onClick();
  }

  // Режим счёта
  if (mode == 1) {
    int cur = (millis() - start) / 1000;
    if (cur >= 50) { cur = 50; mode = 2; }
    if (cur != val) {
      val = cur;
      flash = millis() + 500;
    }
  }

  if (mode == 0) val = 5555;

  // Мигание точки
  bool dot = (flash > millis()) && ((millis() / 100) % 2 == 0);
  
  display(val, dot);
}

void onClick() {
  if (mode == 0) {        // 5555 -> запуск с 0
    mode = 1;
    val = 0;
    start = millis();
    flash = millis() + 500;
  }
  else if (mode == 1) {   // стоп
    int cur = (millis() - start) / 1000;
    val = (cur > 50) ? 50 : cur;
    mode = 2;
  }
  else if (mode == 2) {   // сброс на 5555
    mode = 0;
    val = 5555;
    flash = millis() + 500;
  }
}

void display(int v, bool dot) {
  int d1 = v / 1000;
  int d2 = (v % 1000) / 100;
  int d3 = (v % 100) / 10;
  int d4 = v % 10;
  int digs[4] = {d1, d2, d3, d4};

  for (int i = 0; i < 4; i++) {
    clearAll();
    showDigit(digs[i], (i == 3) ? dot : false); // точка только на последнем разряде
    digitalWrite(com[i], LOW);  // включаем текущий разряд
    delay(3);
  }
}

void showDigit(int num, bool dot) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(seg[i], bitRead(sym[num], i + 1) ? HIGH : LOW);
  }
  digitalWrite(seg[7], dot ? HIGH : LOW);
}

void clearAll() {
  for (int i = 0; i < 4; i++) digitalWrite(com[i], HIGH);  // выкл все разряды
  for (int i = 0; i < 8; i++) digitalWrite(seg[i], LOW);   // выкл все сегменты
}
