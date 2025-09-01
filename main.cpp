#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>

int main() {
  const char *serialPort =
      "/dev/tty.usbserial-ABADF5SD"; // Change this to your serial port (Linux:
                                     // /dev/ttyUSB0 or /dev/ttyS0)
  int fd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd == -1) {
    std::cerr << "Error opening serial port: " << strerror(errno) << std::endl;
    return 1;
  }

  // Configure the serial port
  struct termios options;
  tcgetattr(fd, &options);
  cfsetispeed(&options, 250000); // Set baud rate to 9600
  cfsetospeed(&options, 250000);
  options.c_cflag |=
      (CLOCAL | CREAD);       // Enable receiver, ignore modem control lines
  options.c_cflag &= ~PARENB; // No parity
  options.c_cflag &= ~CSTOPB; // 1 stop bit
  options.c_cflag &= ~CSIZE;  // Clear the current data size setting
  options.c_cflag |= CS8;     // 8 data bits
  options.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
  options.c_oflag &= ~OPOST;                          // Raw output
  tcsetattr(fd, TCSANOW, &options);

  // Write bytes to the serial port
  char data[513];
  for (int i = 0; i < 513; i++) {
    data[i] = 0;
  }
  for (int i = 1; i < 8; i++) {
    data[i] = 0xFF;
  }

  while (true) {
    ssize_t bytesWritten = write(fd, data, sizeof(data));

    if (bytesWritten < 0) {
      std::cerr << "Error writing to serial port: " << strerror(errno)
                << std::endl;
      close(fd);
      return 1;
    }

    std::cout << "Wrote " << bytesWritten << " bytes to the serial port."
              << std::endl;
    usleep(100 * 1000);
  }

  // Close the serial port
  close(fd);
  return 0;
}
