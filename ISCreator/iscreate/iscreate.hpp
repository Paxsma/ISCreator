#pragma once
#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace iscreate {

      enum class language : std::uint8_t {
            cpp,
            c
      };

      struct operand {
            std::string operand_ = "";
            std::string encoding = "";
            std::string size = "";
            std::string hint = "";
            std::string kind = "";
      };

      struct instruction {
            std::string mnemonic = "";
            std::string hint = "";
            std::vector<operand> operands;
      };

      class instruction_set {

          public:
            instruction_set(const std::string &name)
                : name(name) {
            }

#pragma region add

            /* Add instruction */
            template <std::intptr_t opcode>
            void add(const std::string &mnemonic, const std::string &hint) {
                  instruction i;
                  i.mnemonic = mnemonic;
                  i.hint = hint;
                  this->instructions.insert(std::make_pair(opcode, i));
                  return;
            }

            /* Add instruction */
            void add(const std::intptr_t opcode, const std::string &mnemonic, const std::string &hint, const std::vector<operand> &operands) {
                  instruction i;
                  i.mnemonic = mnemonic;
                  i.hint = hint;
                  i.operands = operands;
                  this->instructions.insert(std::make_pair(opcode, i));
                  return;
            }

            /* Add instruction */
            template <std::intptr_t opcode>
            void add(const std::string &mnemonic, const std::string &hint, const std::vector<operand> &operands) {
                  instruction i;
                  i.mnemonic = mnemonic;
                  i.hint = hint;
                  i.operands = operands;
                  this->instructions.insert(std::make_pair(opcode, i));
                  return;
            }

            /* Add instruction */
            void add(const std::string &mnemonic, const std::string &hint, const std::vector<operand> &operands) {
                  instruction i;
                  i.mnemonic = mnemonic;
                  i.hint = hint;
                  i.operands = operands;
                  this->instructions.insert(std::make_pair(!this->instructions.empty() ? this->instructions.rbegin()->first + 1u : 0u, i));
                  return;
            }

            /* Add instruction */
            void add(const std::string &mnemonic, const std::string &hint) {
                  instruction i;
                  i.mnemonic = mnemonic;
                  i.hint = hint;
                  this->instructions.insert(std::make_pair(!this->instructions.empty() ? this->instructions.rbegin()->first + 1u : 0u, i));
                  return;
            }

#pragma endregion

#pragma region file

            /* Save instruction set to file. */
            void save(const std::string &dir);

            /* Load instruction set from file. */
            void load(const std::string &dir);

#pragma endregion

#pragma region represent

#pragma region enums

            /* Create opcode enum */
            template <language lang = language::cpp>
            std::string represent_enum_opcodes() {

                  std::string definition = "";
                  std::string footer = "";
                  std::string retn = "";

                  /* Nothing */
                  if (this->instructions.empty()) {
                        return retn;
                  }

                  /* See if every entry increases by 1. */
                  bool set = true;
                  std::size_t next = 0u;
                  if (next >= 0u) {
                        for (const auto &i : this->instructions) {
                              if (next++ != i.first) {
                                    set = false;
                                    break;
                              }
                        }
                  } else {
                        next = false;
                  }

                  /* Create definition */
                  switch (lang) {
                        case language::cpp: {
                              definition = "enum class opcodes {\n";
                              break;
                        }
                        case language::c: {
                              definition = "typedef enum {\n";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Create footer */
                  switch (lang) {
                        case language::cpp: {
                              footer = "};";
                              break;
                        }
                        case language::c: {
                              footer = "} opcodes;";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  for (const auto &inst : this->instructions) {

                        const auto last = inst.first == this->instructions.rbegin()->first;

                        /* Create hint */
                        std::stringstream stream;
                        stream << std::hex << inst.first;
                        std::string hex = stream.str();

                        auto hint = "(" + hex + ")"; /* Opcode */
                        if (!inst.second.hint.empty()) {
                              hint += " | " + inst.second.hint;
                        } /* Hint */
                        if (!inst.second.operands.empty()) {
                              hint += " | ";
                        } /* Operands */
                        for (auto idx = 0u; idx < inst.second.operands.size(); ++idx) {
                              const auto i = inst.second.operands[idx];
                              hint += "* " + i.operand_ + "(" + i.size + ")" + (idx != inst.second.operands.size() - 1u ? ", " : "");
                        }

                        /* Capatalize */
                        auto name = "   OP_" + inst.second.mnemonic;
                        std::transform(name.begin(), name.end(), name.begin(), std::toupper);

                        /* Create enum */
                        switch (lang) {
                              case language::c:
                              case language::cpp: {
                                    if (set) {
                                          retn += name + (!last ? "," : "");
                                    } else {
                                          retn += name + " = " + std::to_string(inst.first) + (!last ? "," : "");
                                    }
                                    retn += " /* " + hint + " */\n";
                                    break;
                              }
                              default: {
                                    break;
                              }
                        }
                  }

                  return definition + retn + footer;
            }

            /* Create operand encoding enum */
            template <language lang = language::cpp>
            std::string represent_enum_opencodings() {

                  std::string definition = "";
                  std::string footer = "";
                  std::string retn = "";
                  std::vector<std::string> analyzed;

                  /* Nothing */
                  if (this->instructions.empty()) {
                        return retn;
                  }

                  /* Create definition */
                  switch (lang) {
                        case language::cpp: {
                              definition = "enum class operand_encoding {\n";
                              break;
                        }
                        case language::c: {
                              definition = "typedef enum {\n";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Create footer */
                  switch (lang) {
                        case language::cpp: {
                              footer = "};";
                              break;
                        }
                        case language::c: {
                              footer = "} operand_encoding;";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Add all operand encodings to pending analysis */
                  for (const auto &inst : this->instructions)
                        for (const auto &op : inst.second.operands)
                              if (std::find(analyzed.begin(), analyzed.end(), op.encoding) == analyzed.end()) {
                                    analyzed.emplace_back(op.encoding);
                              }

                  for (const auto &enc : analyzed) {

                        const auto last = enc == analyzed.back();

                        /* Create enum */
                        switch (lang) {
                              case language::c:
                              case language::cpp: {
                                    retn += "   " + enc + (!last ? ",\n" : "\n");
                                    break;
                              }
                              default: {
                                    break;
                              }
                        }
                  }

                  return definition + retn + footer;
            }

            /* Create operand kind enum */
            template <language lang = language::cpp>
            std::string represent_enum_opkinds() {

                  std::string definition = "";
                  std::string footer = "";
                  std::string retn = "";
                  std::vector<std::string> analyzed;

                  /* Nothing */
                  if (this->instructions.empty()) {
                        return retn;
                  }

                  /* Create definition */
                  switch (lang) {
                        case language::cpp: {
                              definition = "enum class operand_kind {\n";
                              break;
                        }
                        case language::c: {
                              definition = "typedef enum {\n";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Create footer */
                  switch (lang) {
                        case language::cpp: {
                              footer = "};";
                              break;
                        }
                        case language::c: {
                              footer = "} operand_kind;";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Add all operand encodings to pending analysis */
                  for (const auto &inst : this->instructions)
                        for (const auto &op : inst.second.operands)
                              if (std::find(analyzed.begin(), analyzed.end(), op.kind) == analyzed.end()) {
                                    analyzed.emplace_back(op.kind);
                              }

                  for (const auto &k : analyzed) {

                        const auto last = k == analyzed.back();

                        /* Create enum */
                        switch (lang) {
                              case language::c:
                              case language::cpp: {
                                    retn += "   " + k + (!last ? ",\n" : "\n");
                                    break;
                              }
                              default: {
                                    break;
                              }
                        }
                  }

                  return definition + retn + footer;
            }

#pragma endregion

#pragma region arrays

            /* Create opcode encoding table */
            template <language lang = language::cpp>
            std::string represent_opencodings() {

                  std::string definition = "";
                  std::string footer = "";
                  std::string retn = "";

                  /* Nothing */
                  if (this->instructions.empty()) {
                        return retn;
                  }

                  /* See if every entry increases by 1. */
                  bool set = true;
                  std::size_t next = 0u;
                  if (next >= 0u) {
                        for (const auto &i : this->instructions) {
                              if (next++ != i.first) {
                                    set = false;
                                    break;
                              }
                        }
                  } else {
                        next = false;
                  }

                  /* Create definition */
                  switch (lang) {
                        case language::cpp: {
                              definition = "struct optable_encoding {\n\
   opcodes op;\n\
   std::vector<operand_encoding> encodings;\n\
};\n\
static std::map<opcodes, optable_encoding> opencodings = {\n";
                              break;
                        }
                        case language::c: {
                              definition = "struct optable_encoding {\n\
   opcodes op;\n\
   operand_encoding []encodings;\n\
   size_t num_encodings;\n\
};\n\
static struct optable_encoding opencodings[] = {\n";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Create footer */
                  switch (lang) {
                        case language::cpp: {
                              footer = "};";
                              break;
                        }
                        case language::c: {
                              footer = "};";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  for (const auto &inst : this->instructions) {

                        const auto last = inst.first == this->instructions.rbegin()->first;

                        /* Create hint */
                        std::stringstream stream;
                        stream << std::hex << inst.first;
                        std::string hint = stream.str();

                        /* Capatalize */
                        auto name = "OP_" + inst.second.mnemonic;
                        std::transform(name.begin(), name.end(), name.begin(), std::toupper);

                        /* Create enum */
                        switch (lang) {
                              case language::c: { /* {??, {??, {??, ??}}} 1 */
                                    retn += "   {" + name + ", {";
                                    for (auto idx = 0u; idx < inst.second.operands.size(); ++idx) {
                                          const auto i = inst.second.operands[idx];
                                          retn += i.encoding + (idx != inst.second.operands.size() - 1u ? ", " : "");
                                    }
                                    retn += "}, " + std::to_string(inst.second.operands.size()) + std::string(!last ? "}," : "}");
                                    retn += " /* " + hint + " */\n";
                                    break;
                              }
                              case language::cpp: { /* {opcodes::??, {opcodes::??, {operand_encoding::??, operand_encoding::??}}} 1 */
                                    retn += "   {opcodes::" + name + ", {opcodes::" + name + ", {";
                                    for (auto idx = 0u; idx < inst.second.operands.size(); ++idx) {
                                          const auto i = inst.second.operands[idx];
                                          retn += "operand_encoding::" + i.encoding + (idx != inst.second.operands.size() - 1u ? ", " : "");
                                    }
                                    retn += "}}}" + std::string(!last ? "," : "");
                                    retn += " /* " + hint + " */\n";
                                    break;
                              }
                              default: {
                                    break;
                              }
                        }
                  }

                  return definition + retn + footer;
            }

            /* Create opcode kind table */
            template <language lang = language::cpp>
            std::string represent_opkinds() {

                  std::string definition = "";
                  std::string footer = "";
                  std::string retn = "";

                  /* Nothing */
                  if (this->instructions.empty()) {
                        return retn;
                  }

                  /* See if every entry increases by 1. */
                  bool set = true;
                  std::size_t next = 0u;
                  if (next >= 0u) {
                        for (const auto &i : this->instructions) {
                              if (next++ != i.first) {
                                    set = false;
                                    break;
                              }
                        }
                  } else {
                        next = false;
                  }

                  /* Create definition */
                  switch (lang) {
                        case language::cpp: {
                              definition = "struct optable_kind {\n\
   opcodes op;\n\
   std::vector<operand_kind> kinds;\n\
};\n\
static std::map<opcodes, optable_kind> opkinds = {\n";
                              break;
                        }
                        case language::c: {
                              definition = "struct optable_kind {\n\
   opcodes op;\n\
   operand_encoding []kinds;\n\
   size_t num_kinds;\n\
};\n\
static struct optable_kind opkinds[] = {\n";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Create footer */
                  switch (lang) {
                        case language::cpp: {
                              footer = "};";
                              break;
                        }
                        case language::c: {
                              footer = "};";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  for (const auto &inst : this->instructions) {

                        const auto last = inst.first == this->instructions.rbegin()->first;

                        /* Create hint */
                        std::stringstream stream;
                        stream << std::hex << inst.first;
                        std::string hint = stream.str();

                        /* Capatalize */
                        auto name = "OP_" + inst.second.mnemonic;
                        std::transform(name.begin(), name.end(), name.begin(), std::toupper);

                        /* Create enum */
                        switch (lang) {
                              case language::c: { /* {??, {??, ??}}, 1 */
                                    retn += "   {" + name + ", {";
                                    for (auto idx = 0u; idx < inst.second.operands.size(); ++idx) {
                                          const auto i = inst.second.operands[idx];
                                          retn += i.kind + (idx != inst.second.operands.size() - 1u ? ", " : "");
                                    }
                                    retn += "}, " + std::to_string(inst.second.operands.size()) + std::string(!last ? "}," : "}");
                                    retn += " /* " + hint + " */\n";
                                    break;
                              }
                              case language::cpp: { /* {opcodes::??, {opcodes::??, {operand_kind::??, operand_kind::??}}}, 1 */
                                    retn += "   {opcodes::" + name + ", {opcodes::" + name + ", {";
                                    for (auto idx = 0u; idx < inst.second.operands.size(); ++idx) {
                                          const auto i = inst.second.operands[idx];
                                          retn += "operand_kind::" + i.kind + (idx != inst.second.operands.size() - 1u ? ", " : "");
                                    }
                                    retn += "}}}" + std::string(!last ? "," : "");
                                    retn += " /* " + hint + " */\n";
                                    break;
                              }
                              default: {
                                    break;
                              }
                        }
                  }

                  return definition + retn + footer;
            }

            /* Create opcode descriptor table */
            template <language lang = language::cpp>
            std::string represent_opdescriptors() {

                  std::string definition = "";
                  std::string footer = "";
                  std::string retn = "";

                  /* Nothing */
                  if (this->instructions.empty()) {
                        return retn;
                  }

                  /* See if every entry increases by 1. */
                  bool set = true;
                  std::size_t next = 0u;
                  if (next >= 0u) {
                        for (const auto &i : this->instructions) {
                              if (next++ != i.first) {
                                    set = false;
                                    break;
                              }
                        }
                  } else {
                        next = false;
                  }

                  /* Create definition */
                  switch (lang) {
                        case language::cpp: {
                              definition = "struct optable_descriptor {\n\
   const char *const opname;\n\
   const char *const mnemonic;\n\
   const char *const hint;\n\
   std::vector<const char *> operand_encodings;\n\
};\n\
static std::map<opcodes, optable_descriptor> opdescriptor = {\n";
                              break;
                        }
                        case language::c: {
                              definition = "struct optable_descriptor {\n\
   const char *const opname;\n\
   const char *const mnemonic;\n\
   const char *const hint;\n\
   const char* []operand_encodings;\n\
   size_t num_operand_encoding;\n\
};\n\
static struct optable_descriptor opdescriptor[] = {\n";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  /* Create footer */
                  switch (lang) {
                        case language::cpp: {
                              footer = "};";
                              break;
                        }
                        case language::c: {
                              footer = "};";
                              break;
                        }
                        default: {
                              break;
                        }
                  }

                  for (const auto &inst : this->instructions) {

                        const auto last = inst.first == this->instructions.rbegin()->first;

                        /* Create hint */
                        std::stringstream stream;
                        stream << std::hex << inst.first;
                        std::string hint = stream.str();

                        /* Capatalize */
                        auto name = "OP_" + inst.second.mnemonic;
                        std::transform(name.begin(), name.end(), name.begin(), std::toupper);

                        /* Create enum */
                        switch (lang) {
                              case language::c: { /* {??, {??, ??}}, 1 */
                                    retn += "   {" + name + ", {\"" + name + "\", \"" + inst.second.mnemonic + "\", \"" + inst.second.hint + "\", {";
                                    for (auto idx = 0u; idx < inst.second.operands.size(); ++idx) {
                                          const auto i = inst.second.operands[idx];
                                          retn += "\"" + i.operand_ + "(" + i.hint + ")\"" + (idx != inst.second.operands.size() - 1u ? ", " : "");
                                    }
                                    retn += "}, " + std::to_string(inst.second.operands.size()) + std::string(!last ? "}," : "}");
                                    retn += " /* " + hint + " */\n";
                                    break;
                              }
                              case language::cpp: { /* {opcodes::??, {opcodes::??, {operand_kind::??, operand_kind::??}}}, 1 */
                                    retn += "   {opcodes::" + name + ", {\"" + name + "\", \"" + inst.second.mnemonic + "\", \"" + inst.second.hint + "\", {";
                                    for (auto idx = 0u; idx < inst.second.operands.size(); ++idx) {
                                          const auto i = inst.second.operands[idx];
                                          retn += "\"" + i.operand_ + "(" + i.hint + ")\"" + (idx != inst.second.operands.size() - 1u ? ", " : "");
                                    }
                                    retn += "}}}" + std::string(!last ? "," : "");
                                    retn += " /* " + hint + " */\n";
                                    break;
                              }
                              default: {
                                    break;
                              }
                        }
                  }

                  return definition + retn + footer;
            }

#pragma endregion

#pragma endregion

            /* Remove every instruction */
            void clear() {
                  instructions.clear();
                  return;
            }

            /* Return instruction data */
            std::map<std::intptr_t, instruction> data() {
                  return this->instructions;
            }

          private:
            std::map<std::intptr_t, instruction> instructions;
            std::string name = "";
      };

      /* Create operand */
      __inline operand create_operand(const std::string &operand_ = "", const std::string &encoding = "", const std::string &size = "", const std::string &hint = "", const std::string &kind = "") {
            operand retn;
            retn.operand_ = operand_;
            retn.hint = hint;
            retn.size = size;
            retn.encoding = encoding;
            retn.kind = kind;
            return retn;
      }

} // namespace iscreate
