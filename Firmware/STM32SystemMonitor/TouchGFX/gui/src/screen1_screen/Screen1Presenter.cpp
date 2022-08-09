#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{

}

void Screen1Presenter::deactivate()
{

}

void Screen1Presenter::updateGraph(uint16_t value)
{
	view.updateGraph(value);
}

void Screen1Presenter::updateRAMUsageProgress(uint16_t value)
{
	view.updateRAMUsageProgress(value);
}

void Screen1Presenter::updateCPUUsageProgress(uint16_t value)
{
	view.updateCPUUsageProgress(value);
}

void Screen1Presenter::updateGPUUsageProgress(uint16_t value)
{
	view.updateGPUUsageProgress(value);
}

void Screen1Presenter::updateSystemTime(uint8_t hr, uint8_t min, uint8_t sec)
{
	view.updateSystemTime(hr, min, sec);
}
