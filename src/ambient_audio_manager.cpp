#include "ambient_audio_manager.h"

#include "func_wrapper.h"

void ambient_audio_manager::create_inst() {

    void (__fastcall *func)(void *, void *) = (decltype(func)) 0x0053EC10;
    func(0, nullptr);
}

void ambient_audio_manager::delete_inst()
{
    void (__fastcall *func)(void *, void *) = (decltype(func)) 0x00552800;
    func(0, nullptr);
}

void ambient_audio_manager::frame_advance(Float a1) {

    void (__fastcall *func)(void *, void * Float ) = (decltype(func)) 0x00559380;
    func(0, nullptr);
}

void ambient_audio_manager::reset() {
    void (__fastcall *func)(void *, void *) = (decltype(func)) 0x0054DF90;
    func(0, nullptr);
}
