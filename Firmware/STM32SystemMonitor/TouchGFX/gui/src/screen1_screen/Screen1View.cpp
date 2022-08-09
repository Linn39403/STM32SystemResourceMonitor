#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::updateGraph(uint16_t value)
{
	dynamicGraph1.addDataPoint(value);
}

void Screen1View::updateRAMUsageProgress(uint16_t value)
{
	RAMUsageProgress.setValue(value);
}

void Screen1View::updateCPUUsageProgress(uint16_t value)
{
	CPUUsageProgress.setValue(value);
}

void Screen1View::updateGPUUsageProgress(uint16_t value)
{
	GPUUsageProgress.setValue(value);
}

void Screen1View::updateSystemTime(uint8_t hr, uint8_t min, uint8_t sec)
{
	digitalClock1.setTime24Hour(hr, min, sec);
}

