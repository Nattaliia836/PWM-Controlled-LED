const int ledPin = 11;         // пін до якого під'єднано світлодіод
const int buttonPin = 12;      // пін до якого під'єднано кнопку
const int debounceDelay = 50;  // затримка для debounce

// початкові стани
bool ledState = false;
bool lastButtonState = LOW;
bool buttonState = LOW;

// почакові значення часових проміжків
unsigned long lastDebounceTime = 0;
unsigned long fadeStartTime = 0;  // момент часу коли починається або перезапускається цикл яскравості
const unsigned long fadeDuration = 2000;  // час зміни яскравості

void setup() {
  // встановлення пінів
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  // дебаунс кнопки
  bool reading = digitalRead(buttonPin);    // зчитуємо поточний стан кнопки
  if (reading != lastButtonState) {         // якщо поточний стан не відповідає останньому
    lastDebounceTime = currentMillis;       // оновлюємо lastDebounceTime
  }

  // перевірка чи пройшло достатньо часу для зміни кнопки та опрацювання сигналу
  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    // якщо статус кнопки змінився
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        //якщо кнопка натиснута змінюємо стан світлодіоду
        ledState = !ledState;
        if (ledState) {
          //якщо світлодіод вмикається змінюємо fadeStartTime
          fadeStartTime = currentMillis;
        }
      }
    }
  }
  // після закінчення опрацювання статусу кнопки змінюємо lastButtonState 
  lastButtonState = reading;


  // зміна яскравості світлодіоду
  if (ledState) {
    // розраховуємо час що минув після зміни яскравості світлодіоду
    unsigned long elapsedTime = currentMillis - fadeStartTime;

    //визначення теперішнього значення яскравості
    // 0-1 світлодіод зменшує яскравість від 100% до 0%
    // 1-2 світлодіод збільшує яскравість від 0% до 100%
    float brightness = (float)elapsedTime / fadeDuration;

    // якщо brightness більше 1, тобто пройшло >= 2 секунд з останньої зміни стану світлодіоду
    // світлодіод на 0% яскравості, починаємо інферсію світлодіоду (збільшення яскравості)
    if (brightness > 1.0) {
      // brightness має бути в проміжку від 0 до 1 (для розрахунку pwmValue)
      // так як світлодіод досяг 0% яскравості - збільшуємо яскравіть
      brightness = 2.0 - brightness;

      // якщо нове значення brightness менше нуля, значить світлодіод знову на 100%, обнуляємо цикл
      if (brightness <= 0) {
        brightness = 0;
        //змінюємо час зміни статусу світлодіоду на теперішній
        fadeStartTime = currentMillis;
      }
    }

    // розраховуємо яскравість світлодіоду
    // в формулі використовуємо (1.0 - brightness), для того щоб після включення кнопки завжди мати 100%
    // якщо замінити на просто brightness, то світлодіод починатиме з 0%
    int pwmValue = (int)((1.0 - brightness) * 255);
    // змінюємо яскравість світлодіоду
    analogWrite(ledPin, pwmValue);
  } else {
    // вимикаємо світлодіод
    analogWrite(ledPin, 0);
  }
}