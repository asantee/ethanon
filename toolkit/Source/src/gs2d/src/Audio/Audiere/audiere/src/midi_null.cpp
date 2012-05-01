#include "audiere.h"
#include "internal.h"

namespace audiere {
  namespace hidden {

    ADR_EXPORT(MIDIDevice*) AdrOpenMIDIDevice(const char* /*name*/) {
      return 0;
    }

  }
}
