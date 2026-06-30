#include <iostream>
#include <string>
#include <unordered_set>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;


  while(true){
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);


    std::string command = input.substr(0, input.find(" "));


    if(command == "exit"){
      break;
    }
    else if(command == "echo"){
      std::cout << input.substr(5) << std::endl;
    }
    else if(command == "type"){
      std::string type_check = input.substr(5);
      std::unordered_set<std::string> commands = {
        "echo",
        "exit",
        "pwd",
        "type"
      };

      if(commands.contains(type_check)){
        std::cout << type_check << " is a shell builtin" << std::endl;
      }
      else{
        std::cout << tyep_check << ": not found" << std::endl;
      }
    }
    else{
      std::cout << command << ": command not found" << std::endl;
    }
    
  }
}
