// Frame locking implementation for controlling game speed
#include "os_developer_options.h"
#include "debug_menu.h"

// Alternative implementation with slow motion support
class SlowMotionManager 
{
private:
    static constexpr float NORMAL_FRAMERATE = 60.0f;  // Normal game runs at 60 FPS
    static constexpr float SLOWMO_FRAMERATE = 120.0f; // Slow motion at 120 FPS (half speed)
    
    bool slow_motion_enabled;
    float time_scale;
    int original_frame_lock;
    
public:
    SlowMotionManager() 
        : slow_motion_enabled(false)
        , time_scale(1.0f)
        , original_frame_lock(0) 
    {
    }
    
    // Enable or disable slow motion
    void set_slow_motion(bool enabled) 
    {
        if (enabled && !slow_motion_enabled) {
            // Entering slow motion
            original_frame_lock = os_developer_options::instance->get_int(mString{"FRAME_LOCK"});
            os_developer_options::instance->set_int(mString{"FRAME_LOCK"}, static_cast<int>(SLOWMO_FRAMERATE));
            time_scale = 0.5f; // Half speed
            slow_motion_enabled = true;
        }
        else if (!enabled && slow_motion_enabled) {
            // Exiting slow motion
            os_developer_options::instance->set_int(mString{"FRAME_LOCK"}, original_frame_lock);
            time_scale = 1.0f; // Normal speed
            slow_motion_enabled = false;
        }
    }
    
    // Toggle slow motion on/off
    void toggle_slow_motion() 
    {
        set_slow_motion(!slow_motion_enabled);
    }
    
    // Get current time scale for physics/animation updates
    float get_time_scale() const 
    {
        return time_scale;
    }
    
    bool is_slow_motion_active() const 
    {
        return slow_motion_enabled;
    }
    
    // Apply time scaling to a delta time value
    float apply_time_scale(float delta_time) const 
    {
        return delta_time * time_scale;
    }
};

// Enhanced version of handle_frame_locking with slow motion support

// Usage example in the game loop

// Integration with the debug menu handler (from case 2 in game_flags_handler)
void handle_slow_motion_debug_menu(debug_menu_entry *entry) 
{
    static int old_frame_lock = 0;
    static SlowMotionManager slowmo_mgr;
    
    bool enabled = entry->get_bval();
    
    if (enabled) {
        // Store original frame lock value
        old_frame_lock = os_developer_options::instance->get_int(mString{"FRAME_LOCK"});
        // Set to 120 FPS for smooth slow motion (half speed)
        os_developer_options::instance->set_int(mString{"FRAME_LOCK"}, 120);
        slowmo_mgr.set_slow_motion(true);
    } else {
        // Restore original frame lock value
        os_developer_options::instance->set_int(mString{"FRAME_LOCK"}, old_frame_lock);
        slowmo_mgr.set_slow_motion(false);
    }
    
    debug_menu::hide();
}