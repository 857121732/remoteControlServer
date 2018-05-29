#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <qevent.h>
#include <qdialog.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <Qt3DInput\qmouseevent.h>
#include "Socket.h"

// enum for mouse event type
enum Mouse {
	EMPTY,				// no event
	MOVE,				// move event
	CLICK_DOUBLE,	// double click
	CLICK_LEFT,		// press left
	CLICK_RIGHT,		// press right
	RELEASE_L,			// release click left
	RELEASE_R,			// release click right
	WHEEL,				// wheel up or down
	ZOOM				// zoom in£¨·Å´ó£© or out
};
// struct for mouse event
struct sendMouse {
	Mouse		mouse;	// current event type
	int			posX;		// current pos of x
	int			posY;		// current pos of y
	double	steps;		// scroll steUps when wheel up or down

	sendMouse() {}
	sendMouse(int widthVal, int heightVal)
		:mouse(MOVE), posX(widthVal), posY(heightVal), steps(0) {}
};

class ClientWindow : public QWidget
{
	Q_OBJECT
public:
	ClientWindow(JingSocket *jingSocket, SOCKET socket);
	~ClientWindow();
public:
	void paintEvent(QPaintEvent *event);
	void timerEvent(QTimerEvent* event);
signals:
	void CloseClient(SOCKET sock);
protected:
	void mouseMoveEvent(QMouseEvent* event);
	//void touchEvent(QTouchEvent *event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
private:
	void closeEvent(QCloseEvent *event);
private:
	JingSocket	*_jingSocket;
	SOCKET		_socket;
	QPixmap		pixmap;
	Mouse			mouse;
	int				clientWidth;
	int				clientHeight;
	int				timer;
	char			*data;
};

#endif // CLIENTWINDOW_H