#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <stdlib.h>
#include "main.h"

extern struct systemInfo_s SystemInfo;
Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	//modelListener->updateGraph(rand()%100);
	//modelListener->RAMUsageProgressPainter.setColor(touchgfx::Color::getColorFromRGB(217, 55, 23));
	modelListener->updateRAMUsageProgress(SystemInfo.u16RAMUsage);
	modelListener->updateCPUUsageProgress(SystemInfo.u16CPUUsage);
	modelListener->updateGPUUsageProgress(SystemInfo.u16GPUUsage);
	modelListener->updateSystemTime(SystemInfo.u8SysTimeHr, SystemInfo.u8SysTimeMin, SystemInfo.u8SysTimeSec);
}
