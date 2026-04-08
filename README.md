# esptool
ZARZĄDZANIE WSL
wsl -l -v
wsl -l -o

wsl --install <DYSTRYBUCJA>
wsl --unregister <DYSTRYBUCJA>
wsl -d <DYSTRYBUCJA>
wsl --set-default <DYSTRYBUCJA>



PROBLEM Z GENEROWANIEM PROJEKTU:
nowy terminal i:
code ~/.platformio/packages/contrib-piohome
wejść w plik main.xxxxxx.js lub main.xxxxxx.min.js
ctrl+f
"\\":"/"  zamienić na:  "/":"/"
wsl --shutdown

 
KOMPILACJA NA PŁYTKĘ ARDUINO
pobranie avrdude-v8.1-windows-x64 i rozpakowanie
uruchomienie avrdude.exe
skompilowanie programu w PlatformIO
uruchomienie cmd i tam:
avrdude.exe -p atmega328p -c arduino -P COM3 -b 115200 -U flash:w:"\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\uno\firmware.hex":i
PODKREŚLONE SPRAWDZIĆ



KOMPILACJA NA PŁYTKĘ ESP
pip install esptool
pobrać https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads - sterownik do programatora esp
w managerze urządzeń jak wchodzi się w esp(bez sterownika, to tam wybrać wgraj sterownik i wybrać odpowiedni)
test płytki: python -m esptool --chip esp32 --port COM4 flash_id
wgranie: python -m esptool --chip esp32 --port COM4 write-flash -z 0x1000 "\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\esp32dev\bootloader.bin" 0x8000 "\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\esp32dev\partitions.bin" 0x10000 "\\wsl.localhost\FedoraLinux-43\home\user\Documents\PlatformIO\Projects\test\.pio\build\esp32dev\firmware.bin"
