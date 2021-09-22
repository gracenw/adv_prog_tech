/* just a helper file with some useful functions */

#include <cstring>

/* 
  function: conform
  ensures the command line argument conforms to requirements, 
  saves unsigned long in placeholder and returns false if fail.
  accepts argc, argv1, and unsigned long reference
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
        if (int(argv1[i]) < 48 || int(argv1[i]) > 57) 
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

/* 
  function: conform
  ensures the command line argument conforms to requirements, 
  saves float in placeholder and returns false if fail.
  accepts argc, argv1, float reference, and range of value
*/
bool conform(const int argc, const char * argv1, float & placeholder, int lower, int upper) 
{
    /* make sure there is a command line arg */
    if (argc < 2) 
    {
        return false;
    }

    /* check that all chars in argv[1] are numbers */
    for (int i = 0; i < strlen(argv1); i++) 
    {
        if (int(argv1[i]) < 45 || int(argv1[i]) > 57 || int(argv1[1]) == 47)
        {
            return false;
        }
    }

    /* convert to long */
    placeholder = strtof(argv1, NULL);

    /* check that long is in correct range */
    if (placeholder > lower && placeholder < upper) 
    {
        return true;
    }
    else 
    {
        return false;
    }
}