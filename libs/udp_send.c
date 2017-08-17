#include <sys/socket.h>

int sendUdpData(char *data){
	int i;
	int xbeeTSFd;
	struct sockaddr_in xbeeT_addr;
	
	/* ���M�A�h���X�ݒ� */
	memset(&xbeeT_addr, 0, sizeof(xbeeT_addr)); 		// xbeeT_addr�̏�����
	xbeeT_addr.sin_family = AF_INET;					// �A�h���X�t�@�~���[ AF_INET
	xbeeT_addr.sin_port = htons( 1024 );				// ���M�|�[�g�ԍ�
	xbeeT_addr.sin_addr.s_addr = (in_addr_t)192+(in_addr_t)168*256+(in_addr_t)0*256*256+(in_addr_t)255*256*256*256;		// ���MIP�A�h���X
	/* �\�P�b�g���� */
	xbeeTSFd = socket(AF_INET, SOCK_DGRAM, 0);	// ���M�p�\�P�b�g�̐���
//	printf("IP(TX)=%s\n", inet_ntoa( xbeeT_addr.sin_addr ) );

	i=sendto(xbeeTSFd, data, strlen(data), 0, (struct sockaddr *)&xbeeT_addr, sizeof(xbeeT_addr));
	if(i<=0){
		fprintf(stderr,"Failed to send UDP (%d)\n",i);
	}
	close(xbeeTSFd);
	usleep( 1000ul );
	return i;
}
