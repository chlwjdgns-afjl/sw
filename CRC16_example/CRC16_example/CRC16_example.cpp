// CRC16_example.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "stdint.h"


typedef unsigned char  BYTE_t;

union
{
	float data;
	char  bytedata[4];

}  m_voltage, m_current, m_temperature;

union
{
	short data;
	char  bytedata[2];

} m_int16;

union
{
	unsigned short check_sum;
	unsigned char b[2];
} crc_16_val;

BYTE_t pwm_data[160] = { 0, };
bool m_IG_bank[160]  = { 0, };
bool m_RUN_bank[160] = { 0, };
bool m_OCP_bank[160] = { 0, };
bool m_OTP_bank[160] = { 0, };


unsigned char protocal_test[250] = { 0, };

int CRC16_MODBUS(const uint8_t *nData, uint16_t wLength)
{

	static const uint16_t wCRCTable[] = { 0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241, 0XC601, 0X06C0, 0X0780,
		0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440, 0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1,
		0XCE81, 0X0E40, 0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841, 0XD801,
		0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40, 0X1E00, 0XDEC1, 0XDF81, 0X1F40,
		0XDD01, 0X1DC0, 0X1C80, 0XDC41, 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680,
		0XD641, 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040, 0XF001, 0X30C0,
		0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240, 0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501,
		0X35C0, 0X3480, 0XF441, 0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
		0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840, 0X2800, 0XE8C1, 0XE981,
		0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41, 0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1,
		0XEC81, 0X2C40, 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640, 0X2200,
		0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041, 0XA001, 0X60C0, 0X6180, 0XA141,
		0X6300, 0XA3C1, 0XA281, 0X6240, 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480,
		0XA441, 0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41, 0XAA01, 0X6AC0,
		0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840, 0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01,
		0X7BC0, 0X7A80, 0XBA41, 0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
		0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640, 0X7200, 0XB2C1, 0XB381,
		0X7340, 0XB101, 0X71C0, 0X7080, 0XB041, 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0,
		0X5280, 0X9241, 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440, 0X9C01,
		0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40, 0X5A00, 0X9AC1, 0X9B81, 0X5B40,
		0X9901, 0X59C0, 0X5880, 0X9841, 0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81,
		0X4A40, 0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41, 0X4400, 0X84C1,
		0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641, 0X8201, 0X42C0, 0X4380, 0X8341, 0X4100,
		0X81C1, 0X8081, 0X4040 };

	uint8_t nTemp;
	uint16_t wCRCWord = 0xFFFF;

	while (wLength--)
	{
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord ^= wCRCTable[nTemp];
	}

	return wCRCWord;
}

void set_pwm_channel(BYTE_t IPD_channel, BYTE_t pwm_data)
{
	int i = 0;

	protocal_test[0] = 184;                 //PC
	protocal_test[1] = 174;                 //IPD_controller
	protocal_test[2] = 's';                 // PID
	protocal_test[3] = 2;                   // data number
	protocal_test[4] = IPD_channel;         // data number
	protocal_test[5] = pwm_data;            // data number

	crc_16_val.check_sum = CRC16_MODBUS(protocal_test, 6);


	protocal_test[6] = crc_16_val.b[1];
	protocal_test[7] = crc_16_val.b[0];

	printf("Send Protocal : ");
	for (i = 0; i <= 7; i++)
	{
		printf("0x%02X ", protocal_test[i]);
	}
	printf("\n");
	for (i = 0; i <= 7; i++)
	{
		printf("%3d ", protocal_test[i]);
	}
	printf("\n\n");

}


