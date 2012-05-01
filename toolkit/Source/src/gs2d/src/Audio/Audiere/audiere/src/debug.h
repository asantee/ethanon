#ifndef DEBUG_H
#define DEBUG_H


#include <assert.h>
#include <stdio.h>
#include <string>


namespace audiere {

  class Log {
  public:
    static void Write(const char* str);
    static void Write(const std::string& str) { Write(str.c_str()); }
    static void IncrementIndent() { ++indent_count; }
    static void DecrementIndent() { --indent_count; }

  private:
    static void EnsureOpen();
    static void Close();

  private:
    static FILE* handle;
    static int indent_count;
  };


  class Guard {
  public:
    Guard(const char* label)
    : m_label(label) {
      Write("+");
      Log::IncrementIndent();
    }

    ~Guard() {
      Log::DecrementIndent();
      Write("-");
    }

    void Write(const char* prefix) {
      Log::Write((prefix + m_label).c_str());
    }

  private:
    std::string m_label;
  };

}


//#define ADR_FORCE_DEBUG


#if defined(ADR_FORCE_DEBUG) || defined(_DEBUG) || defined(DEBUG)

  #define ADR_GUARD(label) Guard guard_obj__(label)
  #define ADR_LOG(label)   (Log::Write(label))
  #define ADR_IF_DEBUG     if (true)

  #ifdef _MSC_VER
    #define ADR_ASSERT(condition, label) if (!(condition)) { __asm int 3 }
  #else  // assume x86 gcc
    #define ADR_ASSERT(condition, label) assert(condition && label);
  #endif

#else

  #define ADR_GUARD(label) 
  #define ADR_LOG(label)
  #define ADR_IF_DEBUG     if (false)
  #define ADR_ASSERT(condition, label)

#endif


#endif
