#ifndef RFID_H_
#define RFID_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "rc522.h"

tag_stat find_tag(uint16_t *cardType);
tag_stat select_tag_sn(uint8_t * sn, uint8_t * len);
tag_stat read_tag_str(uint8_t addr, char * str);

#endif /* RFID_H_ */
