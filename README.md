# M5Smile

M5Stack Core S3用のニコちゃんマークアプリ。タッチ操作で表情が変わります。

## 動作

| 操作 | 表情 | 背景色 | 音 |
|------|------|--------|-----|
| 初期状態 | 通常（真顔） | 黒 | - |
| 1回タップ | 微笑み | 白 | 明るいメロディ |
| ダブルタップ | 怒り | 赤 | 低い音 |

※ 5秒後に自動で通常状態に戻ります

## 必要なもの

- M5Stack Core S3
- Arduino IDE または arduino-cli
- M5Unified ライブラリ

## インストール

1. Arduino IDEでM5Stackボードをインストール
2. ライブラリマネージャから `M5Unified` をインストール
3. ボードを `M5Stack CoreS3` に設定
4. `smiley_face.ino` を書き込み

### arduino-cliを使う場合

```bash
arduino-cli compile --fqbn m5stack:esp32:m5stack_cores3 smiley_face.ino
arduino-cli upload --fqbn m5stack:esp32:m5stack_cores3 --port /dev/cu.usbmodem101 smiley_face.ino
```

## ライセンス

MIT
