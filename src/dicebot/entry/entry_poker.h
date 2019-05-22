#include "./entry_base.h"
namespace dicebot::entry {
    class entry_poker : public entry_base {
        std::regex filter_command;

    public:
        entry_poker();
        bool resolve_request(std::string const &message, event_info &event, std::string &response) override;
    };
} // namespace dicebot::entry