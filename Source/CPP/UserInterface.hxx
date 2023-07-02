#ifndef __USER_INTERFACE_HXX
#define __USER_INTERFACE_HXX

#include "StoreLines.hxx"

/**
 * @brief Pure virtual class to define the interface for the user interface.
 */
class UserInterface
{
    public:
        UserInterface();
        ~UserInterface();

        virtual void UpdateDisplayTube(StoreLines &storeLines) const  = 0;
        virtual void UpdateProgress(uint32_t number_of_lines) = 0;
        virtual void DisplayError(const char *error) = 0;

    private:
        
};

#endif // __USER_INTERFACE_HXX