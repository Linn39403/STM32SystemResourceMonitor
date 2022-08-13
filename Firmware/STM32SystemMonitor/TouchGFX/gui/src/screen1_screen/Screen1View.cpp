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

colortype ConvertGadgeColorBasedonPercent(uint16_t value)
{
	if(value < 50 )
	{
		return Color::getColorFromRGB(40, 205, 65); //green
	}
	else if( value < 70 )
	{
		return Color::getColorFromRGB(255, 149, 0); //orange
	}
	else
	{
		return Color::getColorFromRGB(255, 39, 38); //red
	}
}

void Screen1View::updateRAMUsageProgress(uint16_t value)
{
	RAMUsageProgressPainter.setColor(ConvertGadgeColorBasedonPercent(value));
	RAMUsageProgress.setPainter(RAMUsageProgressPainter);
	RAMUsageProgress.setValue(value);  
	RAMUsageProgress.invalidate();
	RAMUsageTxt.setColor(ConvertGadgeColorBasedonPercent(value));
	Unicode::snprintf(RAMUsageTxtBuffer, RAMUSAGETXT_SIZE, "%u", value);
	RAMUsageTxt.invalidate();
}

void Screen1View::updateCPUUsageProgress(uint16_t value)
{
	CPUUsageProgressPainter.setColor(ConvertGadgeColorBasedonPercent(value));
	CPUUsageProgress.setPainter(CPUUsageProgressPainter);
	CPUUsageProgress.setValue(value);
	CPUUsageProgress.invalidate();
	CPUUsageTxt.setColor(ConvertGadgeColorBasedonPercent(value));
	Unicode::snprintf(CPUUsageTxtBuffer, CPUUSAGETXT_SIZE, "%u", value);	
	CPUUsageTxt.invalidate();
}

void Screen1View::updateGPUUsageProgress(uint16_t value)
{
	GPUUsageProgressPainter.setColor(ConvertGadgeColorBasedonPercent(value));
	GPUUsageProgress.setPainter(GPUUsageProgressPainter);
	GPUUsageProgress.setValue(value);
	GPUUsageProgress.invalidate();
	GPUUsageTxt.setColor(ConvertGadgeColorBasedonPercent(value));
	Unicode::snprintf(GPUUsageTxtBuffer, GPUUSAGETXT_SIZE, "%u", value);	
	GPUUsageTxt.invalidate();
}

void Screen1View::updateSystemTime(uint8_t hr, uint8_t min, uint8_t sec)
{
	digitalClock1.setTime24Hour(hr, min, sec);
}

