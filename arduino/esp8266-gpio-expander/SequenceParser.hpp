#ifndef SEQUENCE_PARSER_H
#define SEQUENCE_PARSER_H

#include <iostream>
#include <cstring>

#define OUTPUTS_BUFFER_SIZE 128

namespace gpio_expander {

struct SequenceStep_t {
  uint8_t outputs[OUTPUTS_BUFFER_SIZE];
  uint32_t offset;
  uint32_t duration;
  uint8_t reps;
};

enum class ParserStatus {
  OK,
  END,
  ERROR,
};

class SequenceParser {
public:
  SequenceParser() {}

  void init(const char* sequence);
  ParserStatus parseNextStep(SequenceStep_t* step);

private:
  const char* m_sequence;
};

} // namespace gpio_expander

#endif  // SEQUENCE_PARSER_H
