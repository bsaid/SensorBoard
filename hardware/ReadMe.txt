ESP32 Sensor board:

Popis desky:
- dvě desky jako sendvič 50 mm * 31 mm * 13 mm
- propojení desek lze využít jako konektor pro připojení dalších periferií
- 3 možnosti bezdrátového připojení + USB

Vlastnosti:
- vlastní LiPoly baterie se správou napájení a nabíjením přes USB
- USB FTDI
- 80 MHz procesor
- micro SD karta standard 3.0 (lze >32 GB)
- 14 pinů pro připojení volitelných periferií, i výstupních (serva, LED, DAC reproduktor)
- Bluetooth 4.0 nebo WiFi připojení, lze v jeden okamžik používat jen jedno
- 433 nebo 868 MHz radio pro komunikaci na vzdálenost do 1.5 km nebo pro komunikaci skrz biologický materiál (tělo koně)
- citlivá elektronika ukrytá uvnitř sendviče
- 2 tlačítka
- 2 SW LED (+ několik HW LED)
- adapter na magnetický USB-C pro lepší utěsnění proti prachu
- možnost připojit senzor srdečního tepu, dechu, saturace krve kyslíkem (po konzultaci s Monet+)

Senzory:
- akcelerometr
- gyroskop
- kompas
- vlhkoměr
- barometr
- teploměr
- externě GPS, instenzita světla, mikrofon

Navíc:
- deska má dostatek senzorů, výkonu na procesoru a vyvedených konektorů na to, aby šla naprogramovat jako jednoduchý autopilot
- návrh jsem zamýšlel i jako univerzální desku pro vývoj na ESP32 procesorech s některými senzory jako bonus
- 2x DAC (třeba pro příjem a přehrání zvuku)

Problémy během návrhu:
- dostatečně malá deska, která nebude zvířeti zavazet
- chybějící kondenzátor na pinu EN způsoboval nepředvídatelné chování a občas úplnou nefunkčnost procesoru ESP32
- těžko sehnatelná ručně pájitelná pouzdra senzorů
- výběr vhodného čipu pro správu napájení, stále slabé místo, nevyzkoušené předem
- komunikace s firmou a získání relevantních informací a požadavků

Seznam součástek je v samostatném souboru.
Původní zadání a high-level požadavky jsou v samostatném souboru.
Všechny použité součástky jsou uloženy jako samostatné knihovny ve složce lib.
