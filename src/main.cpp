

#include "./ShaderCPoll.h"
#include "./Utils.h"

using namespace SCP;

int main(int argc, char** argv) {
  std::shared_ptr<ShaderCPoll> p = std::make_shared<ShaderCPoll>();

  try {
    Utils::_exeLocation = argv[0];
    std::vector<std::string> args;
    for (auto i = 1; i < argc; ++i) {
      args.push_back(std::string(argv[i]));
    }
    p->init(args);
  }
  catch (std::exception& ex) {
    Utils::logError(Stz "Exception: " + ex.what());
  }

  try {
    p->run();
  }
  catch (std::exception& ex) {
    Utils::logError(Stz "Error: " + ex.what());
    std::cin.get(); 
  }


  return 0;
}