void stop_pwm_channel(BYTE_t IPD_channel)
{
	
	int i = 0;

	protocal_test[0] = 184;                 //PC
	protocal_test[1] = 174;                 //IPD_controller
	protocal_test[2] = 's';                 // PID
	protocal_test[3] = 2;                   // data number
	protocal_test[4] = IPD_channel;         // data number
	protocal_test[5] = 0;                   // data number

	crc_16_val.check_sum = CRC16_MODBUS(protocal_test, 6);


	protocal_test[6] = crc_16_val.b[1];
	protocal_test[7] = crc_16_val.b[0];

	printf("Send Protocal : ");
	for (i = 0; i <= 7; i++)
	{
		printf("0x%02X ", protocal_test[i]);
	}
	printf("\n");
	for (i = 0; i <= 7; i++)
	{
		printf("%3d ", protocal_test[i]);
	}

	//printf("0x%02x " , protocal_test[0]);
	printf("\n\n");

}


void send_IPD_PWM_bank_data(BYTE_t bank_id, BYTE_t pwm_channel_data[])
{
    int i = 0;
	int array_index = 0;

	protocal_test[0] = 184;                               // PC
	protocal_test[1] = 174;                               // IPD_controller
	protocal_test[2] = 'A';                               // PID
	protocal_test[3] = 21;                                // data number
	protocal_test[4] = bank_id;                           // bank ID , ID 당 20개 data 보냄


	for (i = 0; i < 20; i++)
	{
		array_index = (bank_id * 20) + i;

		protocal_test[5+i] = i + 50;                    
		//protocal_test[5 + i] = pwm_channel_data[array_index];

	}
	
	crc_16_val.check_sum = CRC16_MODBUS(protocal_test, 44);

	protocal_test[24] = crc_16_val.b[1];
	protocal_test[25] = crc_16_val.b[0];
	printf("send IPD PWM Bank data\n"); 
	printf("Send Protocal : ");

	for (i = 0; i <= 25; i++)
	{
		printf("0x%02X ", protocal_test[i]);
	}
	printf("\n\n\n\n");


}


void send_IPD_data(BYTE_t IPD_channel, float voltage, float current, float temperature)
{
	int i = 0;

	protocal_test[0] = 174;                               // IPD_controller
	protocal_test[1] = 184;                               // PC
	protocal_test[2] = 'd';                               // PID
	protocal_test[3] = 13;                                // data number

	protocal_test[4] = IPD_channel;
	// voltage data 4 byte float

	m_voltage.data = voltage;

	protocal_test[5] = m_voltage.bytedata[3];
	protocal_test[6] = m_voltage.bytedata[2];
	protocal_test[7] = m_voltage.bytedata[1];
	protocal_test[8] = m_voltage.bytedata[0];

		// currnet data 4 byte float
	m_current.data = current;
	protocal_test[9] = m_current.bytedata[3];
	protocal_test[10] = m_current.bytedata[2];
	protocal_test[11] = m_current.bytedata[1];
	protocal_test[12] = m_current.bytedata[0];


	// temp data 4 byte float

	m_temperature.data = temperature;
	protocal_test[13] = m_temperature.bytedata[3];
	protocal_test[14] = m_temperature.bytedata[2];
	protocal_test[15] = m_temperature.bytedata[1];
	protocal_test[16] = m_temperature.bytedata[0];

	crc_16_val.check_sum = CRC16_MODBUS(protocal_test, 16);
	// Modbus crc16

	protocal_test[17] = crc_16_val.b[1];
	protocal_test[18] = crc_16_val.b[0];
	
	printf("Send IPD data\n");

	printf("Send Protocal : ");
	for (i = 0; i <= 18; i++)
	{
		printf("0x%02X ", protocal_test[i]);
	}
	printf("\n\n\n");
}


