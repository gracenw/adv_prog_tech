/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 11/06/21

  Description: 
  Simple SFML client application to send messages from command line to a designated TCP server.
*/

#include <SFML/Network.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    /* make sure there are correct number of args*/
    if (argc != 3)
    {
        cout << "Invalid command line argument detected: incorrect number of arguments passed." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* check that all chars in argv[2] are numbers */
    for (int i = 0; i < strlen(argv[2]); i++)
    {
        if (int(argv[2][i]) < 48 || int(argv[2][i]) > 57)
        {
            cout << "Invalid command line argument detected: non-numeric values in port number." << endl;
            cout << "Please check your values and press any key to end the program!" << endl;
            cin.get();
            return EXIT_FAILURE;
        }
    }

    /* convert to unsigned long */
    unsigned long portNumber = strtoul(argv[2], NULL, 10);

    /* check that port number is in correct range */
    if (portNumber < 61000 || portNumber > 65535)
    {
        cout << "Invalid command line argument detected: port number outside accepted range." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* check validity of IP address */
    sf::IpAddress ipAddress(argv[1]);
    if (ipAddress == sf::IpAddress::None)
    {
        cout << "Invalid command line argument detected: invalid IP address." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* create client socket to server */
    sf::TcpSocket socket;
    if (socket.connect(ipAddress, portNumber) != sf::Socket::Done)
    {
        cout << "Failed to connect to the server at " << ipAddress.toString() << " on " << portNumber << "." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* loop until escape is entered */
    cout << "Please enter a message: " << endl;
    while (true)
    {
        /* prompt for input */
        char out[128];
        out[0] = 0;
        cin.getline(out, 128);

        /* check for ESC to end loop */
        if (out[0] == 27)
        {
            break;
        }

        /* send data to server */
        if (out[0])
        {
            socket.send(out, strlen(out));
            cout << "Please enter a message: " << endl;
        }
    }

    /* end connection with server */
    socket.disconnect();

    /* return successfully */
    return EXIT_SUCCESS;
}