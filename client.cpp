#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
    
using boost::asio::ip::udp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 4)
    {
      std::cerr << "Usage: blocking_udp_echo_client <host> <port1> <port2>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    udp::socket s(io_service, udp::endpoint(udp::v4(), 0));

    udp::resolver resolver(io_service);
    udp::endpoint endpoint = *resolver.resolve({udp::v4(), argv[1], argv[2]});
    udp::endpoint endpoint2 = *resolver.resolve({udp::v4(), argv[1], argv[3]});

    std::cout << "SEND!" << std::endl;
    unsigned char request[max_length];

    bool m1 = true;
    bool m2 = false;
    bool m3 = true;
    bool m4 = false;
    size_t request_length = 2;
    std::thread platformThread([&]{
      unsigned char request[2];
      unsigned char pwma = 3;
      unsigned char pwmb = 45;
      for (int i = 0; i < 1000; ++i)
      {
        request[0] = pwma << 2 | m1 << 1 | m2;
        request[1] = pwmb << 2 | m3 << 1 | m4;
        s.send_to(boost::asio::buffer(request, request_length), endpoint);
        pwma++;
        pwmb++;
        if (pwma > 63)
        {
          pwma = 0;
        }
        if (pwmb > 63)
        {
          pwmb = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    });

    std::thread cameraThread([&]{
      unsigned char request2[2];
      request2[0] = 100;
      request2[1] = 190;
      for (int i = 0; i < 1000; ++i)
      {
        s.send_to(boost::asio::buffer(request2, request_length), endpoint2);
        request2[0]++;
        request2[1]++;
        if (request2[0] > 200)
        {
          request2[0] = 100;
        }
        if (request2[1] > 200)
        {
          request2[1] = 100;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    });
    platformThread.join();
    cameraThread.join();
    std::cout << "DONE! " << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}