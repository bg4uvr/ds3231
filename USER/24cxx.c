/********************************************************************************

	STM32F103C8T6ģ�� + DS3231 + 24C32��ģ�� + TM1637�����ģ�� + OLED��ʾģ��

	��ʵʱʱ�� + �¶Ȳ��������棬ʵʱOLED��ʾ�¶����ߣ������ʵ����ʾʱ�ӡ�

															�ģ�ʵ����򡭡�

														2019.2.3��� by bg4uvr~

********************************************************************************/



#include "24cxx.h"
#include "delay.h"

//Mini STM32������
//24CXX��������(�ʺ�24C01~24C16,24C32~256δ��������!�д���֤!)
//����ԭ��@ALIENTEK
//2010/6/10
//V1.2

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
uint8_t AT24CXX_ByteRead(uint16_t ReadAddr)
{
	uint8_t temp=0;
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);		//����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);	//���͸ߵ�ַ
	}
	else
		IIC_Send_Byte(0xA0 + ((ReadAddr/256)<<1));   //����������ַ0xA0,д����

	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256);	//���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(EE_ADD|0x01);		//�������ģʽ
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);
	IIC_Stop();						//����һ��ֹͣ����
	return temp;
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
void AT24CXX_ByteWrite(uint16_t WriteAddr, uint8_t DataToWrite)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
	}
	else
		IIC_Send_Byte(EE_ADD + ((WriteAddr/256)<<1));   //����������ַ0xA0,д����
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Send_Byte(DataToWrite);     //�����ֽ�
	IIC_Wait_Ack();
	IIC_Stop();						//����һ��ֹͣ����
	delay_ms(2);
}

//ҳ��д��
void AT24CXX_PageWrite(uint16_t WriteAddr,  uint8_t Len, uint8_t *buf)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
	}
	else
		IIC_Send_Byte(EE_ADD + ((WriteAddr/256)<<1));   //����������ַ0xA0,д����
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();
	while(Len--)
	{
		IIC_Send_Byte(*(buf++));	//�����ֽ�
		IIC_Wait_Ack();
	}
	IIC_Stop();						//����һ��ֹͣ����
	delay_ms(10);
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//ReadAddr   :��ʼ�����ĵ�ַ
//Len        :Ҫ�������ݵĳ���
void AT24CXX_Read(uint16_t ReadAddr, uint16_t Len, uint8_t *buf)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(EE_ADD);		//����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);	//���͸ߵ�ַ
	}
	else
		IIC_Send_Byte(EE_ADD + ((ReadAddr/256)<<1));   //����������ַ0xA0,д����

	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr%256);	//���͵͵�ַ
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(EE_ADD|0x01);		//�������ģʽ
	IIC_Wait_Ack();
	while(--Len)
	{
		*(buf++)=IIC_Read_Byte(1);
	}
	*buf=IIC_Read_Byte(0);
	IIC_Stop();						//����һ��ֹͣ����
}

//����д��
void AT24CXX_Write(uint16_t ReadAddr, uint16_t Len, uint8_t *buf)
{
	uint8_t i;		//��1��Ŀ��ҳ��ʣ��Ŀռ��ֽ���
	uint8_t j;		//��Ҫ��ҳд���ҳ��
	uint8_t k;		//���һҳ��Ҫд����ֽ���
	uint8_t l;

	i = PAGE_SIZE - ReadAddr%PAGE_SIZE;			//��1��Ŀ��ҳ��ʣ��Ŀռ��ֽ���

	if(Len <= i)								//�����Ҫд������ֽ���С�ڵ�ǰPAGEʣ���ֽ���
		AT24CXX_PageWrite(ReadAddr,Len,buf);	//ֱ��ҳ��д�뼴��
	else										//������Ҫ��ҳ
	{
		AT24CXX_PageWrite(ReadAddr,i,buf);		//д���1ҳ������

		j = (Len-i)/PAGE_SIZE;					//������Ҫ��ҳд���ҳ��
		k = (Len-i)%PAGE_SIZE;					//�������һҳ��Ҫд����ֽ���

		ReadAddr += i;							//�����µĵ�ַƫ��
		buf += i;								//����ָ��ָ���µ�ƫ��

		if(j>0)									//���������ҳд���ҳ������0
		{
			for(l=0; l<j; l++)					//д����Ӧ��ҳ��
			{
				AT24CXX_PageWrite(ReadAddr,PAGE_SIZE,buf);
				ReadAddr += PAGE_SIZE;
				buf += PAGE_SIZE;
			}
		}
		if(k>0)
			AT24CXX_PageWrite(ReadAddr,k,buf);	//д�����һҳ������
	}
}










