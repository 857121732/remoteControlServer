#pragma execution_character_set("utf-8")
#include "clientWindow.h"

ClientWindow::ClientWindow(JingSocket *jingSocket, SOCKET socket)
: _jingSocket(jingSocket), _socket(socket)
, data(new char[999999]), mouse(MOVE)
{
	_jingSocket->RecvData(&clientWidth, sizeof(clientWidth), _socket);
	_jingSocket->RecvData(&clientHeight, sizeof(clientHeight), _socket);
	timer = startTimer(400);
	setMouseTracking(true);
}
ClientWindow::~ClientWindow(){
	delete[] data;
}

void ClientWindow::paintEvent(QPaintEvent *event){
	Q_UNUSED(event);
	QPainter		painter(this);
	QPixmap p;
	p.load("test.jpg");
	painter.drawPixmap(rect(), p);
}
void ClientWindow::timerEvent(QTimerEvent* event){
	int size, ret;
	ret = _jingSocket->RecvData(&size, sizeof(size), _socket);
	if (ret == 0){
		QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("客户端已断开连接，将关闭此窗口。"));
		close();
		return;
	}
	if (ret < 0){
		QMessageBox::information(this, QString::fromUtf8("警告"), "接收数据出错！错误码为：" + QString::number(ret) + "。将断开连接并关闭此窗口。");
		close();
		return;
	}
	if (size <= 999999 && size > 0){
		ret = _jingSocket->RecvData(data, size, _socket);
		if (ret != size){
			if (ret == 0){
				SysError() << "客户端已断开连接。" << endl;
				QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("客户端已断开连接，将关闭此窗口。"));
			}
			else{
				SysError() << "接收数据失败！应接收数据大小：" << size << "\t接收返回值为：" << ret << endl;
				QMessageBox::information(this, QString::fromUtf8("警告"), QString::fromUtf8("连接出错！将断开连接并关闭此窗口。"));
			}
			close();
			return;
		}
		SysDebug() << "接收图片大小：" << size << endl;
		QByteArray ba(data, size);
		QPixmap pixmap;
		pixmap.loadFromData(ba, "jpg");
		pixmap.save("test.jpg", "jpg");
		this->update();
	}
}
void ClientWindow::closeEvent(QCloseEvent *event){
	emit CloseClient(_socket);
	killTimer(timer);
	delete[] data;
}
// Event of press mouse  
void ClientWindow::mousePressEvent(QMouseEvent *event)
{
	sendMouse sdMouse(clientWidth * event->x() / this->width(), clientHeight * event->y() / this->height());
	sdMouse.mouse = CLICK_LEFT;
	if (Qt::RightButton == event->button())
		sdMouse.mouse = CLICK_RIGHT;
	mouse = sdMouse.mouse;
	_jingSocket->SendData(&sdMouse, sizeof(sdMouse), _socket);
}
// /Event of release mouse
void ClientWindow::mouseReleaseEvent(QMouseEvent *event)
{
	sendMouse sdMouse(clientWidth * event->x() / this->width(), clientHeight * event->y() / this->height());
	if (mouse == CLICK_RIGHT)
		sdMouse.mouse = RELEASE_R;
	else
		sdMouse.mouse = RELEASE_L;
	mouse = sdMouse.mouse;
	_jingSocket->SendData(&sdMouse, sizeof(sdMouse), _socket);
}
// Event of double click
void ClientWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
	sendMouse sdMouse(clientWidth * event->x() / this->width(), clientHeight * event->y() / this->height());
	sdMouse.mouse = CLICK_DOUBLE;
	mouse = sdMouse.mouse;
	_jingSocket->SendData(&sdMouse, sizeof(sdMouse), _socket);
}
// Event of move mouse 
void ClientWindow::mouseMoveEvent(QMouseEvent *event)
{
	sendMouse sdMouse(clientWidth * event->x() / this->width(), clientHeight * event->y() / this->height());
	mouse = sdMouse.mouse = MOVE;
	_jingSocket->SendData(&sdMouse, sizeof(sdMouse), _socket);
}
// Event of wheel
void ClientWindow::wheelEvent(QWheelEvent* event) {
	sendMouse sdMouse;
	sdMouse.steps = event->delta();
	mouse = sdMouse.mouse = WHEEL;
	sdMouse.posX = clientWidth * event->x() / this->width();
	sdMouse.posY = clientHeight * event->y() / this->height();
	_jingSocket->SendData(&sdMouse, sizeof(sdMouse), _socket);
}

