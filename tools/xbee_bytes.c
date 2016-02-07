#define DEBUG_TX
#include "../libs/xbee.c"

byte coord[]  	=	 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
byte test[] =		{0xAA,0x5A,0xCF,0x10,0x07,0x21,0x23,0x15,0x18,0xA0,0x00,0xF4,0x51,0x66,0x66};

void ir_send_new( byte *dev, byte *data, byte type, byte len){
	byte i;
	byte b[20];
	char ir_type[3][5]={"AEHA","NEC ","SIRC"};
	
	b[0]= 0x1B; b[1]='I'; b[2]='R'; b[3]=type; b[4]=len;
	for( i=0 ; i<len ; i++) b[5+i]= data[i];
	b[5+len]='\0';
	xbee_bytes(dev,b,len+5);
	
	printf("\nIR send(%s):",ir_type[type]);
	for( i=0 ; i<len ; i++){
		lcd_disp_hex( data[i] );
		printf(" ");
	}
	printf("\n");
}


int main(int argc,char **argv){
    xbee_init( 5 );                  // XBee用COMポートの初期化(引数はポート番号)
	printf("\nIR send\n");
    ir_send_new( coord, test, 0, 15);
    exit(1);
}
