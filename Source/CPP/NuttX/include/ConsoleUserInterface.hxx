#ifndef __CONSOLE_USER_INTERFACE_HXX
#define __CONSOLE_USER_INTERFACE_HXX

#include "UserInterface.hxx"

class ConsoleUserInterface : public UserInterface
{
    public:
        /**
         * @brief Construct a new ConsoleUserInterface object
         */
        ConsoleUserInterface();

        /**
         * @brief Destroy the ConsoleUserInterface object
         */
        ~ConsoleUserInterface();

        /**
         * @brief Display the contents of the store lines on the console.
         * 
         * @param storeLines Store lines to be displayed.
         */
        void UpdateDisplayTube(StoreLines &storeLines) const override;

        /**
         * @brief Show the number of lines executed on the console.
         * 
         * @param number_of_lines Number of line executed.
         */
        void UpdateProgress(uint number_of_lines) override;

        /**
         * @brief Present an error message to the console.
         * 
         * @param error Message to display.
         */
        void DisplayError(const char *error) override;

    private:
        
};

#endif // __CONSOLE_USER_INTERFACE_HXX