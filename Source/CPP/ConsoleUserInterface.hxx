#ifndef __CONSOLE_USER_INTERFACE_HXX
#define __CONSOLE_USER_INTERFACE_HXX

#include "UserInterface.hxx"

class ConsoleUserInterface : public UserInterface
{
    public:
        ConsoleUserInterface();
        ~ConsoleUserInterface();

        void UpdateDisplayTube(StoreLines &storeLines) const override;
        void UpdateProgress(uint32_t number_of_lines) override;
        void DisplayError(const char *error) override;

    private:
        
};

#endif // __CONSOLE_USER_INTERFACE_HXX