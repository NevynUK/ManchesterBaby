#ifndef __USER_INTERFACE_HXX
#define __USER_INTERFACE_HXX

#include "StoreLines.hxx"

/**
 * @brief Abstract class to define the interface for the user interface.
 */
class UserInterface
{
    public:
        /**
         * @brief Construct a new User Interface object
         */
        UserInterface();

        /**
         * @brief Destroy the User Interface object
         */
        ~UserInterface();

        /**
         * @brief Place holder for the method that will update the display tube
         * 
         * @param storeLines Store lines to be displayed.
         */
        virtual void UpdateDisplayTube(StoreLines &storeLines) const  = 0;

        /**
         * @brief Place holder for the method that will update the progress of the application.
         * 
         * @param number_of_lines Number of lines executed so far.
         */
        virtual void UpdateProgress(uint number_of_lines) = 0;

        /**
         * @brief Place holder for the method that will display an error message to the user.
         * 
         * @param error Message to be displayed.
         */
        virtual void DisplayError(const char *error) = 0;

    private:
        
};

#endif // __USER_INTERFACE_HXX