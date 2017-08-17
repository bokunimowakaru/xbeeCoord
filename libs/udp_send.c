#include <sys/socket.h>

int sendUdpData(char *data){
	int i;
	int xbeeTSFd;
	struct sockaddr_in xbeeT_addr;
	
	/* 送信アドレス設定 */
	memset(&xbeeT_addr, 0, sizeof(xbeeT_addr)); 		// xbeeT_addrの初期化
	xbeeT_addr.sin_family = AF_INET;					// アドレスファミリー AF_INET
	xbeeT_addr.sin_port = htons( 1024 );				// 送信ポート番号
	xbeeT_addr.sin_addr.s_addr = (in_addr_t)192+(in_addr_t)168*256+(in_addr_t)0*256*256+(in_addr_t)255*256*256*256;		// 送信IPアドレス
	/* ソケット生成 */
	xbeeTSFd = socket(AF_INET, SOCK_DGRAM, 0);	// 送信用ソケットの生成
//	printf("IP(TX)=%s\n", inet_ntoa( xbeeT_addr.sin_addr ) );

	i=sendto(xbeeTSFd, data, strlen(data), 0, (struct sockaddr *)&xbeeT_addr, sizeof(xbeeT_addr));
	if(i<=0){
		fprintf(stderr,"Failed to send UDP (%d)\n",i);
	}
	close(xbeeTSFd);
	usleep( 1000ul );
	return i;
}
