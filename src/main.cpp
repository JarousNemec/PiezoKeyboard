
#include <Arduino.h>
#include <SD.h>
#include <GiveFreguency.h>
#include <TM1638.h>

#define DATA 2
#define CLOCK 3
#define STROBE 4

/// vytvoření instance panel z knihovny TM1638
TM1638 panel(DATA, CLOCK, STROBE);

const short Asize = 300;

const short SSPin = 10;
const short CSPin = 5;

const short PiezoPin = 9;
const short RePlayButtonPin = 7;
const short SignalLedPin = 6;
const short UploadlLedPin = 8;

const short BlinkPause = 200;
const short DataPause = 100;

short SongNumber = 1;
const String SongName = "song";
const String FileExtension = ".ars";
String FileName;

short SongTones[Asize];

bool SDcardConnected = false;

File song;
GiveFreguency GF;


byte panelTlac;
short frequency = 0;

void setup() {
    ///incializace pinů
    pinMode(RePlayButtonPin, INPUT);
    pinMode(SignalLedPin, OUTPUT);
    pinMode(UploadlLedPin, OUTPUT);
    pinMode(SSPin, OUTPUT);
    pinMode(PiezoPin, OUTPUT);
    ///inicializace Seriálového portu
    Serial.begin(9600);
}

///metoda pro přehrávání skladby z pole /SongTones/
void RePlaySong() {
    for (short i = 0; i < Asize; i++) {
        frequency = SongTones[i];
        if (frequency != 0) {
            tone(PiezoPin, frequency);
        } else {
            noTone(PiezoPin);
        }
        delay(DataPause);
    }
}

///metoda pro nahravání skadby do paměti
///skladba je tak velká jak velké je pole /Asize/
void RecordingSong() {

    for (short i = 0; i < Asize; i++) {

        ///pokyny pro piezo reproduktor
        if (frequency != 0) {
            Serial.println(frequency);
            tone(PiezoPin, frequency);
        } else {
            noTone(PiezoPin);
        }

        ///sbírání dat z ovládacího panelu
        panelTlac = panel.getButtons();

        ///rozsvícení LED podle zmáčknutého tlačítka
        panel.setLEDs(((panelTlac & 0xF0) << 8) | (panelTlac & 0xF));

        ///zjískání hodnoty /frequency/
        if (panelTlac != 0) {
            GF.GetFreguencyNumber(panelTlac);
            frequency = GF.ReturnFreguencyNumber();
        } else {
            frequency = 0;
        }

        ///uložení hodnoty do pole
        SongTones[i] = frequency;

        ///potřebná pauza, aby se pole nenaplnilo příliš rychle a protože ne třeba taková rychlost
        delay(DataPause);
    }

    ///vypnutí piezo reproduktoru
    noTone(PiezoPin);

}

///kontrola připojení SD karty na začátku cyklu
///SD karta musí být připojena, aby bylo možno pokračovat
void SDbeginControl() {

    backToStart:
    digitalWrite(SignalLedPin, LOW);
    analogWrite(A2, 0);

    ///kontrola připojení SD karty na začátku cyklu
    ///SD karta musí být připojena, aby bylo možno pokračovat
    if (!SD.begin(CSPin)) {
        Serial.println(" inicializace selhala!");

        ///rozblikání červené LED, když není SD karta připojena
        analogWrite(A2, 1023);
        delay(BlinkPause);

        ///dodá informaci že je SD karta nepřipojena
        SDcardConnected = false;

        ///vrátí program na začátek cyklu
        goto backToStart;
    } else {
        ///dodá informaci že je SD karta připojena
        SDcardConnected = true;
    }
}

void UploadSongToSD() {

    backToStartUpload:

    ///na základě 3 parametrů se složí název souboru /FileName/
    FileName = SongName + SongNumber + FileExtension;

    ///pokud soubor s tímto jménem /FileName/ zatím na SD kartě není vytvořen, vytvoří se a začne se ukládat skladba
    if (!SD.exists(FileName)) {

        ///otevření souboru
        song = SD.open(FileName, FILE_WRITE);

        ///zápis parametrů skladby do souboru /FileName/
        song.println("#Pdata");
        song.println(DataPause);
        song.println("#ASdata");
        song.println(Asize);
        song.println("#Fdata");
        digitalWrite(8, HIGH);

        ///zápis skladby
        for (int i = 0; i < Asize; i++) {
            song.println(SongTones[i]);
            delay(1);
        }

        ///ukončení zápisu do souboru /FileName/
        song.println("##end");
        song.close();
        delay(1000);
        digitalWrite(8, LOW);
    }
        ///pokud soubor již existuje vytvoří se jiný název změněním jednoho z parametrů při tvorbě
    else {
        SongNumber = SongNumber + 1;
        FileName = "";
        goto backToStartUpload;
    }
}
void SERIALW(){
    Serial.println("------------------");
    Serial.println(analogRead(0));
    Serial.println(analogRead(1));
    Serial.println(analogRead(2));
    Serial.println(analogRead(3));
    Serial.println(analogRead(4));
    Serial.println(analogRead(5));
    Serial.println("------------------");
};
void MainMenu() {

    ///začátek cyklu goto
    back:
    noTone(PiezoPin);
    digitalWrite(SignalLedPin, LOW);
    delay(1);
    SERIALW();

    ///při stisknutí tlačítka PLAY-REPLAY se začne přehrávat skladba a program se vrátí na začátek
    if (analogRead(A5) == 1023) {
        RePlaySong();
    }

    ///pokud příjde na A0 signál HIGH spustí se nahrávání na SD kartu
    if (analogRead(A3) == 1023) {
        UploadSongToSD();
    }

    /// je možno okamžitě nahrávat skladbu znovu
    if (analogRead(A1)== 1023) {

        ///kontrola připojení SD karty na začátku cyklu
        ///SD karta musí být připojena, aby bylo možno pokračovat
        SDbeginControl();
        if (SDcardConnected) {
            ///nahrává skladbu dokud nedojde místo
            RecordingSong();
        }
    }

    ///signalizace rozblikáním žluté led, že program čeká na pokyny
    else {
        digitalWrite(SignalLedPin, HIGH);
        delay(BlinkPause);

        goto back;
    }
    ///vrátí program na začátek metody
    goto back;
}



///hlavní metoda celého programu
void loop() {
    MainMenu();
}

