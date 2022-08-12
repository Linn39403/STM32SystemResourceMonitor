#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Color.hpp>

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

void Screen1View::updateCPUTempGraph(float value)
{
	CPUTempdynamicGraph.addDataPoint(value);
}

void Screen1View::updateGPUTempGraph(float value)
{
	GPUTempdynamicGraph.addDataPoint(value);
}

void Screen1View::updateRAMUsageProgress(uint16_t value)
{
	if( value > 30 )
	{
		RAMUsageProgressPainter.setColor(Color::getColorFromRGB(0, 222, 113));
	}
	RAMUsageProgress.setPainter(RAMUsageProgressPainter);
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

