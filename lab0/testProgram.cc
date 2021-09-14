/* 
Author: Gracen Wallace
Class: ECE 6122 A
Last Date Modified: 09/07/2021

Description: simple application to print student name and use some
escape sequences.

I, Gracen Wallace, have successfully accessed my pace-ice account. I am
able to transfer files from my pace-ice account to my local machine using
an ftp client. I am ready for the next assignment on pace-ice.
*/

/* include standard c++ namespace and io library */
#include <iostream>
using namespace std;

/* 
  function: main()
  entry point into application
*/
int main() 
{
    /* print required strings */
    cout << "My name is: (Gracen Wallace)\n";
    cout << "This (\") is a double quote.\n";
    cout << "This (\') is a single quote.\n";
    cout << "This (\\) is a backslash.\n";
    cout << "This (/) is a forward slash.\n";

    /* return successfully */
    return 0;
}