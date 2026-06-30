#include <iostream>
#include <string>

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
      if(input.substr(5) == "echo" || input.substr(5) == "exit" || input.substr(5) == "type"){
        std::cout << input.substr(5) << " is a shell builtin" << std::endl;
      }
      else{
        std::cout << input.substr(5) << ": not found" << std::endl;
      }
    }
    else{
      std::cout << command << ": command not found" << std::endl;
    }
    
  }
}
