//
// Created by Jaja-new on 10.04.2020.
//

#ifndef PIEZOKEYBOARD_GIVEFREGUENCY_H
#define PIEZOKEYBOARD_GIVEFREGUENCY_H

class GiveFreguency {
    int freguency;
public:
    void GetFreguencyNumber(byte panelTlac) {
        switch (panelTlac) {
            case 1:
                freguency = 270;
                break;
            case 2:
                freguency = 294;
                break;
            case 4:
                freguency = 330;
                break;
            case 8:
                freguency = 349;
                break;
            case 16:
                freguency = 392;
                break;
            case 32:
                freguency = 440;
                break;
            case 64:
                freguency = 493;
                break;
            case 128:
                freguency = 523;
                break;
            default:
                freguency = 0;
        }
    }
    int ReturnFreguencyNumber() {
        return freguency;
    }
};

#endif //PIEZOKEYBOARD_GIVEFREGUENCY_H
