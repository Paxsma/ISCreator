#include "iscreate/iscreate.hpp"
#include <iostream>

std::int32_t main() {

      /* Create instruction set with the name: "example". */
      iscreate::instruction_set is("example");

      /* Add an example instruction "nop" */
      is.add<0>("nop", "Nothing operation");

      /* Add an example instruction "move" */
      is.add(1, "move", "Move an integer to an register.", {iscreate::create_operand("Dest", "Mod", "8-Bits", "Register", "register"), iscreate::create_operand("Source", "Mod", "8-Bits", "Integer", "integer")});

      /* Save instruction set to "example.json" */
      is.save("example.json");

      /* Clear to load instruction set from example json. */
      is.clear();

      /* Load instruction set from "example.json" */
      is.load("example.json");

      /* Print opcodes */
      std::cout << "Enumeral: \n"
                << is.represent_enum_opcodes() << std::endl;
      std::cout << "Enumeral opencodings: \n"
                << is.represent_enum_opencodings() << std::endl;
      std::cout << "Enumeral opkinds: \n"
                << is.represent_enum_opkinds() << std::endl;
      std::cout << "\n\nEncoding: \n"
                << is.represent_opencodings() << std::endl;
      std::cout << "\n\nKind: \n"
                << is.represent_opkinds() << std::endl;
      std::cout << "\n\Descriptor: \n"
                << is.represent_opdescriptors() << std::endl;

      std::cin.get();

      return 0;
}
