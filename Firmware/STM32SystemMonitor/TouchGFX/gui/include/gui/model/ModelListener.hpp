#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>
#include <touchgfx/hal/Types.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void updateGraph(uint16_t value) {}
    virtual void updateRAMUsageProgress(uint16_t value) {}
    virtual void updateCPUUsageProgress(uint16_t value) {}
    virtual void updateGPUUsageProgress(uint16_t value) {}
    virtual void updateSystemTime(uint8_t hr, uint8_t min, uint8_t sec) {}
    virtual void updateCPUTempGraph(float value) {}
    virtual void updateGPUTempGraph(float value) {}
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
