#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void updateCPUTempGraph(float value);
    virtual void updateGPUTempGraph(float value);
    virtual void updateRAMUsageProgress(uint16_t value);
    virtual void updateCPUUsageProgress(uint16_t value);
    virtual void updateGPUUsageProgress(uint16_t value);
    virtual void updateSystemTime(uint8_t hr, uint8_t min, uint8_t sec);
protected:
};

#endif // SCREEN1VIEW_HPP
