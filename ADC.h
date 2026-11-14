/* Name:  Amelia Smith
 * Assignment:	Engs 28 26F, Lab 5
 *
 * Program name:	ADC.h
 * Date created:	10.18.26
 * Description:	Declares ADC functions used to initialize ADC, select input channel, set resolution, and get ADC readings
 *
 */

#ifndef MYLIB_ADC_H_
#define MYLIB_ADC_H_

#include <stdint.h>
void adc_init (void);
void adc_setChannel (unsigned int chNum);
void adc_setWidth (int widthCode);
int adc_getValue (void);

#endif /* MYLIB_ADC_H_ */