void send_battery_data(float voltage, float current, float temperature)
{
	int i = 0;
	protocal_test[0] = 174;                               // IPD_controller
	protocal_test[1] = 184;                               // PC
	protocal_test[2] = 'b';                               // PID
	protocal_test[3] = 12;                                // data number

	
	m_voltage.data = voltage;

	protocal_test[4] = m_voltage.bytedata[3];
	protocal_test[5] = m_voltage.bytedata[2];
	protocal_test[6] = m_voltage.bytedata[1];
	protocal_test[7] = m_voltage.bytedata[0];

	// currnet data 4 byte float
	m_current.data = current;
	protocal_test[8] = m_current.bytedata[3];
	protocal_test[9] = m_current.bytedata[2];
	protocal_test[10] = m_current.bytedata[1];
	protocal_test[11] = m_current.bytedata[0];


	// temp data 4 byte float

	m_temperature.data = temperature;
	protocal_test[12] = m_temperature.bytedata[3];
	protocal_test[13] = m_temperature.bytedata[2];
	protocal_test[14] = m_temperature.bytedata[1];
	protocal_test[15] = m_temperature.bytedata[0];

	crc_16_val.check_sum = CRC16_MODBUS(protocal_test, 15);
	// Modbus crc16

	protocal_test[16] = crc_16_val.b[1];
	protocal_test[17] = crc_16_val.b[0];

	printf("Send Battery Data\n");
	printf("Send Protocal : ");
	for (i = 0; i <= 17; i++)
	{
		printf("0x%02X ", protocal_test[i]);
	}
	printf("\n\n\n");
}

void send_warning_data(BYTE_t IPD_channel, bool IG, bool RUN, bool OCP, bool OTP)
{
	int i = 0;
	protocal_test[0] = 174;                               // IPD_controller
	protocal_test[1] = 184;                               // PC
	protocal_test[2] = 'w';                               // PID
	protocal_test[3] = 2;                                 // data number

	protocal_test[4] = IPD_channel;                                // data number
	protocal_test[5] = (IG * 8) + (RUN * 4) + (OCP * 2) + (OTP * 1);
	
	crc_16_val.check_sum = CRC16_MODBUS(protocal_test, 5);
	// Modbus crc16

	protocal_test[6] = crc_16_val.b[1];
	protocal_test[7] = crc_16_val.b[0];
	printf("Send Warning Data\n");
	printf("Send Protocal : ");
	for (i = 0; i <= 7; i++)
	{
		printf("0x%02X ", protocal_test[i]);
	}
	printf("\n\n\n");

}

void send_warning_bank_data(BYTE_t bank_id, bool IG_bank[], bool RUN_bank[], bool OCP_bank[], bool OTP_bank[])
{
	int i = 0;
	int array_index = 0;
	protocal_test[0] = 174;                               // IPD_controllerPC
	protocal_test[1] = 184;                               // PC
	protocal_test[2] = 'W';                               // PID  bank로 보낼때는 대문자로
	protocal_test[3] = 21;                                // data number

	protocal_test[4] = bank_id;                           // bank ID  20개당 1개씩 보냄

	for (i = 0; i < 20; i++)
	{
		array_index = (bank_id * 20) + i;

		//protocal_test[5+i] = (IG_bank[array_index] * 8) + (RUN_bank[array_index] * 4) + (OCP_bank[array_index] * 2) + (OTP_bank[array_index] * 1);
		protocal_test[5 + i] = (1 * 8) + (1 * 4) + (0 * 2) + (1 * 8);
	}


	crc_16_val.check_sum = CRC16_MODBUS(protocal_test, 24);
	// Modbus crc16

	protocal_test[25] = crc_16_val.b[1];
	protocal_test[26] = crc_16_val.b[0];
	printf("Send Warning Data\n");
	printf("Send Protocal : ");
	for (i = 0; i <= 26; i++)
	{
		printf("0x%02X ", protocal_test[i]);
	}
	printf("\n\n\n");

}


int _tmain(int argc, _TCHAR* argv[])
{
	
	/*
	set_pwm_channel(1, 80);
	
	stop_pwm_channel(1);
	
	send_IPD_PWM_bank_data(1, pwm_data); // 1번 뱅크에 pwm data 20 개 보냄
	
	send_IPD_data(1, 5.3, 1.1, 80.3);
	
	send_battery_data(12.4, 10.7, 30.3);
	
	send_warning_data(2, 1, 1, 0, 1);

	send_warning_bank_data(2, m_IG_bank, m_RUN_bank, m_OCP_bank, m_OTP_bank);
	*/

	send_IPD_data(1, 5.3, 1.1, 80.3);

	return 0;
}

