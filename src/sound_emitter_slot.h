#include "sound_instance_id.h"

#include <list>


struct sound_emitter_slot
{
    uint8_t m_padding[32];                              // field_0 to field_1F
    std::list<sound_instance_slot*> m_instanceList;    // field_20 - list of attached instances
    uint16_t m_generation;                              // field_40 - generation counter
};

extern sound_emitter_slot*& s_sound_emitter_slots;