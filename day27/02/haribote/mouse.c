#include "bootpack.h"

/* 处理鼠标中断 */
struct FIFO32 *mousefifo;
int mousedata0; // 存储要添加的基本数据

void inthandler2c(int *esp) {
	int data;
	io_out8(PIC1_OCW2, 0x64); //	通知PIC1(从中断控制器) IRQ-12的受理已经完成
	io_out8(PIC0_OCW2, 0x62);	//	通知PIC0(主中断控制器) IRQ-02的受理已经完成
	data = io_in8(PORT_KEYDAT); //	跟键盘一样
	fifo32_put(mousefifo, data + mousedata0);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define	MOUSECMD_ENABLE	0xf4
/* 发送激活鼠标的指令，*/
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec){
	// 将FIFO缓冲区中的信息保存到全局变量
	mousefifo = fifo;
	mousedata0 = data0;
	// 激活鼠标
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	//成功的话，键盘控制器返回ACK即0xfa
	mdec->phase = 0;
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat){

	if (mdec->phase == 0) {
		// 等待鼠标的 0xfa阶段
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		// 等待鼠标第一字节的阶段
		//用c8的原因是dat的前一位一直在0-3变化,这样能够保证不论怎么变化计算后一直为0
		if ((dat & 0xc8) == 0x08){ 
			// 如果第一字节正确
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		// 等待鼠标的第二字节阶段
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		// 等待鼠标的第三字节阶段
		mdec->buf[2] = dat;
		mdec->phase = 1;
		// 记录按键的信息,不按是8,利用0x07读取鼠标的按键状态
		mdec->btn = mdec->buf[0] & 0x07; 
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];

		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; // 鼠标的方向与画面符号相反
		return 1;
	}
	return -1;
}
