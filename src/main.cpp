#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>


void typeCommand(std::string input){
  std::string type_check = input.substr(5);
  std::unordered_set<std::string> commands = {
    "echo",
    "exit",
    "type"
  };
  bool found = false;

  if(commands.contains(type_check)){
    found = true;
    std::cout << type_check << " is a shell builtin" << std::endl;
    break;
  }
  if(!found){
    std::string path = getenv("PATH");
    std::istringstream ss(path);
    std::string dictionary;

    while(getline(ss, dictionary, ':')){
      std::string full_path = dictionary + "/" + type_check;

      if(!access(full_path.c_str(), X_OK)){
        found = true;
        std::cout << type_check << " is" << full_path << std::endl;
        return;
      }
    }
    std::cout << type_check << ": not found" << std::endl;

    return;
  }

}


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
      typeCommand(input);
    }
    else{
      std::cout << command << ": command not found" << std::endl;
    }
    
  }
}
