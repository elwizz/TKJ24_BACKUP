/*
 * opt3001.c
 *
 *  Created on: 22.7.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/opt3001.pdf
 */

#include <string.h>
#include <math.h>

#include <xdc/runtime/System.h>

#include "sensors/opt3001.h"
#include "Board.h"

void opt3001_setup(I2C_Handle *i2c) {

	I2C_Transaction i2cTransaction;
	char itxBuffer[3];

    i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
    itxBuffer[0] = OPT3001_REG_CONFIG;
    itxBuffer[1] = 0xCE; // continuous mode s.22
    itxBuffer[2] = 0x02;
    i2cTransaction.writeBuf = itxBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(*i2c, &i2cTransaction)) {

        System_printf("OPT3001: Config write ok\n");
    } else {
        System_printf("OPT3001: Config write failed!\n");
    }
    System_flush();

}

uint16_t opt3001_get_status(I2C_Handle *i2c) {

	uint16_t e=0;
	I2C_Transaction i2cTransaction;
	char itxBuffer[1];
	char irxBuffer[2];

	/* Read sensor state */
	i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
	itxBuffer[0] = OPT3001_REG_CONFIG;
	i2cTransaction.writeBuf = itxBuffer;
	i2cTransaction.writeCount = 1;
	i2cTransaction.readBuf = irxBuffer;
	i2cTransaction.readCount = 2;

	if (I2C_transfer(*i2c, &i2cTransaction)) {

		e = (irxBuffer[0] << 8) | irxBuffer[1];
	} else {

		e = 0;
	}
	return e;
}

/**************** JTKJ: DO NOT MODIFY ANYTHING ABOVE THIS LINE ****************/

double opt3001_get_data(I2C_Handle *i2c) {

    // JTKJ: Tehtävä 2. Muokkaa funktiota niin että se palauttaa mittausarvon lukseina
    // JTKJ: Exercise 2. Complete this function to return the measured value as lux

	double lux; // return value of the function
    // JTKJ: Find out the correct buffer sizes (n) with this sensor?
	// i2c-viesteille lähetys- ja vastaanottopuskurit
	uint8_t txBuffer[1];
    uint8_t rxBuffer[2];

	// JTKJ: Fill in the i2cMessage data structure with correct values
    //       as shown in the lecture material
    I2C_Transaction i2cMessage;
    i2cMessage.slaveAddress = Board_OPT3001_ADDR;
    txBuffer[0] = OPT3001_REG_RESULT; // Rekisterin osoite lähetyspuskuriin
    i2cMessage.writeBuf = txBuffer; // Lähetyspuskurin asetus
    i2cMessage.writeCount = 1;      // Lähetetään 1 tavu
    i2cMessage.readBuf = rxBuffer;  // Vastaanottopuskurin asetus
    i2cMessage.readCount = 2;       // Vastaanotetaan 2 tavua

	if (opt3001_get_status(i2c) & OPT3001_DATA_READY) {

		if (I2C_transfer(*i2c, &i2cMessage)) {
	        // JTKJ: Here the conversion from register value to lux
		    uint16_t RN_figure = rxBuffer[0] >> 4;
		    uint16_t temp = rxBuffer[0] | rxBuffer[1];
		    uint16_t R_figure = temp | 0x7FF;
		    lux = 0.01 * pow(2, RN_figure) * R_figure;

		} else {

			System_printf("OPT3001: Data read failed!\n");
			System_flush();
		}

	} else {
		System_printf("OPT3001: Data not ready!\n");
		System_flush();
	}

	return lux;
}
