/****************************************************************************
��������: str_to_hex
��������: �ַ���ת��Ϊʮ������
�������: string �ַ��� cbuf ʮ������ len �ַ����ĳ��ȡ�
�������: ��
*****************************************************************************/ 
static int Str2Hex(char *string, unsigned char *cbuf, int len)
{
	BYTE high, low;
	int idx, ii=0;
	for (idx=0; idx<len; idx+=2) 
	{
		high = string[idx];
		low = string[idx+1];
		
		if(high>='0' && high<='9')
			high = high-'0';
		else if(high>='A' && high<='F')
			high = high - 'A' + 10;
		else if(high>='a' && high<='f')
			high = high - 'a' + 10;
		else
			return -1;
		
		if(low>='0' && low<='9')
			low = low-'0';
		else if(low>='A' && low<='F')
			low = low - 'A' + 10;
		else if(low>='a' && low<='f')
			low = low - 'a' + 10;
		else
			return -1;
		
		cbuf[ii++] = high<<4 | low;
	}
	return 0;
}

/****************************************************************************
��������: hex_to_str
��������: ʮ������ת�ַ���
�������: ptr �ַ��� buf ʮ������ len ʮ�������ַ����ĳ��ȡ�
�������: ��
*****************************************************************************/ 
static void Hex2Str(char *ptr,unsigned char *buf,int len)
{
	for(int i = 0; i < len; i++)
	{
		sprintf(ptr, "%02x",buf[i]);
		ptr += 2;
	}
}