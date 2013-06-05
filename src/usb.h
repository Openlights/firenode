#ifndef _USB_H
#define _USB_H

#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <libusb.h>

#define STRAND_CONTROLLER_VID 0x03EB
#define STRAND_CONTROLLER_PID 0x1337

#define EP_OUT 0x02
#define EP_IN 0x81


//! Device driver for the USB Strand Controller
class USBStrandController : public QObject
{
    Q_OBJECT

public:
    USBStrandController();
    ~USBStrandController();

    bool connect(void);

public slots:
    void write_data(QByteArray *data);

signals:
    void data_written(); 

private:
    libusb_device_handle *_handle;
};

#endif
