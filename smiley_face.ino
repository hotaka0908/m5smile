#include <M5Unified.h>

// 表情の状態
enum FaceState {
  NORMAL,   // 通常
  HAPPY,    // 微笑み（1回タッチ）
  ANGRY     // 怒り（2回タッチ）
};

FaceState currentState = NORMAL;

// タッチ検出用
unsigned long lastTouchTime = 0;
int touchCount = 0;
const unsigned long DOUBLE_TAP_THRESHOLD = 400;  // ダブルタップ判定時間(ms)
bool wasTouched = false;

// 表情リセット用タイマー
unsigned long stateChangeTime = 0;
const unsigned long RESET_DELAY = 5000;  // 5秒後にリセット

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Display.setRotation(1);
  M5.Display.fillScreen(TFT_BLACK);

  // スピーカー初期化
  M5.Speaker.setVolume(128);

  drawFace(NORMAL);
}

void loop() {
  M5.update();

  // タッチ検出
  auto touchCount_now = M5.Touch.getCount();

  if (touchCount_now > 0 && !wasTouched) {
    // タッチ開始
    wasTouched = true;
    unsigned long now = millis();

    if (now - lastTouchTime < DOUBLE_TAP_THRESHOLD) {
      // ダブルタップ検出 → 怒り
      touchCount = 0;
      currentState = ANGRY;
      stateChangeTime = millis();
      drawFace(ANGRY);
      playAngrySound();
    } else {
      // シングルタップの可能性
      touchCount = 1;
      lastTouchTime = now;
    }
  } else if (touchCount_now == 0 && wasTouched) {
    // タッチ終了
    wasTouched = false;
  }

  // シングルタップ確定判定
  if (touchCount == 1 && millis() - lastTouchTime > DOUBLE_TAP_THRESHOLD) {
    // シングルタップ確定 → 微笑み
    touchCount = 0;
    currentState = HAPPY;
    stateChangeTime = millis();
    drawFace(HAPPY);
    playHappySound();
  }

  // 5秒後に通常に戻す
  if (currentState != NORMAL && millis() - stateChangeTime > RESET_DELAY) {
    currentState = NORMAL;
    drawFace(NORMAL);
  }

  delay(10);
}

void drawFace(FaceState state) {
  int centerX = M5.Display.width() / 2;
  int centerY = M5.Display.height() / 2;
  int faceRadius = 80;

  // 背景クリア（通常時は黒、怒りは赤、微笑みは白）
  if (state == NORMAL) {
    M5.Display.fillScreen(TFT_BLACK);
  } else if (state == ANGRY) {
    M5.Display.fillScreen(M5.Display.color565(255, 0, 0));  // 明示的に赤
  } else {
    M5.Display.fillScreen(TFT_WHITE);
  }

  // 顔の輪郭（黄色い丸）
  M5.Display.fillCircle(centerX, centerY, faceRadius, TFT_YELLOW);
  M5.Display.drawCircle(centerX, centerY, faceRadius, TFT_BLACK);

  // 目の位置
  int eyeOffsetX = 25;
  int eyeOffsetY = 20;
  int eyeRadius = 8;

  switch (state) {
    case NORMAL:
      // 通常の目（丸い目）
      M5.Display.fillCircle(centerX - eyeOffsetX, centerY - eyeOffsetY, eyeRadius, TFT_BLACK);
      M5.Display.fillCircle(centerX + eyeOffsetX, centerY - eyeOffsetY, eyeRadius, TFT_BLACK);
      // 通常の口（直線）
      M5.Display.drawLine(centerX - 30, centerY + 30, centerX + 30, centerY + 30, TFT_BLACK);
      break;

    case HAPPY:
      // 嬉しい目（笑顔で閉じた目）
      M5.Display.drawArc(centerX - eyeOffsetX, centerY - eyeOffsetY + 5, eyeRadius + 2, eyeRadius - 2, 200, 340, TFT_BLACK);
      M5.Display.drawArc(centerX + eyeOffsetX, centerY - eyeOffsetY + 5, eyeRadius + 2, eyeRadius - 2, 200, 340, TFT_BLACK);
      // 嬉しい口（大きな笑顔）
      M5.Display.fillArc(centerX, centerY + 20, 40, 20, 0, 180, TFT_BLACK);
      // 頬のピンク
      M5.Display.fillCircle(centerX - 50, centerY + 10, 12, TFT_PINK);
      M5.Display.fillCircle(centerX + 50, centerY + 10, 12, TFT_PINK);
      break;

    case ANGRY:
      // 怒った目（吊り上がった目）
      M5.Display.fillCircle(centerX - eyeOffsetX, centerY - eyeOffsetY, eyeRadius, TFT_BLACK);
      M5.Display.fillCircle(centerX + eyeOffsetX, centerY - eyeOffsetY, eyeRadius, TFT_BLACK);
      // 怒りの眉毛
      M5.Display.fillTriangle(
        centerX - eyeOffsetX - 15, centerY - eyeOffsetY - 15,
        centerX - eyeOffsetX + 15, centerY - eyeOffsetY - 20,
        centerX - eyeOffsetX + 15, centerY - eyeOffsetY - 12,
        TFT_BLACK
      );
      M5.Display.fillTriangle(
        centerX + eyeOffsetX + 15, centerY - eyeOffsetY - 15,
        centerX + eyeOffsetX - 15, centerY - eyeOffsetY - 20,
        centerX + eyeOffsetX - 15, centerY - eyeOffsetY - 12,
        TFT_BLACK
      );
      // 怒った口（への字口）
      M5.Display.drawArc(centerX, centerY + 50, 30, 25, 180, 360, TFT_BLACK);
      break;
  }

  // 状態表示テキスト（通常時・怒り時は白文字、微笑みは黒文字）
  uint32_t textColor = (state == HAPPY) ? TFT_BLACK : TFT_WHITE;
  M5.Display.setTextColor(textColor);
  M5.Display.setTextSize(1);
  M5.Display.setCursor(10, M5.Display.height() - 20);
  switch (state) {
    case NORMAL: M5.Display.print("Normal"); break;
    case HAPPY:  M5.Display.print("Happy! (1 tap)"); break;
    case ANGRY:  M5.Display.print("Angry! (2 taps)"); break;
  }
}

// 微笑み音（明るいメロディ）
void playHappySound() {
  M5.Speaker.tone(440, 100);   // A4
  delay(100);
  M5.Speaker.tone(554, 100);   // C#5
  delay(100);
  M5.Speaker.tone(659, 150);   // E5
  delay(150);
  M5.Speaker.stop();
}

// 怒り音（低い不快な音）
void playAngrySound() {
  M5.Speaker.tone(200, 150);   // 低い音
  delay(150);
  M5.Speaker.tone(150, 200);   // さらに低い音
  delay(200);
  M5.Speaker.stop();
}
