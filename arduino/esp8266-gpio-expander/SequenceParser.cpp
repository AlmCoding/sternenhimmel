#include "SequenceParser.hpp"

namespace gpio_expander {

int32_t parseStep(const char* str, SequenceStep_t* step);

void SequenceParser::init(const char* sequence) {
  m_sequence = sequence;

  // Find beginning of first step
  m_sequence = strchr(m_sequence, '[');
  m_sequence++;  // Skip outer list '['
}

ParserStatus SequenceParser::parseNextStep(SequenceStep_t* step) {
  // Find beginning of next step
  m_sequence = strchr(m_sequence, '[');

  // End reached
  if (m_sequence == nullptr) {
    return ParserStatus::END;
  }

  int32_t parsedChars = parseStep(m_sequence, step);
  if (parsedChars > 0) {
    m_sequence += parsedChars;
    return ParserStatus::OK;
  } else {
    return ParserStatus::ERROR;
  }
}

int32_t parseStep(const char* str, SequenceStep_t* step) {
  int32_t parsedChars = -1;

  // Find the start of the outputs list
  const char* outputs_start = strchr(str, '[');
  if (!outputs_start) {
    // No sequence step found
    parsedChars = 0;
    return parsedChars;
  }
  // Go to start of outputs
  outputs_start++;

  // Find the end of the outputs list
  const char* outputs_end = strchr(outputs_start, ']');
  if (!outputs_end) {
    // List end not found
    return parsedChars;
  }

  // Parse the outputs list
  const char* p = outputs_start + 1;
  char* endptr;
  int i = 0;
  while (p < outputs_end && i < OUTPUTS_BUFFER_SIZE) {

    int value = strtol(p, &endptr, 10);
    if (endptr == p) {
      // Invalid number
      return parsedChars;
    }
    step->outputs[i++] = value;
    p = endptr;
    p++;
  }

  // Mark end
  step->outputs[i] = 0xff;
  p++;  // skip ','

  // Find the end of the step
  const char* step_end = strchr(p, ']');
  if (!step_end) {
    // List end not found
    return parsedChars;
  }

  // Parse the offset, duration and reps
  step->offset = strtol(p, &endptr, 10);
  endptr++;  // skip ','
  step->duration = strtol(endptr, &endptr, 10);
  endptr++;  // skip ','
  step->reps = strtol(endptr, &endptr, 10);
  endptr++;  // skip ']'

  parsedChars = endptr - str;
  return parsedChars;
}

}  // namespace gpio_expander
