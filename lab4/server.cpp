/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 

  Description: 
  
*/

#include <SFML/Network.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    /* make sure there are correct number of args*/
    if (argc != 2)
    {
        cout << "Invalid command line argument detected: incorrect number of arguments passed." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* check that all chars in argv[2] are numbers */
    for (int i = 0; i < strlen(argv[1]); i++)
    {
        if (int(argv[1][i]) < 48 || int(argv[1][i]) > 57)
        {
            cout << "Invalid command line argument detected: non-numeric values in port number." << endl;
            cout << "Please check your values and press any key to end the program!" << endl;
            cin.get();
            return EXIT_FAILURE;
        }
    }

    /* convert to unsigned long */
    unsigned long port_number = strtoul(argv[1], NULL, 10);

    /* check that port number is in correct range */
    if (port_number < 61000 || port_number > 65535)
    {
        cout << "Invalid command line argument detected: port number outside accepted range." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* bind tcp listener to port */
    sf::TcpListener listener;
    if (listener.listen(port_number) != sf::Socket::Done)
    {
        cout << "Failed to bind to port at " << port_number << "." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }
    cout << "Begin TCP listening port" << endl;

    /* establish vector of sockets for multiple clients */
    vector<sf::TcpSocket *> sockets;

    /* open server.log for writing */
    ofstream ofs("server.log", ofstream::app);

    /* loop until escape is entered */
    while (true)
    {
        // /* read input */
        // char cmd = cin.get();

        // /* check for ESC to end loop */
        // if (cmd == 27)
        // {
        //     cout << "Ending TCP listening port" << endl;
        //     break;
        // }

        /* accept new connection */
        sf::TcpSocket client;
        if (listener.accept(client) == sf::Socket::Done)
        {
            /* log new connection */
            ofs << "Client at " << client.getRemoteAddress() << " (port " << client.getRemotePort() << ") connected." << endl;
            sockets.push_back(&client);
        }

        /* check all established connections for incoming bytes or if they disconnected */
        cout << "Checking established connections" << endl;
        for (auto it = begin(sockets); it != end(sockets); it++)
        {
            /* check for disconnect */
            if ((**it).getRemoteAddress() == sf::IpAddress::None)
            {
                /* logs disconnect */
                ofs << "Client at " << (**it).getRemoteAddress() << " (port " << (**it).getRemotePort() << ") disconnected." << endl;
                sockets.erase(it);
            }

            /* accept incoming bytes from client */
            char in[128];
            std::size_t rcvd;
            if ((**it).receive(in, strlen(in), rcvd) == sf::Socket::Done)
            {
                ofs << (**it).getRemoteAddress() << " (" << (**it).getRemotePort() << "): " << (*in) << endl;
            }
        }
    }

    /* close log file */
    ofs.close();

    /* return successfully */
    return EXIT_SUCCESS;
}