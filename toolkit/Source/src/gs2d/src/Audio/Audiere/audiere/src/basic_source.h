#ifndef BASIC_SOURCE_H
#define BASIC_SOURCE_H


#include <vector>
#include <string>
#include "audiere.h"


namespace audiere {

  struct Tag {
    Tag(const std::string& k, const std::string& v, const std::string& t) {
      key = k;
      value = v;
      type = t;
    }

    std::string key;
    std::string value;
    std::string type;
  };

  /**
   * Basic implementation of a sample source including things such as
   * repeat.  BasicSource also defines the required methods for unseekable
   * sources.  Override them if you can seek.
   */
  class BasicSource : public RefImplementation<SampleSource> {
  public:
    BasicSource();

    /**
     * Manages repeating within read().  Implement doRead() in
     * implementation classes.
     */
    int ADR_CALL read(int frame_count, void* buffer);

    bool ADR_CALL isSeekable()                  { return false; }
    int  ADR_CALL getLength()                   { return 0;     }
    void ADR_CALL setPosition(int /*position*/) {               }
    int  ADR_CALL getPosition()                 { return 0;     }

    bool ADR_CALL getRepeat()                   { return m_repeat; }
    void ADR_CALL setRepeat(bool repeat)        { m_repeat = repeat; }

    int ADR_CALL getTagCount()              { return int(m_tags.size()); }
    const char* ADR_CALL getTagKey(int i)   { return m_tags[i].key.c_str(); }
    const char* ADR_CALL getTagValue(int i) { return m_tags[i].value.c_str(); }
    const char* ADR_CALL getTagType(int i)  { return m_tags[i].type.c_str(); }

    /// Implement this method in subclasses.
    virtual int doRead(int frame_count, void* buffer) = 0;

  protected:
    void addTag(const Tag& t) {
      m_tags.push_back(t);
    }

    void addTag(const std::string& k, const std::string& v, const std::string& t) {
      addTag(Tag(k, v, t));
    }

  private:
    bool m_repeat;
    std::vector<Tag> m_tags;
  };

}


#endif
