/*
 * I2C_EndEffector.c
 *
 *  Created on: 1 มิ.ย. 2566
 *      Author: natch
 */

#include "I2C_EndEffector.h"
#include "math.h"
#include "ModBusRTU.h"

#define SLAVE_ADDR (0x15<<1) //End Effector Address
uint32_t timestamp;
int complete;
void ENDEFF_SOFT_RESET(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[4] = { 0x00, 0xFF, 0x55, 0xAA };
	if (HAL_GetTick() - timestamp >= 10) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 4, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
void ENDEFF_EMERGENCY(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[1] = { 0xF0 };
	if (HAL_GetTick() - timestamp >= 10) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 1, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}

}
void ENDEFF_EMERGENCY_QUIT(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[4] = { 0xE5, 0x7A, 0xFF, 0x81 };
	if (HAL_GetTick() - timestamp >= 10) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 4, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
void ENDEFF_TEST_MODE(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[2] = { 0x01, 0x11 };
	if (HAL_GetTick() - timestamp >= 10) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 2, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
void ENDEFF_TEST_MODE_QUIT(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[2] = { 0x01, 0x00 };
	if (HAL_GetTick() - timestamp >= 10) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 2, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
void ENDEFF_GRIPPER_RUNMODE(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[2] = { 0x10, 0x13 };
	if (HAL_GetTick() - timestamp >= 10) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 2, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
void ENDEFF_GRIPPER_IDLE(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[2] = { 0x10, 0x8C };
	if (HAL_GetTick() - timestamp >= 10) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 2, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
void ENDEFF_GRIPPER_PICK(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[2] = { 0x10, 0x5A };
	if (HAL_GetTick() - timestamp >= 2000) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 2, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
void ENDEFF_GRIPPER_PLACE(I2C_HandleTypeDef *hi2c) {
	static uint8_t data[2] = { 0x10, 0x69 };
	if (HAL_GetTick() - timestamp >= 2000) {
		HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDR, data, 2, HAL_MAX_DELAY);
		timestamp = HAL_GetTick();
		complete = 1;
		return;
	}
}
//void decimalToBinary(uint8_t decimal, char *binary) {
//	int index = 0;
//	for (int i = 0; i < sizeof(binary); i++) {
//		binary[i] = 0;
//	}
//	if (decimal == 0) {
//		binary[index++] = '0';
//	} else {
//		while (decimal > 0) {
//			binary[index++] = (decimal & 1) ? '1' : '0';
//			decimal >>= 1;
//		}
//	}
//	binary[index] = '0'; // Add null terminator to indicate the end of the string
//}
void I2C_TO_BASESYSTEM(uint16_t *DataFrame, I2C_HandleTypeDef *hi2c) {
	static int picked;
	static enum {
		INIT, TEST, RUNMODE, PICKED
	} END_EFFECTOR_STATE = INIT;
	switch (END_EFFECTOR_STATE) {
	case (INIT):
		if ((*DataFrame & 0b0001) == 1) {
			ENDEFF_TEST_MODE(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = TEST;
				complete = 0;
			}
		} else if ((*DataFrame & 0b0010) == 0b10) {
			ENDEFF_GRIPPER_RUNMODE(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = RUNMODE;
				complete = 0;
			}
		}
		break;
	case (TEST):
		if ((*DataFrame & 0b0001) == 0) {
			ENDEFF_TEST_MODE_QUIT(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = INIT;
				complete = 0;
			}
		} else if ((*DataFrame & 0b0010) == 0b10) {
			ENDEFF_TEST_MODE_QUIT(hi2c);
			ENDEFF_GRIPPER_RUNMODE(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = RUNMODE;
				complete = 0;
			}
		}
		break;
	case (RUNMODE):
		if ((*DataFrame & 0b0010) == 0) {
			ENDEFF_GRIPPER_IDLE(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = INIT;
				complete = 0;
			}
		} else if ((*DataFrame & 0b0001) == 1) {
			ENDEFF_TEST_MODE(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = TEST;
				complete = 0;
			}
		} else if ((*DataFrame & 0b0100) == 0b100) {
			ENDEFF_GRIPPER_PICK(hi2c);
			if (complete == 1) {
				*DataFrame = 0b0010;
				END_EFFECTOR_STATE = PICKED;
				complete = 0;
				picked = 1;
			}
		} else if ((*DataFrame & 0b1000) == 0b1000) {
			if (picked == 1) {
				ENDEFF_GRIPPER_PLACE(hi2c);
				if (complete == 1) {
					*DataFrame = 0b0010;
					complete = 0;
					picked = 0;
				}
			}
		}
		break;
	case (PICKED):
		if ((*DataFrame & 0b0010) == 0) {
			ENDEFF_GRIPPER_IDLE(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = INIT;
				complete = 0;
			}
		} else if ((*DataFrame & 0b0001) == 1) {
			ENDEFF_TEST_MODE(hi2c);
			if (complete == 1) {
				END_EFFECTOR_STATE = TEST;
				complete = 0;
			}
		} else if ((*DataFrame & 0b1000) == 0b1000) {
			ENDEFF_GRIPPER_PLACE(hi2c);
			if (complete == 1) {
				*DataFrame = 0b0010;
				END_EFFECTOR_STATE = RUNMODE;
				complete = 0;
				picked = 0;
			}
		}
		break;
	}
}
//void Transmit_To_Basesystem(uint16_t* DataFrame,I2C_HandleTypeDef *hi2c)
//{
//	static int lastTest;
//	static int lastRun;
//	static int lastPick;
//	if ((*DataFrame&0b0001) == 1 && lastTest == 0) {
//		ENDEFF_TEST_MODE(hi2c);
//		lastTest = 1;
//	} else if ((*DataFrame&0b0001) == 0 && lastTest == 1) {
//		ENDEFF_TEST_MODE_QUIT(hi2c);
//		lastTest = 0;
//	} else if ((*DataFrame&0b0010) == 0b10 && lastRun == 0) {
//		ENDEFF_GRIPPER_RUNMODE(hi2c);
//		lastRun = 1;
//	} else if ((*DataFrame&0b0010) == 0 && lastRun == 1) {
//		ENDEFF_GRIPPER_IDLE(hi2c);
//		lastRun = 0;
//	} else if ((*DataFrame&0b0100) == 0b100 && (*DataFrame&0b0010) == 0b10 && lastPick == 0) {
//		ENDEFF_GRIPPER_PICK(hi2c);
//		*DataFrame = 0b0010;
//		lastPick = 1;
//	} else if ((*DataFrame&0b1000) == 0b1000 && (*DataFrame&0b0010) == 0b10 && lastPick == 1) {
//		ENDEFF_GRIPPER_PLACE(hi2c);
//		*DataFrame = 0b0010;
//		lastPick = 0;
//	}
//}
//void TransmitToBase(char* Data,uint8_t result)
//{
//	  char NewData[8];
//	  result = 0;
//	  for(int i = 0; i < 4;i++)
//	  {
//		  if(i == 0) NewData[2] = Data[i];
//		  else if(i == 1) NewData[3] = Data[i];
//		  else if(i == 2) NewData[1] = Data[i];
//		  else if(i == 3) NewData[0] = Data[i];
//	  }
//
//	  for(int j = 0; j<sizeof(NewData);j++)
//	  {
//		  if(NewData[j] == '\0') j = sizeof(NewData);
//		  else result += ((int)NewData[j]-'0')*pow(2,j);
//	  }
//}
