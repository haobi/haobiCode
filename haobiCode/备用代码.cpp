/****************************************************************************
函数名称: str_to_hex
函数功能: 字符串转换为十六进制
输入参数: string 字符串 cbuf 十六进制 len 字符串的长度。
输出参数: 无
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
函数名称: hex_to_str
函数功能: 十六进制转字符串
输入参数: ptr 字符串 buf 十六进制 len 十六进制字符串的长度。
输出参数: 无
*****************************************************************************/ 
static void Hex2Str(char *ptr,unsigned char *buf,int len)
{
	for(int i = 0; i < len; i++)
	{
		sprintf(ptr, "%02x",buf[i]);
		ptr += 2;
	}
}