#include "../include/core.h"
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

// Global file descriptor to keep the hardware connection open
int serial_fd = -1; 

int init_serial(const char* portname, int baudrate) {
    // Open the raw hardware file (e.g., /dev/ttyUSB0 or /dev/ttyACM0)
    serial_fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd < 0) {
        printf("[NODE ERROR] Could not open %s. Is it plugged in?\n", portname);
        return -1;
    }

    struct termios tty;
    if (tcgetattr(serial_fd, &tty) != 0) {
        printf("[NODE ERROR] Hardware configuration failed.\n");
        return -1;
    }

    // Dynamically assign the communication speed
    speed_t speed;
    switch (baudrate) {
        case 9600: speed = B9600; break;       // Standard for basic Arduinos
        case 115200: speed = B115200; break;   // Standard for ESP32, Pico, STM32
        default: speed = B115200; 
    }
    
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // 8N1 Protocol: 8 bits, No parity, 1 stop bit (Universal standard)
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS; // Disable hardware flow control

    // Disable all weird terminal echo and signal processing
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 5; // 0.5-second read timeout

    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        printf("[NODE ERROR] Failed to apply hardware settings.\n");
        return -1;
    }

    printf("[NODE SYSTEM] Linked to hardware node at %s (Baud: %d)\n", portname, baudrate);
    return 0;
}

void send_serial(const char* message) {
    if (serial_fd != -1) {
        write(serial_fd, message, strlen(message));
        write(serial_fd, "\n", 1); // Send newline to trigger MCU read
        printf("[NODE TX] Transmitted: %s\n", message);
    } else {
        printf("[NODE ERROR] No hardware node connected.\n");
    }
}
