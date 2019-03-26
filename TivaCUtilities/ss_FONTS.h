/*
 * ss_FONTS.h
 *
 *  Created on: 17-Jul-2018
 *      Author: sagarshubham
 */

#ifndef SS_FONTS_H_
#define SS_FONTS_H_

#include "stdint.h"
#include "stdbool.h"
#include "FONT_8x8.h"

typedef struct FONT {
    uint16_t fontHeight;
    uint16_t fontWidth;
    uint16_t fontNumberOfCharacters;
    uint16_t fontMaxCharactersOnScreen;
    char fontFirstCharacter;
    const uint8_t *fontArrayPointer ;
}FONT;



#endif /* SS_FONTS_H_ */
