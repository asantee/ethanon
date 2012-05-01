#include <vector>
#include "audiere.h"
#include "debug.h"
#include "internal.h"
#include "utility.h"


namespace audiere {

  struct LoopPoint {
    int location;
    int target;
    int loopCount;
    int originalLoopCount;

    bool operator<(const LoopPoint& rhs) const {
      return location < rhs.location;
    }
  };

  class LoopPointSourceImpl : public RefImplementation<LoopPointSource> {
  public:
    LoopPointSourceImpl(SampleSource* source) {
      source->reset();
      m_source = source;
      m_length = m_source->getLength();

      m_frame_size = GetFrameSize(source);
    }


    // LoopPointSource implementation

    void ADR_CALL addLoopPoint(int location, int target, int loopCount) {
      LoopPoint lp;
      lp.location          = clamp(0, location, m_length);
      lp.target            = clamp(0, target,   m_length);
      lp.loopCount         = loopCount;
      lp.originalLoopCount = lp.loopCount;

      for (size_t i = 0; i < m_loop_points.size(); ++i) {
        if (m_loop_points[i].location == location) {
          m_loop_points[i] = lp;
          return;
        }
      }

      m_loop_points.push_back(lp);
      size_t idx = m_loop_points.size() - 1;
      while (idx > 0 && m_loop_points[idx] < m_loop_points[idx - 1]) {
        std::swap(m_loop_points[idx], m_loop_points[idx - 1]);
        --idx;
      }
    }

    void ADR_CALL removeLoopPoint(int index) {
      m_loop_points.erase(m_loop_points.begin() + index);
    }

    int ADR_CALL getLoopPointCount() {
      return static_cast<int>(m_loop_points.size());
    }

    bool ADR_CALL getLoopPoint(
      int index, int& location, int& target, int& loopCount)
    {
      if (index < 0 || index >= getLoopPointCount()) {
        return false;
      }

      location  = m_loop_points[index].location;
      target    = m_loop_points[index].target;
      loopCount = m_loop_points[index].originalLoopCount;
      return true;
    }

    
    // SampleSource implementation

    void ADR_CALL getFormat(
      int& channel_count, int& sample_rate, SampleFormat& sample_format)
    {
      m_source->getFormat(channel_count, sample_rate, sample_format);
    }


    int ADR_CALL read(int fc, void* buffer) {
      const int frame_count = fc;

      // not repeating?  then ignore loop points.
      if (!m_source->getRepeat()) {
        return m_source->read(frame_count, buffer);
      }

      int frames_read = 0;
      int frames_left = frame_count;
      u8* out = (u8*)buffer;

      while (frames_left > 0) {
        int position = m_source->getPosition();
        int next_point_idx = getNextLoopPoint(position);
        int next_point = (next_point_idx == -1
                            ? m_length
                            : m_loop_points[next_point_idx].location);
        int to_read = std::min(frames_left, next_point - position);
        ADR_ASSERT(to_read >= 0, "How can we read a negative number of frames?");

        int read = m_source->read(to_read, out);
        out += read * m_frame_size;
        frames_read += read;
        frames_left -= read;

        if (read != to_read) {
          return frames_read;
        }

        if (position + read == next_point) {
          if (next_point_idx == -1) {
            m_source->setPosition(0);
          } else {
            LoopPoint& lp = m_loop_points[next_point_idx];

            bool doloop = (lp.originalLoopCount <= 0 || lp.loopCount > 0);
            if (doloop && lp.originalLoopCount > 0) {
              --lp.loopCount;
            }

            if (doloop) {
              if (lp.target == lp.location) {
                return frames_read;
              }
              m_source->setPosition(lp.target);
            }
          }
        }
      }

      return frames_read;
    }

    int getNextLoopPoint(int position) {
      for (size_t i = 0; i < m_loop_points.size(); ++i) {
        if (position < m_loop_points[i].location) {
          return static_cast<int>(i);
        }
      }
      return -1;
    }


    void ADR_CALL reset() {
      for (size_t i = 0; i < m_loop_points.size(); ++i) {
        m_loop_points[i].loopCount = m_loop_points[i].originalLoopCount;
      }

      m_source->reset();
    }


    bool ADR_CALL isSeekable() {
      // must be seekable, otherwise this class wouldn't even be instantiated
      return true;
    }

    int ADR_CALL getLength() {
      return m_length;
    }

    void ADR_CALL setPosition(int position) {
      m_source->setPosition(position);
    }

    int ADR_CALL getPosition() {
      return m_source->getPosition();
    }

    bool ADR_CALL getRepeat() {
      return m_source->getRepeat();
    }

    void ADR_CALL setRepeat(bool repeat) {
      m_source->setRepeat(repeat);
    }

    int ADR_CALL getTagCount()              { return m_source->getTagCount();  }
    const char* ADR_CALL getTagKey(int i)   { return m_source->getTagKey(i);   }
    const char* ADR_CALL getTagValue(int i) { return m_source->getTagValue(i); }
    const char* ADR_CALL getTagType(int i)  { return m_source->getTagType(i);  }


  private:
    SampleSourcePtr m_source;
    int m_length;
    int m_frame_size;

    std::vector<LoopPoint> m_loop_points;
  };


  ADR_EXPORT(LoopPointSource*) AdrCreateLoopPointSource(
    SampleSource* source)
  {
    if (!source || !source->isSeekable()) {
      return 0;
    }

    return new LoopPointSourceImpl(source);
  }
  
}
