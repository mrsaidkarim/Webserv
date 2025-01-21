// 
#include "ParseConfigFile/ParserConfig.hpp"
#include "WebServ.hpp"
#include "Configuration/Location.hpp"
#include "Response/HttpResponse.hpp"
#include "Configuration/Server.hpp"
#include "Request/HttpRequest.hpp"
#include "const.hpp"

int main(int ac, char** av) {
    signal(SIGPIPE, SIG_IGN); // ignore broken pipe signal from client
    cout.setf(ios::unitbuf); // flushes the buffer after every output operation
	if (ac >= 3) {
		cerr << BOLD_YELLOW << "Error Usage: ./webserv path_to_config_file\n" << RESET;
		return (1);
	}
	WebServ web_serv;
    if (ac == 2) {
	    ParserConfig Parser(av[1], web_serv);
        if (!Parser.is_good_config())
            return (1);
    }
    else {
	    ParserConfig Parser(DEFAULT_CONFIG_FILE, web_serv);
        if (!Parser.is_good_config())
            return (1);
    }
    if (DEBUG_MODE) web_serv.print_all_servers();
    
	web_serv.run_servers();
}
