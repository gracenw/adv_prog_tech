/* just a helper file with some useful functions */

#include <cstring>

/* 
  function: conform
  ensures the command line argument conforms to requirements, 
  saves unsigned long in placeholder and returns false if fail
*/
bool conform(const int argc, const char * argv1, unsigned long & placeholder) 
{
    /* make sure there is a command line arg */
    if (argc < 2) 
    {
        return false;
    }

    /* check that all chars in argv[1] are numbers */
    for (int i = 0; i < strlen(argv1); i++) 
    {
        if (isalpha(argv1[i])) 
        {
            return false;
        }
    }

    /* convert to unsigned long */
    placeholder = strtoul(argv1, NULL, 10);

    /* check that unsigned long is not zero or negative */
    if (placeholder <= 0) 
    {
        return false;
    }

    return true;
}