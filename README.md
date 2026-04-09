# esptool
## ZARZĄDZANIE WSL
zainstalowane dystrybucje
```bash 
wsl -l -v
```
dostepne dystrybucje
```bash 
wsl -l -o
```
usuwanie
```bash 
wsl --unregister <DYSTRYBUCJA>
```
instalacja
```bash 
wsl --install <DYSTRYBUCJA>
```
uruchomienie dystrybucji
```bash 
wsl -d <DYSTRYBUCJA>
```
ustawienie domyślnej dystrybucji
```bash 
wsl --set-default <DYSTRYBUCJA>
```

## PROBLEM Z GENEROWANIEM PROJEKTU:
### 1. nowy terminal i:
```bash 
code ~/.platformio/packages/contrib-piohome
```
### 2. wejść w plik:
* main.xxxxxx.js
* lub
* main.xxxxxx.min.js

### 3. ctrl+f
### 4. zamienić
```bash 
"\\":"/"
```
na
```bash 
"/":"/"
```

 
KOMPILACJA NA PŁYTKĘ ARDUINO
pobranie avrdude-v8.1-windows-x64 i rozpakowanie
uruchomienie avrdude.exe
skompilowanie programu w PlatformIO
uruchomienie cmd i tam:
avrdude.exe -p atmega328p -c arduino -P COM3 -b 115200 -U flash:w:"\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\uno\firmware.hex":i
PODKREŚLONE SPRAWDZIĆ


## KOMPILACJA NA PŁYTKĘ ESP
### 1.
```bash 
pip install esptool
```
### 2.
pobrać sterownik do programatora esp
```bash 
https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
```
### 3.
w managerze urządzeń jak wchodzi się w esp(bez sterownika, to tam wybrać wgraj sterownik i wybrać odpowiedni)
### 4.
test płytki:
```bash 
python -m esptool --chip esp32 --port COM4 flash_id
```
* podmienić COM na poprawny
### 5.
wgranie:
```bash 
python -m esptool --chip esp32 --port COM4 write-flash -z 0x1000 "\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\esp32dev\bootloader.bin" 0x8000 "\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\esp32dev\partitions.bin" 0x10000 "\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\esp32dev\firmware.bin"
```
