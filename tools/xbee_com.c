/***************************************************************************************
シリアルポートにコマンドを送信し応答結果を受信します。
9600 8-N-1 NO_FLOW
コマンドは[CR]を自動的に挿入
コマンド「+++」の場合は[CR]なし
                                                Copyright (c) 2013-2016 Wataru KUNINO
                                                http://www.geocities.jp/bokunimowakaru/
***************************************************************************************/
#define RX_MAX  256
#include "../libs/96term.c"

char rx_data[RX_MAX];
int main(int argc,char **argv){
    byte port;
    int i;
    
    if(argc != 3 ){
        fprintf(stderr,"usage: %s com tx_data\t(com: B0=USB0, A0=AMA0)\n",argv[0]);
        return -1;
    }
    if( atoi(argv[1]) < 0 ){
        port = 0x9F + (byte)(-atoi(argv[1])) ;
    }else  if( ( argv[1][0]=='b' || argv[1][0]=='B' )&& argv[1][1]!='\0' ){
        port = 0xB0 + ( argv[1][1] - '0');
    }else  if( ( argv[1][0]=='a' || argv[1][0]=='A' )&& argv[1][1]!='\0' ){
        port = 0xA0 + ( argv[1][1] - '0');
    }else port = (byte)(atoi(argv[1]));
    if( sci_init(port) ==0 ){
        fprintf(stderr,"Serial Open ERROR\n");
        return -1;
    }
    if(strncmp(argv[2],"+++",3)==0){
        puts_serial_port("+++");
        sleep(1);
        usleep(100000);
    }else{
        puts_serial_port(argv[2]);
        puts_serial_port("\r");
    }
    do{
        i=gets_serial_port(rx_data,RX_MAX);
        if(i) printf("%s\n",rx_data);
    }while(i);
    close_serial_port();
    return 0;
}
