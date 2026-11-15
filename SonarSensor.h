/* Sonar Sensor Header File
 * Names		: Amelia Smith and Mazvita Nhidza
 * Program name	: SonarSensor.h
 * Date created	: 11.14.26
 * Descrption	:
 * 
 * Dependencies	: 
 *
 * I/O Pins
 * 
 * Revisions: 
 * 
 */

#ifndef SONARSENSOR_H_
#define SONARSENSOR_H_

#include "stm32C0x.h"
#include <stdio.h>

// functions
void sonar_init(void);			// initialize the sonar sensor
int sonar_read(void);			// read/compute sonar distance data
void sonarTimer_init(int microseconds);		// initialize microsecond timer
void TIM17_IRQHandler (void);	// handle timer interrupts

