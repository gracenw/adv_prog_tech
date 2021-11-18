/* 
  Author: Gracen Wallace
  Class: ECE 6122 A
  Last Date Modified: 11/06/21

  Description: 
  Simple SFML server application that writes all connections, incoming messages, and disconnections
  to a server log file; supports multiple clients.
*/

#include <SFML/Network.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <list>

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
    unsigned long portNumber = strtoul(argv[1], NULL, 10);

    /* check that port number is in correct range */
    if (portNumber < 61000 || portNumber > 65535)
    {
        cout << "Invalid command line argument detected: port number outside accepted range." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* bind tcp listener to port */
    sf::TcpListener listener;
    if (listener.listen(portNumber) != sf::Socket::Done)
    {
        cout << "Failed to bind to port at " << portNumber << "." << endl;
        cout << "Please check your values and press any key to end the program!" << endl;
        cin.get();
        return EXIT_FAILURE;
    }

    /* establish selector for multiple clients and add listening socket */
    sf::SocketSelector selector;
    selector.add(listener);

    /* create list of all client sockets */
    list<sf::TcpSocket*> clients;

    /* open server.log for writing */
    ofstream ofs("server.log", ofstream::app);
    
    /* loop until program is ended */
    while (true)
    {
        /* wait for a socket to be ready */
        if (selector.wait())
        {
            if (selector.isReady(listener))
            {
                /* ready socket is listener, accept new client */
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done)
                {
                    /* add client to list of clients and selector, and write connection to log */
                    clients.push_back(client);
                    selector.add(*client);
                    ofs << "Client at " << (*client).getRemoteAddress() << " (port " << (*client).getRemotePort() << ") connected." << endl;
                }
            }
            else
            {
                for (list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
                {
                    /* check if client is ready to receive incoming data */
                    sf::TcpSocket& client = **it;
                    if (selector.isReady(client))
                    {
                        /* create placeholder for incoming data */
                        char in[128];
                        size_t rcvd;
                        auto status = client.receive(in, 128, rcvd);

                        /* get rid of leftover and extraneous chars in buffer */
                        for (int i = rcvd; i < 128; i++) 
                        {
                            in[i] = 0;
                        }

                        /* write message to log if reception complete */
                        if (status == sf::Socket::Done)
                        {
                            ofs << client.getRemoteAddress() << " (" << client.getRemotePort() << "): " << in << endl;
                        }
                        /* log disconnect if client is no longer connected, remove from selector and list of clients */
                        else if (status == sf::Socket::Disconnected)
                        {
                            ofs << "Client at " << (**it).getRemoteAddress() << " (port " << (**it).getRemotePort() << ") disconnected." << endl;
                            selector.remove(**it);
                            it = clients.erase(it);
                        }
                    }
                }
            }
        }
    }

    /* close log file */
    ofs.close();

    /* return successfully */
    return EXIT_SUCCESS;
}