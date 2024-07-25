# ESP32 Audio Recording and Transcription

Proyek ini menggunakan ESP32 untuk merekam audio, mengirimkan data ke server Flask, dan mendapatkan transkripsi dari audio tersebut menggunakan AssemblyAI.

## Komponen yang Dibutuhkan

- ESP32
- Modul MAX4466
- Modul OLED 128x64
- Tombol
- Kartu SD dan modul SD card reader

## Instalasi Arduino

1. Pastikan Anda telah menginstal [Arduino IDE](https://www.arduino.cc/en/software).
2. Tambahkan URL board ESP32 ke Preferences di Arduino IDE: `https://dl.espressif.com/dl/package_esp32_index.json`.
3. Install board ESP32 melalui Board Manager.
4. Install library berikut melalui Library Manager:
   - `WiFi.h`
   - `HTTPClient.h`
   - `Adafruit_GFX.h`
   - `Adafruit_SSD1306.h`
   - `ArduinoJson.h`

## Konfigurasi Hardware

### Pin Koneksi

- MAX4466 ke Pin ADC ESP32: `34`
- Tombol ke Pin: `25`
- OLED SCK ke Pin: `22`
- OLED SDA ke Pin: `21`

## Instalasi Server Flask

1. Pastikan Anda telah menginstal Python 3.x.
2. Install library yang diperlukan dengan menjalankan:
   ```bash
   pip install flask numpy wave assemblyai
