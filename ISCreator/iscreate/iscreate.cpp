#include "iscreate.hpp"
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

void iscreate::instruction_set::save(const std::string &dir) {

      rapidjson::Document doc;
      doc.SetArray();

      for (const auto &i : this->instructions) {

            rapidjson::Value inst(rapidjson::kObjectType);

            rapidjson::Value mnemonic(i.second.mnemonic.c_str(), doc.GetAllocator());
            rapidjson::Value hint(i.second.hint.c_str(), doc.GetAllocator());
            inst.AddMember("mnemonic", mnemonic, doc.GetAllocator());
            inst.AddMember("hint", hint, doc.GetAllocator());
            inst.AddMember("opcode", i.first, doc.GetAllocator());

            rapidjson::Value operands(rapidjson::kArrayType);
            for (const auto &operand : i.second.operands) {

                  rapidjson::Value operandv(rapidjson::kObjectType);

                  rapidjson::Value operand_o(operand.operand_.c_str(), doc.GetAllocator());
                  rapidjson::Value encoding_o(operand.encoding.c_str(), doc.GetAllocator());
                  rapidjson::Value size_o(operand.size.c_str(), doc.GetAllocator());
                  rapidjson::Value hint_o(operand.hint.c_str(), doc.GetAllocator());
                  rapidjson::Value kind_o(operand.kind.c_str(), doc.GetAllocator());

                  operandv.AddMember("operand", operand_o, doc.GetAllocator());
                  operandv.AddMember("encoding", encoding_o, doc.GetAllocator());
                  operandv.AddMember("size", size_o, doc.GetAllocator());
                  operandv.AddMember("hint", hint_o, doc.GetAllocator());
                  operandv.AddMember("kind", kind_o, doc.GetAllocator());

                  operands.PushBack(operandv, doc.GetAllocator());
            }

            inst.AddMember("operands", operands, doc.GetAllocator());
            doc.PushBack(inst, doc.GetAllocator());
      }

      rapidjson::StringBuffer buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      doc.Accept(writer);

      std::ofstream out(dir);
      if (!out.is_open()) {
            std::cerr << "Failed to open " + dir + "." << std::endl;
            return;
      }
      out << buffer.GetString() << std::endl;
      out.close();

      return;
}

void iscreate::instruction_set::load(const std::string &dir) {

      std::ifstream in(dir);
      if (!in.is_open()) {
            std::cerr << "Failed to open " + dir + "." << std::endl;
            return;
      }

      std::string json = "";
      std::string line = "";
      while (std::getline(in, line)) {
            json += line;
      }
      in.close();

      rapidjson::Document document;
      document.Parse(json.c_str());
      if (!document.IsArray()) {
            std::cerr << "Invalid JSON format. Expected an array." << std::endl;
            return;
      }

      for (auto i = 0u; i < document.Size(); ++i) {

            const auto &inst = document[i];

            if (!inst.HasMember("mnemonic") || !inst["mnemonic"].IsString()) {
                  std::cerr << "Invalid JSON format. Expected an string for mnemonic." << std::endl;
                  return;
            }
            if (!inst.HasMember("hint") || !inst["hint"].IsString()) {
                  std::cerr << "Invalid JSON format. Expected an string for hint." << std::endl;
                  return;
            }
            if (!inst.HasMember("opcode") || !inst["opcode"].IsInt64()) {
                  std::cerr << "Invalid JSON format. Expected an int for opcode." << std::endl;
                  return;
            }

            const auto mnemonic = inst["mnemonic"].GetString();
            const auto hint = inst["hint"].GetString();
            const auto opcode = inst["opcode"].GetInt64();

            if (!inst.HasMember("operands") || !inst["operands"].IsArray()) {
                  std::cerr << "Invalid JSON format. Expected an array for operands." << std::endl;
            }

            std::vector<operand> operands;

            const auto &operand = inst["operands"];
            for (auto j = 0u; j < operand.Size(); ++j) {
                  const auto &operandf = operand[j];

                  if (!operandf.HasMember("operand") || !operandf["operand"].IsString()) {
                        std::cerr << "Invalid JSON format. Expected an string for operand." << std::endl;
                        return;
                  }
                  if (!operandf.HasMember("encoding") || !operandf["encoding"].IsString()) {
                        std::cerr << "Invalid JSON format. Expected an string for encoding." << std::endl;
                        return;
                  }
                  if (!operandf.HasMember("size") || !operandf["size"].IsString()) {
                        std::cerr << "Invalid JSON format. Expected an string for size." << std::endl;
                        return;
                  }
                  if (!operandf.HasMember("hint") || !operandf["hint"].IsString()) {
                        std::cerr << "Invalid JSON format. Expected an string for hint." << std::endl;
                        return;
                  }
                  if (!operandf.HasMember("kind") || !operandf["kind"].IsString()) {
                        std::cerr << "Invalid JSON format. Expected an string for kind." << std::endl;
                        return;
                  }

                  operands.emplace_back(create_operand(operandf["operand"].GetString(), operandf["encoding"].GetString(), operandf["size"].GetString(), operandf["hint"].GetString(), operandf["kind"].GetString()));
            }

            this->add(opcode, mnemonic, hint, operands);
      }

      return;
}
